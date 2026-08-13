#include <iostream>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <string>

using namespace std;

// Strategy interface
class RateLimitingStrategy {
public:
    virtual bool allowRequest(const string& userId) = 0;
    virtual ~RateLimitingStrategy() = default;
};


// Configuration
struct RateLimitConfig {
    int limit;
    long long windowSizeMs;
};


// Fixed Window Strategy
class FixedWindowStrategy : public RateLimitingStrategy {

private:
    struct UserState {
        int count;
        long long windowStart;
    };

    RateLimitConfig config;

    unordered_map<string, UserState> users;

    mutex mtx;

    long long getCurrentTime() {
        return chrono::duration_cast<chrono::milliseconds>(
            chrono::steady_clock::now().time_since_epoch()
        ).count();
    }

public:
    FixedWindowStrategy(const RateLimitConfig& config)
        : config(config) {}

    bool allowRequest(const string& userId) override {

        lock_guard<mutex> lock(mtx);

        long long now = getCurrentTime();

        auto it = users.find(userId);

        // First request from this user
        if (it == users.end()) {

            users[userId] = {1, now};

            return true;
        }

        UserState& state = it->second;

        // Current window expired
        if (now - state.windowStart >= config.windowSizeMs) {

            state.count = 1;
            state.windowStart = now;

            return true;
        }

        // Limit reached
        if (state.count >= config.limit) {
            return false;
        }

        // Allow request
        state.count++;

        return true;
    }
};


// RateLimiter
class RateLimiter {

private:
    unique_ptr<RateLimitingStrategy> strategy;

public:
    RateLimiter(unique_ptr<RateLimitingStrategy> strategy)
        : strategy(move(strategy)) {}

    bool allowRequest(const string& userId) {
        return strategy->allowRequest(userId);
    }
};


// Client
class Client {

private:
    RateLimiter& rateLimiter;

public:
    Client(RateLimiter& rateLimiter)
        : rateLimiter(rateLimiter) {}

    void sendRequest(const string& userId) {

        if (rateLimiter.allowRequest(userId)) {
            cout << "Request allowed\n";
        } 
        else {
            cout << "Request rejected\n";
        }
    }
};


int main() {

    // 5 requests per 60 seconds
    RateLimitConfig config{
        5,
        60'000
    };

    // Choose algorithm
    auto strategy =
        make_unique<FixedWindowStrategy>(config);

    // RateLimiter owns the strategy
    RateLimiter rateLimiter(move(strategy));

    // Client uses RateLimiter
    Client client(rateLimiter);

    // Send requests
    for (int i = 0; i < 7; i++) {
        client.sendRequest("user1");
    }

    return 0;
}