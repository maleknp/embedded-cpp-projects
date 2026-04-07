#include "device/device_controller.hpp"
#include "utils/logger.hpp"

int main() {
    auto& log = etc::Logger::instance();
    log.setLevel(etc::LogLevel::INFO);

    etc::DeviceController device;

    if (!device.init()) {
        log.error("MAIN", "Device initialization failed. Exiting.");
        return 1;
    }

    // Run a demo with 20 iterations
    device.runDemo(20);

    return 0;
}
