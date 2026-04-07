cmake_minimum_required(VERSION 3.14)
project(EmbeddedTelemetryController VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Library target ──────────────────────────────────────────────────────────────
add_library(etclib
    src/utils/logger.cpp
    src/drivers/temperature_sensor.cpp
    src/drivers/pressure_sensor.cpp
    src/drivers/battery_monitor.cpp
    src/comm/crc16.cpp
    src/comm/packet.cpp
    src/comm/uart_sim.cpp
    src/device/state_machine.cpp
    src/device/device_controller.cpp
    src/device/scheduler.cpp
    src/storage/eeprom_sim.cpp
)

target_include_directories(etclib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ── Main executable ─────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE etclib)

# ── Tests ───────────────────────────────────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    # Prevent GoogleTest from overriding our compiler/linker options
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(tests
        tests/test_crc.cpp
        tests/test_packet.cpp
        tests/test_state_machine.cpp
    )

    target_link_libraries(tests PRIVATE etclib GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
