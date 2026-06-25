# Hardware And API Reference

This document describes the current M5StickC Plus2 hardware contract and firmware API used by the Palladio Physics Lab dashboard. It is documentation-only: it records the current implementation so future hardware, UI, and performance work can be reviewed against a shared map.

## Current Hardware Role

The M5StickC Plus2 is the measurement device and local API server.

- It creates the Wi-Fi network `PhysicsLab-M5`.
- It serves HTTP on `http://192.168.4.1`.
- It samples the active sensor mode at the configured rate.
- It exposes the latest reading and a rolling sample buffer through HTTP.
- It still broadcasts UDP packets for the legacy Python desktop app.

The full classroom dashboard in `docs/index.html` defaults to `http://192.168.4.1`, but can be pointed to another device URL with:

```text
?device=http://192.168.4.1
```

The selected device URL is stored in browser `localStorage` under `palladio-device-url`.

## Sensor Modes

| Mode | Firmware name | Dashboard use | Hardware |
| --- | --- | --- | --- |
| `0` | `Acceleration` | Acceleration, force-from-acceleration activities | Built-in M5StickC Plus2 IMU |
| `1` | `Gyroscope` | Rotation / angular velocity | Built-in M5StickC Plus2 IMU |
| `2` | `Load Cell` | Force, Hooke's law, buoyancy force readings | HX711 + load cell |
| `3` | `WiFi Info` | Device network status on the M5 display | M5StickC Plus2 only |
| `4` | `Ultrasonic` | Distance, speed, acceleration, sonar-style activities | DFRobot URM10 ultrasonic sensor |
| `5` | `Temperature` | Heat / thermal-equilibrium activities | Two DS18B20 temperature sensors |

The M5 buttons also cycle modes:

- Button A: next mode
- Button B: previous mode

Changing mode clears the sample buffer and resets mode-specific state.

## Pin Map

Current firmware definitions:

```cpp
#define TRIG_PIN 26
#define ECHO_PIN 25
#define DT 36
#define DT_ALT 25
#define SCK 26
#define TEMP1_PIN 25
#define TEMP2_PIN 26
```

| Sensor | Signal | M5 pin | Notes |
| --- | --- | --- | --- |
| DFRobot URM10 | Trigger | `G26` | Shared with HX711 SCK and DS18B20 T2 in the current firmware. |
| DFRobot URM10 | Echo | `G25` | Input. Powered from 3V in the current classroom wiring. Shared with HX711 alternate DOUT and DS18B20 T1. |
| HX711 | DOUT primary | `G36` | Firmware starts here. |
| HX711 | DOUT alternate | `G25` | Firmware can switch here if the primary path looks unavailable. |
| HX711 | SCK | `G26` | Shared with ultrasonic trigger and DS18B20 T2. |
| DS18B20 T1 | Data | `G25` | Shared with HX711 alternate DOUT. |
| DS18B20 T2 | Data | `G26` | Shared with HX711 SCK and ultrasonic trigger. |

Because some pins are shared, treat the current setup as one active external sensor family at a time unless a final wiring harness proves otherwise.
In particular, the DFRobot URM10 and the two DS18B20 temperature sensors both occupy `G25/G26`; disconnect one sensor family before testing the other.

On startup the firmware runs a conservative sensor auto-detection pass. It checks for two DS18B20 sensors first, then HX711, then URM10. If exactly one sensor family is found, the M5 selects that mode automatically. If nothing is found or the wiring is ambiguous, it falls back to manual mode selection.

## Hardware Notes By Experiment

### Built-In IMU

Modes:

- `0` acceleration
- `1` gyroscope

No external wiring is required. The firmware converts acceleration readings to m/s² by multiplying raw accelerometer values by `9.81`.

### HX711 Load Cell

Mode:

- `2` load cell / force

Current firmware behavior:

- Uses `SCALE_FACTOR = 48163.7`.
- Starts with `DT = 36`, `SCK = 26`.
- If the HX711 is not ready or keeps returning raw zero, the firmware can try `DT_ALT = 25`.
- `calibrate?target=force` runs `scale.tare(10)`.
- Force is reported in newtons as `force`.

Open verification items:

- Confirm the load-cell capacity and mechanical mounting.
- Confirm final DOUT pin for the classroom harness.
- Calibrate `SCALE_FACTOR` against known masses before treating force values as final.

