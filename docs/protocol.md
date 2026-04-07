# UART Communication Protocol

## Overview

The Embedded Telemetry Controller uses a custom binary protocol over simulated UART for host ↔ device communication.

---

## Packet Format

```
+----------+----------+--------+-----------+---------+------------------+----------+----------+
| HEADER_0 | HEADER_1 | LENGTH | DEVICE_ID | COMMAND | PAYLOAD (0..N)   | CRC_HIGH | CRC_LOW  |
| 0xAA     | 0x55     | 1 byte | 1 byte    | 1 byte  | variable         | 1 byte   | 1 byte   |
+----------+----------+--------+-----------+---------+------------------+----------+----------+
```

### Field Descriptions

| Field       | Size     | Description                                                               |
|-------------|----------|---------------------------------------------------------------------------|
| `HEADER_0`  | 1 byte   | Fixed `0xAA`                                                              |
| `HEADER_1`  | 1 byte   | Fixed `0x55`                                                              |
| `LENGTH`    | 1 byte   | Number of bytes from `DEVICE_ID` to end of `PAYLOAD` (inclusive)           |
| `DEVICE_ID` | 1 byte   | Unique device identifier (default `0x01`)                                 |
| `COMMAND`   | 1 byte   | Command/response code (see table below)                                   |
| `PAYLOAD`   | 0–250 B  | Command-specific data                                                     |
| `CRC_HIGH`  | 1 byte   | CRC-16/CCITT-FALSE high byte                                              |
| `CRC_LOW`   | 1 byte   | CRC-16/CCITT-FALSE low byte                                               |

### CRC Computation

CRC is computed over: `LENGTH + DEVICE_ID + COMMAND + PAYLOAD`

- **Algorithm:** CRC-16/CCITT-FALSE
- **Polynomial:** `0x1021`
- **Initial value:** `0xFFFF`

---

## Commands

| Code   | Name              | Direction     | Description                         |
|--------|-------------------|---------------|-------------------------------------|
| `0x01` | `GET_STATUS`      | Host → Device | Request device status               |
| `0x02` | `GET_SENSOR_DATA` | Host → Device | Request latest sensor readings      |
| `0x03` | `SET_CONFIG`      | Host → Device | Write configuration parameter       |
| `0x04` | `RESET_DEVICE`    | Host → Device | Reset device state                  |

### Response Format

Responses use the same command code with bit 7 set (OR'd with `0x80`):

| Code   | Name                  | Description                |
|--------|-----------------------|----------------------------|
| `0x81` | `GET_STATUS_RSP`      | Status response            |
| `0x82` | `GET_SENSOR_DATA_RSP` | Sensor data response       |
| `0x83` | `SET_CONFIG_RSP`      | Config write confirmation  |
| `0x84` | `RESET_DEVICE_RSP`    | Reset confirmation         |

---

## Payload Formats

### GET_STATUS (0x01)

**Request:** No payload.

**Response (0x81):**

| Byte | Description                      |
|------|----------------------------------|
| 0    | Device state (0=BOOT..4=SLEEP)   |
| 1    | Tick count (low byte)            |

### GET_SENSOR_DATA (0x02)

**Request:** No payload.

**Response (0x82):**

| Bytes | Format         | Description                      |
|-------|----------------|----------------------------------|
| 0–3   | `float` (LE)   | Temperature (°C)                 |
| 4–7   | `float` (LE)   | Pressure (hPa)                   |
| 8–11  | `float` (LE)   | Battery voltage (V)              |
| 12    | `uint8_t`      | Device state                     |

### SET_CONFIG (0x03)

**Request:**

| Byte  | Description                          |
|-------|--------------------------------------|
| 0     | Key length (N)                       |
| 1..N  | Key string (ASCII)                   |
| N+1   | Value length (M)                     |
| N+2.. | Value string (ASCII)                 |

**Response (0x83):**

| Byte | Description                        |
|------|------------------------------------|
| 0    | `0x00` = success, `0xFF` = error   |

### RESET_DEVICE (0x04)

**Request:** No payload.

**Response (0x84):**

| Byte | Description                   |
|------|-------------------------------|
| 0    | `0x00` = success              |

---

## Example Packets (Hex)

### GET_STATUS Request

```
AA 55 02 01 01 [CRC_HI] [CRC_LO]
```

- Header: `AA 55`
- Length: `02` (device_id + command = 2 bytes)
- Device ID: `01`
- Command: `01` (GET_STATUS)

### GET_SENSOR_DATA Response

```
AA 55 0F 01 82 [temp_4B] [press_4B] [batt_4B] [state_1B] [CRC_HI] [CRC_LO]
```

- Length: `0x0F` (15 = 2 + 13 payload bytes)
- Command: `0x82` (GET_SENSOR_DATA_RSP)

---

## Device States

| Value | State    | Description                |
|-------|----------|----------------------------|
| 0     | `BOOT`   | Initial boot sequence      |
| 1     | `IDLE`   | Ready, not actively sensing|
| 2     | `ACTIVE` | Reading sensors, sending telemetry |
| 3     | `ERROR`  | Fault detected             |
| 4     | `SLEEP`  | Low-power mode             |

---

## State Transitions

```
BOOT ──[BOOT_COMPLETE]──> IDLE
IDLE ──[START]──────────> ACTIVE
IDLE ──[SLEEP_REQUEST]──> SLEEP
ACTIVE ──[STOP]─────────> IDLE
ACTIVE ──[FAULT]────────> ERROR
ERROR ──[RECOVER]───────> IDLE
SLEEP ──[WAKE]──────────> IDLE
Any* ──[FAULT]──────────> ERROR
```

\* Except ERROR state itself.
