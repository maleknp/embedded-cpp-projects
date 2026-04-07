#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <vector>

namespace etc {

enum class DeviceState : uint8_t {
    BOOT   = 0,
    IDLE   = 1,
    ACTIVE = 2,
    ERROR  = 3,
    SLEEP  = 4,
};

enum class DeviceEvent : uint8_t {
    BOOT_COMPLETE,
    START,
    STOP,
    FAULT,
    RECOVER,
    SLEEP_REQUEST,
    WAKE,
};

class StateMachine {
public:
    StateMachine();

    /// Attempt a state transition. Returns true if transition was valid.
    bool transition(DeviceEvent event);

    DeviceState getState() const { return state_; }

    static std::string stateName(DeviceState s);
    static std::string eventName(DeviceEvent e);

private:
    struct Transition {
        DeviceState from;
        DeviceEvent event;
        DeviceState to;
    };

    DeviceState state_ = DeviceState::BOOT;
    std::vector<Transition> table_;

    void buildTransitionTable();
};

} // namespace etc
