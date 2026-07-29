#include <iostream>
#include <vector>
#include <memory>
#include <string>

using namespace std;

/*
====================================================
Types of parking spots.
Vehicle classes will tell us which spot they require.
====================================================
*/
enum class SpotType {
    MOTORCYCLE,
    COMPACT,
    LARGE
};

/*
====================================================
Abstract Vehicle

Why abstract?
-------------
We never create a generic Vehicle.
Every vehicle is either a Car, Motorcycle or Truck.

Instead of exposing VehicleType enums and writing
if/switch everywhere, we use polymorphism.
====================================================
*/
class Vehicle {
protected:
    string licenseNumber;

public:
    Vehicle(string license)
        : licenseNumber(license) {}

    virtual SpotType requiredSpotType() const = 0;

    string getLicenseNumber() const {
        return licenseNumber;
    }

    virtual ~Vehicle() = default;
};

/*
====================================================
Each derived class simply tells what spot it needs.
====================================================
*/

class Motorcycle : public Vehicle {
public:
    Motorcycle(string license)
        : Vehicle(license) {}

    SpotType requiredSpotType() const override {
        return SpotType::MOTORCYCLE;
    }
};

class Car : public Vehicle {
public:
    Car(string license)
        : Vehicle(license) {}

    SpotType requiredSpotType() const override {
        return SpotType::COMPACT;
    }
};

class Truck : public Vehicle {
public:
    Truck(string license)
        : Vehicle(license) {}

    SpotType requiredSpotType() const override {
        return SpotType::LARGE;
    }
};

/*
====================================================
ParkingSpot

Responsibilities
----------------
1. Knows its own type.
2. Knows whether it is occupied.
3. Decides if a vehicle can park here.

Notice:
ParkingLot DOES NOT decide parking rules.
ParkingSpot owns this responsibility.
====================================================
*/

class ParkingSpot {

    int id;
    SpotType type;
    bool occupied;

    // Pointer to currently parked vehicle.
    // Raw pointer because ParkingSpot does NOT own Vehicle.
    Vehicle* parkedVehicle;

public:

    ParkingSpot(int id, SpotType type)
        : id(id),
          type(type),
          occupied(false),
          parkedVehicle(nullptr) {}

    /*
    ====================================================
    Tell Don't Ask Principle

    Instead of

    ParkingLot:
        if(car)
        if(truck)

    We ask the spot.

        spot->canPark(vehicle)
    ====================================================
    */
    bool canPark(const Vehicle& vehicle) const {

        if (occupied)
            return false;

        SpotType vehicleType = vehicle.requiredSpotType();

        // Large spots can hold trucks and cars.
        if (type == SpotType::LARGE) {
            return vehicleType == SpotType::LARGE ||
                   vehicleType == SpotType::COMPACT;
        }

        return vehicleType == type;
    }

    void parkVehicle(Vehicle* vehicle) {
        occupied = true;
        parkedVehicle = vehicle;
    }

    void removeVehicle() {
        occupied = false;
        parkedVehicle = nullptr;
    }

    bool isOccupied() const {
        return occupied;
    }

    SpotType getSpotType() const {
        return type;
    }

    int getId() const {
        return id;
    }
};

/*
====================================================
ParkingFloor

Responsibilities
----------------
Owns all parking spots on one floor.

Why unique_ptr?

ParkingFloor is the ONLY owner of ParkingSpot.

No other object should delete a ParkingSpot.

Hence unique_ptr expresses ownership perfectly.
====================================================
*/

class ParkingFloor {

    vector<unique_ptr<ParkingSpot>> spots;

public:

    void addSpot(unique_ptr<ParkingSpot> spot) {
        spots.push_back(move(spot));
    }

    /*
    ====================================================
    Returns a raw pointer.

    Why not unique_ptr?

    Because caller DOES NOT own the spot.

    Ownership stays with ParkingFloor.

    We are only lending access.
    ====================================================
    */

    ParkingSpot* findAvailableSpot(const Vehicle& vehicle) {

        for (auto& spot : spots) {

            if (spot->canPark(vehicle))
                return spot.get();
        }

        return nullptr;
    }
};