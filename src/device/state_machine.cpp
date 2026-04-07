#include "device/state_machine.hpp"
#include "utils/logger.hpp"

namespace etc {

StateMachine::StateMachine() {
    buildTransitionTable();
    Logger::instance().info("SM", "State machine initialized in BOOT state");
}

void StateMachine::buildTransitionTable() {
    table_ = {
        // From        Event                 To
        { DeviceState::BOOT,   DeviceEvent::BOOT_COMPLETE, DeviceState::IDLE   },
        { DeviceState::IDLE,   DeviceEvent::START,         DeviceState::ACTIVE },
        { DeviceState::IDLE,   DeviceEvent::SLEEP_REQUEST, DeviceState::SLEEP  },
        { DeviceState::ACTIVE, DeviceEvent::STOP,          DeviceState::IDLE   },
        { DeviceState::ACTIVE, DeviceEvent::FAULT,         DeviceState::ERROR  },
        { DeviceState::ERROR,  DeviceEvent::RECOVER,       DeviceState::IDLE   },
        { DeviceState::SLEEP,  DeviceEvent::WAKE,          DeviceState::IDLE   },
        // Any state can enter ERROR on fault (except ERROR itself)
        { DeviceState::BOOT,   DeviceEvent::FAULT,         DeviceState::ERROR  },
        { DeviceState::IDLE,   DeviceEvent::FAULT,         DeviceState::ERROR  },
        { DeviceState::SLEEP,  DeviceEvent::FAULT,         DeviceState::ERROR  },
    };
}

bool StateMachine::transition(DeviceEvent event) {
    for (const auto& t : table_) {
        if (t.from == state_ && t.event == event) {
            Logger::instance().info("SM",
                "Transition: " + stateName(state_) + " --[" + eventName(event) + "]--> " + stateName(t.to));
            state_ = t.to;
            return true;
        }
    }

    Logger::instance().warn("SM",
        "Invalid transition: " + stateName(state_) + " --[" + eventName(event) + "]--> ???");
    return false;
}

std::string StateMachine::stateName(DeviceState s) {
    switch (s) {
        case DeviceState::BOOT:   return "BOOT";
        case DeviceState::IDLE:   return "IDLE";
        case DeviceState::ACTIVE: return "ACTIVE";
        case DeviceState::ERROR:  return "ERROR";
        case DeviceState::SLEEP:  return "SLEEP";
    }
    return "UNKNOWN";
}

std::string StateMachine::eventName(DeviceEvent e) {
    switch (e) {
        case DeviceEvent::BOOT_COMPLETE: return "BOOT_COMPLETE";
        case DeviceEvent::START:         return "START";
        case DeviceEvent::STOP:          return "STOP";
        case DeviceEvent::FAULT:         return "FAULT";
        case DeviceEvent::RECOVER:       return "RECOVER";
        case DeviceEvent::SLEEP_REQUEST: return "SLEEP_REQUEST";
        case DeviceEvent::WAKE:          return "WAKE";
    }
    return "UNKNOWN";
}

} // namespace etc
