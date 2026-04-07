#include <gtest/gtest.h>
#include "device/state_machine.hpp"

using namespace etc;

// ── Valid Transitions ──────────────────────────────────────────────────────────

TEST(StateMachineTest, StartsInBootState) {
    StateMachine sm;
    EXPECT_EQ(sm.getState(), DeviceState::BOOT);
}

TEST(StateMachineTest, BootToIdle) {
    StateMachine sm;
    EXPECT_TRUE(sm.transition(DeviceEvent::BOOT_COMPLETE));
    EXPECT_EQ(sm.getState(), DeviceState::IDLE);
}

TEST(StateMachineTest, IdleToActive) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);
    EXPECT_TRUE(sm.transition(DeviceEvent::START));
    EXPECT_EQ(sm.getState(), DeviceState::ACTIVE);
}

TEST(StateMachineTest, ActiveToIdle) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);
    sm.transition(DeviceEvent::START);
    EXPECT_TRUE(sm.transition(DeviceEvent::STOP));
    EXPECT_EQ(sm.getState(), DeviceState::IDLE);
}

TEST(StateMachineTest, IdleToSleep) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);
    EXPECT_TRUE(sm.transition(DeviceEvent::SLEEP_REQUEST));
    EXPECT_EQ(sm.getState(), DeviceState::SLEEP);
}

TEST(StateMachineTest, SleepToIdle) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);
    sm.transition(DeviceEvent::SLEEP_REQUEST);
    EXPECT_TRUE(sm.transition(DeviceEvent::WAKE));
    EXPECT_EQ(sm.getState(), DeviceState::IDLE);
}

TEST(StateMachineTest, FullCycle) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);    // BOOT → IDLE
    sm.transition(DeviceEvent::START);             // IDLE → ACTIVE
    sm.transition(DeviceEvent::STOP);              // ACTIVE → IDLE
    sm.transition(DeviceEvent::SLEEP_REQUEST);     // IDLE → SLEEP
    sm.transition(DeviceEvent::WAKE);              // SLEEP → IDLE
    sm.transition(DeviceEvent::START);             // IDLE → ACTIVE
    EXPECT_EQ(sm.getState(), DeviceState::ACTIVE);
}

// ── Error State ────────────────────────────────────────────────────────────────

TEST(StateMachineTest, ActiveToError) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);
    sm.transition(DeviceEvent::START);
    EXPECT_TRUE(sm.transition(DeviceEvent::FAULT));
    EXPECT_EQ(sm.getState(), DeviceState::ERROR);
}

TEST(StateMachineTest, ErrorRecovery) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);
    sm.transition(DeviceEvent::START);
    sm.transition(DeviceEvent::FAULT);
    EXPECT_TRUE(sm.transition(DeviceEvent::RECOVER));
    EXPECT_EQ(sm.getState(), DeviceState::IDLE);
}

TEST(StateMachineTest, FaultFromIdle) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);
    EXPECT_TRUE(sm.transition(DeviceEvent::FAULT));
    EXPECT_EQ(sm.getState(), DeviceState::ERROR);
}

// ── Invalid Transitions ────────────────────────────────────────────────────────

TEST(StateMachineTest, CannotStartFromBoot) {
    StateMachine sm;
    EXPECT_FALSE(sm.transition(DeviceEvent::START));
    EXPECT_EQ(sm.getState(), DeviceState::BOOT);
}

TEST(StateMachineTest, CannotStopFromIdle) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);
    EXPECT_FALSE(sm.transition(DeviceEvent::STOP));
    EXPECT_EQ(sm.getState(), DeviceState::IDLE);
}

TEST(StateMachineTest, CannotRecoverFromActive) {
    StateMachine sm;
    sm.transition(DeviceEvent::BOOT_COMPLETE);
    sm.transition(DeviceEvent::START);
    EXPECT_FALSE(sm.transition(DeviceEvent::RECOVER));
    EXPECT_EQ(sm.getState(), DeviceState::ACTIVE);
}

// ── State Names ────────────────────────────────────────────────────────────────

TEST(StateMachineTest, StateNameReturnsCorrectStrings) {
    EXPECT_EQ(StateMachine::stateName(DeviceState::BOOT),   "BOOT");
    EXPECT_EQ(StateMachine::stateName(DeviceState::IDLE),   "IDLE");
    EXPECT_EQ(StateMachine::stateName(DeviceState::ACTIVE), "ACTIVE");
    EXPECT_EQ(StateMachine::stateName(DeviceState::ERROR),  "ERROR");
    EXPECT_EQ(StateMachine::stateName(DeviceState::SLEEP),  "SLEEP");
}