### Ultrasonic Sensor

Mode:

- `4` ultrasonic

Current firmware behavior:

- Trigger pin: `G26`.
- Echo pin: `G25`.
- Sends a 10 microsecond trigger pulse.
- Uses `pulseIn(ECHO_PIN, HIGH, 30000)`.
- Converts duration to distance with speed of sound in air: `duration * 0.0343 / 2`.
- Rejects readings below 2 cm or above 200 cm.
- Applies smoothing: previous smoothed value `60%`, new reading `40%`.
- Computes distance in meters, velocity in m/s, and acceleration in m/s² from sample timing.
- `calibrate?target=distance` zeroes the current distance as an offset and resets velocity/acceleration state.

Open verification items:

- Confirm the URM10 is powered from the M5 `3V` pin so the echo signal stays 3.3 V logic compatible.
- Test known distances in the intended classroom range.
- Confirm that smoothing is appropriate for fast-motion experiments.

### DS18B20 Temperature Sensors

Mode:

- `5` temperature

Current firmware behavior:

- Sensor 1 data pin: `G25`.
- Sensor 2 data pin: `G26`.
- Uses two OneWire buses and DallasTemperature.
- Sets both sensors to 11-bit resolution.
- Reports `temp1`, `temp2`, and `tempDelta = temp1 - temp2`.
- Skips a sample if either sensor reports disconnected.

Open verification items:

- Confirm whether the final harness uses one shared OneWire bus or two separate buses.
- Confirm pull-up resistor placement.
- Test response time in the actual thermal-equilibrium setup.

## HTTP API

Base URL when connected to the M5 access point:

```text
http://192.168.4.1
```

The firmware includes CORS headers:

- `Access-Control-Allow-Origin: *`
- `Access-Control-Allow-Methods: GET, OPTIONS`
- `Access-Control-Allow-Headers: Content-Type`
- `Access-Control-Allow-Private-Network: true`

### `GET /`

Returns the lightweight firmware-served status page from `Physics_Lab/web_dashboard.h`.

This is not the full classroom dashboard in `docs/index.html`.

### `GET /logo.png`

Returns the embedded Palladio logo from `logo_asset.h`.

### `GET /data`

Returns the latest device state and latest sample values.

Example shape:

```json
{
  "mode": 0,
  "modeName": "Acceleration",
  "ap": "PhysicsLab-M5",
  "ip": "192.168.4.1",
  "battery": 4.05,
  "sampleId": 123,
  "sampleMs": 45678,
  "sampleRate": 10,
  "ax": 0.012,
  "ay": -0.034,
  "az": 9.801,
  "gx": 0.000,
  "gy": 0.000,
  "gz": 0.000,
  "force": 0.000,
  "distance": 0.000,
  "velocity": 0.000,
  "acceleration": 0.000,
  "temp1": 0.000,
  "temp2": 0.000,
  "tempDelta": 0.000
}
```

Notes:

- Values for inactive sensor families may remain from the last sampled value or default to zero.
- `sampleMs` is firmware time from `millis()`, not browser time.
- `sampleId` increases only when the active mode produces a valid sample.

### `GET /samples`

Returns the rolling sample buffer.

```text
GET /samples
GET /samples?since=<sampleId>
```

Response shape:

```json
{
  "mode": 4,
  "modeName": "Ultrasonic",
  "ap": "PhysicsLab-M5",
  "ip": "192.168.4.1",
  "battery": 4.02,
  "sampleRate": 10,
  "latestId": 123,
  "bufferCount": 120,
  "samples": [
    {
      "mode": 4,
      "sampleId": 122,
      "sampleMs": 45600,
      "sampleRate": 10,
      "battery": 4.02,
      "ax": 0.000,
      "ay": 0.000,
      "az": 0.000,
      "gx": 0.000,
      "gy": 0.000,
      "gz": 0.000,
      "force": 0.000,
      "distance": 0.340,
      "velocity": 0.120,
      "acceleration": -0.030,
      "temp1": 0.000,
      "temp2": 0.000,
      "tempDelta": 0.000
    }
  ]
}
```

Notes:

- The firmware stores up to 512 samples in a circular buffer.
- `since` filters samples with `sampleId > since`.
- If there are no newer samples, `samples` is an empty array.
- The dashboard polls this endpoint first and falls back to `/data`.

