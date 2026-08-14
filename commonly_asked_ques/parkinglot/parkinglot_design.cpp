#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <stdexcept>
#include <unordered_map>

using namespace std;

// ============================================================
// ENUMS
// ============================================================

enum class VehicleType {
    BIKE,
    CAR,
    TRUCK
};

enum class SpotType {
    BIKE,
    CAR,
    TRUCK
};


// ============================================================
// VEHICLE
// ============================================================

class Vehicle {
private:
    string licensePlate;
    VehicleType type;

public:
    Vehicle(string licensePlate, VehicleType type)
        : licensePlate(move(licensePlate)), type(type) {}

    const string& getLicensePlate() const {
        return licensePlate;
    }

    VehicleType getType() const {
        return type;
    }
};


// ============================================================
// PARKING SPOT
// ============================================================

class ParkingSpot {
private:
    int id;
    SpotType type;
    bool occupied;
    // Store the plate, not a Vehicle*: the caller owns the Vehicle and its
    // lifetime isn't guaranteed to outlast the parking duration, so holding
    // a raw pointer here would risk dangling.
    string occupantPlate;

public:
    ParkingSpot(int id, SpotType type)
        : id(id), type(type), occupied(false) {}

    int getId() const {
        return id;
    }

    SpotType getType() const {
        return type;
    }

    bool isAvailable() const {
        return !occupied;
    }

    void assignVehicle(const Vehicle& v) {
        occupantPlate = v.getLicensePlate();
        occupied = true;
    }

    void removeVehicle() {
        occupantPlate.clear();
        occupied = false;
    }
};


// ============================================================
// PARKING TICKET
// ============================================================

class ParkingTicket {
private:
    int ticketId;
    string licensePlate;
    int spotId;
    chrono::steady_clock::time_point entryTime;

public:
    ParkingTicket(
        int ticketId,
        const string& licensePlate,
        int spotId
    )
        : ticketId(ticketId),
          licensePlate(licensePlate),
          spotId(spotId),
          entryTime(chrono::steady_clock::now()) {}

    int getTicketId() const {
        return ticketId;
    }

    const string& getLicensePlate() const {
        return licensePlate;
    }

    int getSpotId() const {
        return spotId;
    }

    long long getDurationInHours() const {
        auto now = chrono::steady_clock::now();

        auto duration =
            chrono::duration_cast<chrono::hours>(
                now - entryTime
            );

        // Minimum 1 hour. duration.count()'s type (chrono::hours::rep) is
        // platform-defined (e.g. `long` rather than `long long` on some
        // targets), so match it explicitly instead of hardcoding 1LL -
        // std::max requires both arguments to be the exact same type.
        return max<long long>(1, duration.count());
    }
};


// ============================================================
// SPOT SELECTION STRATEGY
// ============================================================

class SpotSelectionStrategy {
public:
    virtual ParkingSpot* findSpot(
        vector<unique_ptr<ParkingSpot>>& spots,
        const Vehicle& vehicle
    ) = 0;

    virtual ~SpotSelectionStrategy() = default;
};


// ============================================================
// BIKE SPOT STRATEGY
// ============================================================

class BikeSpotSelectionStrategy
    : public SpotSelectionStrategy {

public:
    ParkingSpot* findSpot(
        vector<unique_ptr<ParkingSpot>>& spots,
        const Vehicle& vehicle
    ) override {

        for (auto& spot : spots) {

            if (spot->isAvailable() &&
                spot->getType() == SpotType::BIKE) {

                return spot.get();
            }
        }

        return nullptr;
    }
};


// ============================================================
// CAR SPOT STRATEGY
// ============================================================

class CarSpotSelectionStrategy
    : public SpotSelectionStrategy {

public:
    ParkingSpot* findSpot(
        vector<unique_ptr<ParkingSpot>>& spots,
        const Vehicle& vehicle
    ) override {

        for (auto& spot : spots) {

            if (spot->isAvailable() &&
                spot->getType() == SpotType::CAR) {

                return spot.get();
            }
        }

        return nullptr;
    }
};


// ============================================================
// TRUCK SPOT STRATEGY
// ============================================================

