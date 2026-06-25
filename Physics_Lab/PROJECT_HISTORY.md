# Physics Lab Project History

Last updated: 2026-06-16

## Goal

Build a browser-based physics laboratory dashboard for the M5StickC Plus2. The M5Stick creates its own Wi-Fi network and serves the web app directly, so students connect to `PhysicsLab-M5` and open:

```text
http://192.168.4.1
```

## Current Status

The web app is embedded in the Arduino sketch and has been uploaded successfully to the M5StickC Plus2 several times during development.

Current main capabilities:

- Live web dashboard served from the M5StickC Plus2
- Modern sidebar UI with the Palladio logo
- Sensor modes: acceleration, gyroscope, load cell, ultrasonic motion
- Live chart with zoom, pan, series selection, and CSV export
- Analysis tools: A-B cursors, dt, dy, slope, mean, standard deviation, min/max, area, absolute area, fit equation, R2, RMSE
- Short educational examples next to each analysis/statistics tool
- Sampling-rate control: 5 Hz, 10 Hz, 20 Hz
- Firmware-side timestamps and sample IDs, so the browser does not create duplicate samples
- Calibration controls from the web UI:
  - accelerometer zero
  - gyroscope zero
  - load-cell tare
  - ultrasonic distance zero

## Files Changed Or Added

### `Physics_Lab.ino`

Main firmware sketch.

Important additions:

- Web endpoints:
  - `/`
  - `/logo.png`
  - `/data`
  - `/set-mode`
  - `/settings`
  - `/calibrate`
- Firmware-owned sampling:
  - `sampleId`
  - `sampleMs`
  - `sampleRate`
- Sampling interval control for 5/10/20 Hz
- Sensor zero/tare calibration handlers
- Serves the embedded Palladio logo as PNG

### `web_dashboard.h`

Embedded HTML/CSS/JavaScript for the web app.

Important additions:

- Full web app redesign
- Sidebar navigation and live readouts
- Palladio logo in the top-left sidebar
- Large central chart
- Chart tools:
  - inspect
  - select interval
  - pan
  - zoom in/out/live reset
- Analysis panel with statistics and physics examples
- CSV export
- Sampling-rate selector
- Calibration button that changes by sensor mode

### `logo_asset.h`

Generated C header containing the PNG logo bytes stored in flash memory.

### `assets/palladio-logo.png`

Optimized project copy of the Palladio logo used by the web app.

### `scripts/arduino.sh`

Helper script for compiling and uploading without opening Arduino IDE.

Common commands:

```bash
./scripts/arduino.sh compile
./scripts/arduino.sh upload /dev/cu.usbserial-5A490733351
```

### `.gitignore`

Ignores `.arduino/` build artifacts.

## Hardware / Board Setup

Detected board/core:

```text
M5StickCPlus2
FQBN: m5stack:esp32:m5stack_stickc_plus2
```

Arduino CLI is bundled inside Arduino IDE at:

```text
/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli
```

Typical USB port used:

```text
/dev/cu.usbserial-5A490733351
```

If upload fails because the port disappeared, run:

```bash
'/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli' board list
```

Then upload again with the detected USB serial port.

## Validation Already Done

The project has repeatedly passed:

- JavaScript syntax parsing via macOS JavaScript engine
- Arduino compile for M5StickCPlus2
- Upload with esptool hash verification

Recent compile size after logo and examples:

```text
Sketch uses about 37% of program storage.
Global variables use about 15% of dynamic memory.
```

## How To Use The Web App

1. Upload the sketch to the M5StickC Plus2.
2. Wait for the M5Stick to reboot.
3. Connect laptop/tablet/phone Wi-Fi to:

```text
PhysicsLab-M5
```

4. Open:

```text
http://192.168.4.1
```

5. If the old page appears, force refresh:

```text
Cmd + Shift + R
```

## Important Design Decisions

- The web app runs directly on the M5Stick. There is no external server.
- No school Wi-Fi credentials are needed.
- The dashboard polls `/data`, but records only new firmware samples using `sampleId`.
- Time values come from the M5Stick (`sampleMs`), not from browser polling time.
- Analysis examples are intentionally short and classroom-oriented.
- Ultrasonic mode shows one primary selected quantity at a time because distance, velocity, and acceleration have different units.

## Next Recommended Work

When hardware testing is available:

1. Test accelerometer zeroing on a stable table.
2. Test gyroscope zeroing while stationary.
3. Test load-cell tare with known masses and adjust calibration factor if needed.
4. Test ultrasonic distance at known distances.
5. Compare 5/10/20 Hz sampling for noise and responsiveness.

Good next software features:

- Demo mode with generated data
- CSV import for offline analysis
- Automatic printable/PDF lab report
- Experiment templates:
  - Hooke's law
  - impulse and collision
  - harmonic oscillation
  - motion with ultrasonic sensor

## Notes

- The chat history itself may not be available in every Codex app/session.
- This file is the permanent project-local history and should be updated after major changes.