### `GET /set-mode?mode=<n>`

Switches active mode and returns the same shape as `/data`.

Valid modes:

- `0` acceleration
- `1` gyroscope
- `2` load cell
- `3` Wi-Fi info
- `4` ultrasonic
- `5` temperature

Changing mode:

- configures mode-specific hardware,
- clears the sample buffer,
- resets distance/velocity state,
- resets sampling schedule,
- updates the M5 display.

### `GET /settings?rate=<hz>`

Updates sample rate and returns the same shape as `/data`.

Accepted rates:

- `1`
- `2`
- `5`
- `10`
- `20`
- `50`
- `100`

Notes:

- The default rate is 10 Hz.
- Entering load-cell mode forces the rate back to 10 Hz.
- The dashboard exposes the same values in its sampling-rate dropdown.

### `GET /calibrate?target=<name>`

Runs calibration/zeroing for the requested target and returns the same shape as `/data`.

Valid targets:

| Target | Effect |
| --- | --- |
| `accel` | Stores current raw acceleration as offset and reports zeroed acceleration. |
| `gyro` | Stores current raw gyroscope values as offset and reports zeroed angular velocity. |
| `force` | Runs HX711 tare with 10 samples and resets force zero state. |
| `distance` | Adds current distance to the distance offset and resets velocity/acceleration state. |

The current dashboard maps calibration buttons by active mode:

- mode `0` -> `accel`
- mode `1` -> `gyro`
- mode `2` -> `force`
- mode `4` -> `distance`

## UDP Compatibility

The firmware continues to broadcast UDP packets to:

```text
192.168.4.255
```

Current ports:

| Port | Mode | Payload |
| --- | --- | --- |
| `4210` | acceleration | `ax,ay,az,battery` |
| `4211` | gyroscope | `gx,gy,gz,battery` |
| `4213` | load cell | `force,battery` |
| `4215` | ultrasonic | `distance,velocity,acceleration` |
| `4216` | temperature | `temp1,temp2,tempDelta,battery` |

The legacy desktop app depends on these broadcasts. New browser work should prefer HTTP `/samples` and `/data`.

## Dashboard API Behavior

The full dashboard:

- builds API URLs from `deviceBaseUrl`;
- defaults to `http://192.168.4.1`;
- polls `/samples?since=<lastSampleId>`;
- falls back to `/data` if `/samples` fails;
- schedules polling adaptively from the active sample rate instead of using a fixed 30 ms hardware loop;
- polls more slowly while measurement is idle and backs off when the M5 is offline;
- temporarily uses `/data` first when `/samples` is unavailable, then retries `/samples` periodically;
- uses firmware `sampleId` and `sampleMs` to avoid duplicate samples and browser-time drift;
- sends `/set-mode`, `/settings`, and `/calibrate` through simple GET requests.

The pendulum experiment is digital-only and does not require M5 hardware.

## Verification Checklist

Before broad classroom use, verify with the actual hardware kit:

- M5 boots and advertises `PhysicsLab-M5`.
- Browser can reach `http://192.168.4.1/data`.
- `/samples?since=0` returns sample objects in each hardware mode.
- Built-in acceleration reads near `9.81 m/s²` on the gravity axis before zeroing.
- Gyroscope reads near zero when stationary after calibration.
- HX711 tare works and known masses produce stable force readings.
- Ultrasonic readings match known distances in the expected range.
- DS18B20 sensors both report plausible temperatures and `tempDelta` behaves as expected.
- Dashboard mode switching matches firmware mode switching.
- UDP compatibility still works for the legacy Python app when needed.

## Known Hardware/API Risks

- Several external sensor definitions share pins (`G25`, `G26`). Do not assume simultaneous external sensor wiring until a final harness is tested.
- URM10 echo voltage must be checked against the M5 input tolerance if the wiring changes from the current 3V setup.
- Load-cell calibration factor is currently code-defined and should be validated against known masses.
- Polling every 30 ms is UI-side behavior; firmware sample IDs protect data quality, but performance should still be measured on classroom devices.
- `/data` returns one flat object for all sensor families, so consumers should read only the fields relevant to the active mode.
- API changes should preserve `sampleId`, `sampleMs`, and `/samples?since=` semantics because the dashboard depends on them.
