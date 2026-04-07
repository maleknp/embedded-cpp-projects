# Embedded Telemetry Controller

A simulated embedded device system in C++ that reads sensor data (temperature, pressure, battery), processes it through a cooperative scheduler, and transmits telemetry packets over a simulated UART interface with CRC-16 integrity checks.

Built with clean embedded design patterns: sensor abstraction, state machines, binary protocols, and file-backed persistent storage.

---

## Features

- **Sensor Drivers** — Simulated temperature, pressure, and battery sensors with realistic noise
- **UART Communication** — Binary packet protocol with header, payload, and CRC-16/CCITT-FALSE
- **Device State Machine** — BOOT → IDLE → ACTIVE → ERROR → SLEEP transitions
- **Cooperative Scheduler** — Time-based task dispatch (no threads, embedded-style)
- **EEPROM Simulation** — File-backed key-value config storage
- **Command Handling** — GET_STATUS, GET_SENSOR_DATA, SET_CONFIG, RESET_DEVICE
- **Unit Tests** — GoogleTest suite for CRC, packet encode/decode, state machine

---

## Project Structure

```
├── CMakeLists.txt
├── README.md
├── docs/
│   └── protocol.md              # Full packet protocol specification
├── src/
│   ├── main.cpp                  # Entry point & demo
│   ├── device/
│   │   ├── device_controller.cpp # Central orchestrator
│   │   ├── state_machine.cpp     # Device state management
│   │   └── scheduler.cpp         # Cooperative task scheduler
│   ├── drivers/
│   │   ├── temperature_sensor.cpp
│   │   ├── pressure_sensor.cpp
│   │   └── battery_monitor.cpp
│   ├── comm/
│   │   ├── uart_sim.cpp          # Simulated UART with TX/RX FIFOs
│   │   ├── packet.cpp            # Packet encode/decode
│   │   └── crc16.cpp             # CRC-16/CCITT-FALSE (bit-by-bit)
│   ├── storage/
│   │   └── eeprom_sim.cpp        # File-backed config storage
│   └── utils/
│       └── logger.cpp            # Timestamped logging with severity
├── include/                      # Headers mirror src/ layout
├── tests/
│   ├── test_crc.cpp
│   ├── test_packet.cpp
│   └── test_state_machine.cpp
└── .github/workflows/
    └── build.yml                 # CI: build + test on push/PR
```

---

## Build

### Requirements

- C++17 compiler (GCC ≥ 9, Clang ≥ 10, MSVC ≥ 2019)
- CMake ≥ 3.14
- Internet connection (GoogleTest is fetched automatically)

### Build & Run

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run the demo
./build/EmbeddedTelemetryController
```

### Run Tests

```bash
cd build
ctest --output-on-failure
```

---

## Example Output

```
10:30:15.042 [INFO ] [DEV] ======================================
10:30:15.042 [INFO ] [DEV]   Embedded Telemetry Controller v1.0
10:30:15.042 [INFO ] [DEV] ======================================
10:30:15.042 [INFO ] [EEPROM] Defaults loaded
10:30:15.042 [INFO ] [TEMP] Initializing temperature sensor
10:30:15.042 [INFO ] [PRESS] Initializing pressure sensor
10:30:15.042 [INFO ] [BATT] Initializing battery monitor
10:30:15.042 [INFO ] [SM] Transition: BOOT --[BOOT_COMPLETE]--> IDLE
10:30:15.042 [INFO ] [SM] Transition: IDLE --[START]--> ACTIVE
10:30:15.242 [INFO ] [UART] TX [19 bytes]: aa 55 0f 01 82 ...
```

---

## Protocol

See [docs/protocol.md](docs/protocol.md) for the full communication protocol specification, including:
- Packet format
- Command list
- Payload structures
- Example hex packets
- State transition diagram

---

## Architecture

```
┌─────────────────────────────────────────┐
│           DeviceController              │
│  ┌───────────┐  ┌────────────────────┐  │
│  │ Scheduler │  │   StateMachine     │  │
│  │           │  │ BOOT→IDLE→ACTIVE   │  │
│  └─────┬─────┘  └────────────────────┘  │
│        │                                │
│  ┌─────▼──────────────────────────┐     │
│  │         Sensor Layer           │     │
│  │  Temp │ Pressure │ Battery     │     │
│  └────────────────────────────────┘     │
│                                         │
│  ┌────────────────────────────────┐     │
│  │    Communication Layer         │     │
│  │  UART ←→ Packet ←→ CRC16      │     │
│  └────────────────────────────────┘     │
│                                         │
│  ┌────────────────────────────────┐     │
│  │    EEPROM Storage              │     │
│  │    (file-backed config)        │     │
│  └────────────────────────────────┘     │
└─────────────────────────────────────────┘
```

---

## License

MIT
