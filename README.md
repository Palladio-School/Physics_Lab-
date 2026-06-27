# Palladio Physics Lab

Palladio Physics Lab is a classroom physics toolkit for live, low-cost experiments with an M5StickC Plus2 and common sensors. The current classroom path is browser-based: the M5Stick creates its own Wi-Fi access point, exposes a small HTTP API, and serves a dashboard that students or teachers open from a laptop, tablet, or phone.

The repository also keeps the earlier Python desktop application for reference and compatibility, but new classroom work should start from the M5 firmware and web dashboard.

## What It Supports

- Live acceleration and gyroscope measurements from the M5StickC Plus2.
- Load-cell force measurements through an HX711 module.
- Ultrasonic distance, velocity, and acceleration measurements through a DFRobot URM10 ultrasonic sensor.
- Temperature / thermal-equilibrium measurements through DS18B20 sensors.
- A browser dashboard with experiment selection, live graphs, CSV export, worksheet/report flows, calibration controls, and sampling-rate controls.
- A legacy Python desktop app that can still listen to UDP broadcasts from the M5.

## Repository Map

| Path | Purpose |
| --- | --- |
| `Physics_Lab/Physics_Lab.ino` | Main Arduino firmware for the M5StickC Plus2. It creates the Wi-Fi AP, reads sensors, serves API endpoints, and emits UDP packets for the legacy desktop app. |
| `Physics_Lab/web_dashboard.h` | Lightweight firmware-served API status page embedded into the sketch. |
| `Physics_Lab/logo_asset.h` | Generated C header containing the Palladio logo bytes for firmware use. |
| `Physics_Lab/assets/` | Firmware-side project assets, currently the Palladio logo. |
| `Physics_Lab/scripts/arduino.sh` | Helper for compiling, cleaning, checking Arduino CLI version, and uploading firmware. |
| `Physics_Lab/PROJECT_HISTORY.md` | Development notes and operational history for the current M5/web-dashboard direction. |
| `docs/index.html` | Full GitHub Pages / browser dashboard. This is the current large UI surface and should be changed carefully. |
| `docs/assets/` | Dashboard images and experiment illustrations. |
| `physics_lab_with_angular_velocity.py` | Legacy CustomTkinter desktop app. |
| `requirements.txt` | Python dependencies for the legacy desktop app. |
| `case2.stl`, `cover.stl` | 3D-printable case/cover assets. |
| `PROJECT_STRUCTURE.md` | More detailed technical structure and known hot spots. |
| `HARDWARE_API.md` | Hardware wiring notes, firmware API reference, UDP compatibility, and verification checklist. |
| `UI_FLOW.md` | Student and teacher dashboard flow, classroom checklist, and current UI state model. |

## Run The Web Dashboard

The full dashboard lives in `docs/index.html`. To inspect it locally:

```bash
python3 -m http.server 8000 -d docs
```

Then open:

```text
http://localhost:8000
```

If GitHub Pages is enabled for the repository, it should serve the same `docs/` dashboard.

## Dashboard Safety Checks

The dashboard has lightweight regression checks for extracted helper files and the browser-only GitHub Pages surface:

```bash
npm install
npm test
```

If Playwright has not downloaded Chromium on the machine yet:

```bash
npx playwright install chromium
```

The browser smoke test serves `docs/` locally with mocked M5-shaped API responses, verifies the `docs/js/*.js` load order, checks for console/page errors, and switches through Hooke, Sonar, Pendulum, and Collision views.

For hardware-backed measurements, upload the firmware to the M5StickC Plus2 and use the device access point:

1. Upload `Physics_Lab/Physics_Lab.ino` to the M5StickC Plus2.
2. Power on or reset the M5StickC Plus2.
3. Connect the laptop, tablet, or phone to this Wi-Fi network:

```text
PhysicsLab-M5
```

4. Open this address in a browser:

```text
http://192.168.4.1
```

The M5 serves the live API from `192.168.4.1`. The full dashboard can use that API when the browser is connected to the M5 network.

## Upload Firmware

The helper script expects Arduino CLI either in `PATH`, in the standard macOS Arduino IDE bundle location, or through `ARDUINO_CLI`.

```bash
cd Physics_Lab
./scripts/arduino.sh compile
./scripts/arduino.sh upload /dev/cu.usbserial-0001
```

If the upload port changes, list boards with:

```bash
'/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli' board list
```

The default board identifier is:

```text
m5stack:esp32:m5stack_stickc_plus2
```

Override it only when testing a different compatible board:

```bash
ARDUINO_FQBN=<fqbn> ./scripts/arduino.sh compile
```

## API Surface

The firmware exposes these browser-facing endpoints:

- `/` - lightweight device/API status page from the firmware.
- `/logo.png` - embedded Palladio logo.
- `/data` - latest device state and current readings.
- `/samples?since=<sampleId>` - batched sample retrieval without duplicating already-recorded samples.
- `/set-mode?mode=<n>` - switch active sensor/experiment mode.
- `/settings?rate=<hz>` - set supported sampling rate.
- `/calibrate?target=<name>` - run sensor zero/tare calibration for the active target.

The firmware also keeps UDP output for the legacy desktop app:

- `4210` acceleration / force from accelerometer
- `4211` angular velocity
- `4213` load-cell force
- `4215` ultrasonic motion
- `4216` temperature

## Hardware

Core hardware:

- M5StickC Plus2
- USB-C cable
- Laptop, tablet, or phone with Wi-Fi

Optional experiment hardware:

- HX711 module and load cell for force / Hooke-law style experiments
- DFRobot URM10 ultrasonic sensor for distance and motion
- DS18B20 temperature sensors for thermal-equilibrium experiments
- Breadboard and jumper wires

Current firmware pin notes:

- DFRobot URM10: `TRIG_PIN = 26`, `ECHO_PIN = 25`
- HX711: `DT = 36`, `DT_ALT = 25`, `SCK = 26`
- DS18B20: `TEMP1_PIN = 25`, `TEMP2_PIN = 26`

On startup the firmware attempts to detect the connected external sensor family and selects the matching mode automatically: DS18B20 heat first, then URM10 ultrasonic, then HX711 force. Manual mode selection from the dashboard or M5 buttons remains available.

Some pins are shared across experiment modes. Treat this as a documented hardware constraint until the wiring plan is finalized per lab setup.
With the current wiring, the DFRobot URM10 and the two DS18B20 sensors both use `G25/G26`, so do not keep them connected at the same time.

## Run The Legacy Desktop App

The Python app is still useful for reference, older demos, and UDP compatibility checks.

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python physics_lab_with_angular_velocity.py
```

## Known Hot Spots

- `docs/index.html` is currently a large single-file dashboard. Prefer documentation and small isolated changes before deep UI refactoring.
- The dashboard polls the M5 API frequently. Firmware sample IDs reduce duplicate samples, but performance should still be watched on lower-power devices.
- Sensor wiring and shared pins need a stable classroom-facing hardware guide before broad use.
- Arduino compile/upload validation depends on a local Arduino CLI + M5Stack board setup.
- The legacy Python app and current browser dashboard now describe different generations of the project; keep future changes explicit about which path they affect.

See `PROJECT_STRUCTURE.md` for the fuller technical map.
See `HARDWARE_API.md` for the hardware and API reference.
See `UI_FLOW.md` for the student and teacher classroom flow.