class TruckSpotSelectionStrategy
    : public SpotSelectionStrategy {

public:
    ParkingSpot* findSpot(
        vector<unique_ptr<ParkingSpot>>& spots,
        const Vehicle& vehicle
    ) override {

        for (auto& spot : spots) {

            if (spot->isAvailable() &&
                spot->getType() == SpotType::TRUCK) {

                return spot.get();
            }
        }

        return nullptr;
    }
};


// ============================================================
// PAYMENT
// ============================================================

class Payment {
public:
    bool pay(double amount) {

        cout << "Payment of Rs. "
             << amount
             << " successful\n";

        return true;
    }
};


// ============================================================
// PARKING LOT
// ============================================================

class ParkingLot {
private:
    vector<unique_ptr<ParkingSpot>> spots;

    // One strategy per vehicle type, instead of a single strategy fixed for
    // the whole lot. Picking the strategy by vehicle.getType() is what
    // actually makes sure a bike can't be handed a car spot and vice versa.
    unordered_map<VehicleType, unique_ptr<SpotSelectionStrategy>> strategies;

    int nextTicketId = 1;

public:

    void addSpot(unique_ptr<ParkingSpot> spot) {
        spots.push_back(move(spot));
    }

    void registerStrategy(
        VehicleType type,
        unique_ptr<SpotSelectionStrategy> strategy
    ) {
        strategies[type] = move(strategy);
    }

    unique_ptr<ParkingTicket> parkVehicle(Vehicle& vehicle) {

        auto it = strategies.find(vehicle.getType());

        if (it == strategies.end()) {
            cout << "No spot selection strategy registered "
                    "for this vehicle type\n";
            return nullptr;
        }

        ParkingSpot* spot =
            it->second->findSpot(spots, vehicle);

        if (spot == nullptr) {
            cout << "No suitable parking spot available\n";
            return nullptr;
        }

        spot->assignVehicle(vehicle);

        auto ticket = make_unique<ParkingTicket>(
            nextTicketId++,
            vehicle.getLicensePlate(),
            spot->getId()
        );

        cout << "Vehicle parked at spot "
             << spot->getId() << '\n';

        cout << "Ticket ID: "
             << ticket->getTicketId() << '\n';

        return ticket;
    }

    double calculateBill(const ParkingTicket& ticket) {

        long long hours =
            ticket.getDurationInHours();

        // Simple pricing for our interview design
        return hours * 50.0;
    }

    // Takes ownership of the ticket. Because it's a unique_ptr taken by
    // value, it is freed automatically when this function returns on
    // *every* path (success, payment failure, or spot lookup failure) -
    // no more manual delete, and no more leak on the failure paths.
    bool unparkVehicle(
        unique_ptr<ParkingTicket> ticket,
        Payment& payment
    ) {

        if (ticket == nullptr)
            return false;

        double amount = calculateBill(*ticket);

        cout << "Bill: Rs. "
             << amount << '\n';

        if (!payment.pay(amount)) {
            return false;
        }

        for (auto& spot : spots) {

            if (spot->getId() == ticket->getSpotId()) {

                spot->removeVehicle();

                cout << "Spot "
                     << spot->getId()
                     << " is now available\n";

                return true;
            }
        }

        return false;
    }
};


// ============================================================
// MAIN
// ============================================================

int main() {

    // Create parking lot and register a spot selection strategy per
    // vehicle type, so each vehicle can only ever be matched to its own
    // kind of spot.
    ParkingLot parkingLot;

    parkingLot.registerStrategy(
        VehicleType::CAR,
        make_unique<CarSpotSelectionStrategy>()
    );

    parkingLot.registerStrategy(
        VehicleType::BIKE,
        make_unique<BikeSpotSelectionStrategy>()
    );

    parkingLot.registerStrategy(
        VehicleType::TRUCK,
        make_unique<TruckSpotSelectionStrategy>()
    );


    // Add parking spots
    parkingLot.addSpot(
        make_unique<ParkingSpot>(
            1,
            SpotType::CAR
        )
    );

    parkingLot.addSpot(
        make_unique<ParkingSpot>(
            2,
            SpotType::CAR
        )
    );


    // Create vehicle
    Vehicle car(
        "UP14AB1234",
        VehicleType::CAR
    );


    // Park
    unique_ptr<ParkingTicket> ticket =
        parkingLot.parkVehicle(car);


    // Later...
    Payment payment;

    parkingLot.unparkVehicle(
        move(ticket),
        payment
    );

    return 0;
}