# Physics Lab 🧪📊

**Palladio Physics Lab** is a cross-platform, open-source educational toolkit designed to bring low-cost, real-time physics experimentation into the classroom or home lab. Built using Python and CustomTkinter, and powered by the M5StickC Plus2 microcontroller, this project helps students collect, visualize, and analyze motion, force, and other physics data live through easy-to-use interfaces.

---

## 🚀 Features

- 📡 **Real-Time Data Streaming** via UDP from M5StickC Plus2 to your desktop.
- 📈 **Live Graphs** of Acceleration, Velocity, Force, Angular Velocity, and Ultrasonic Distance.
- ⚙️ **Modular Design** — Add new sensor pages with ease.
- 📱 **Custom GUI** with Python (CustomTkinter + Matplotlib).
- ⚖️ **Force Sensor Integration** using HX711 + Load Cell.
- 🌀 **Angular Velocity Page** using onboard gyroscope.
- 📏 **Ultrasonic Motion Page** to capture position, velocity, and acceleration data.
- 🔄 **Start/Pause Control** and Back Navigation on all pages.
- 🔋 **Battery Monitoring** and Display on M5 Device.
- 🧪 **Designed for Educational Use**: Easy enough for middle school physics labs.

---

## 🛠 Hardware Requirements

- ✅ M5StickC Plus2
- ✅ USB-C Cable
- ✅ HX711 Module + Load Cell (optional)
- ✅ HC-SR04 Ultrasonic Sensor (optional)
- ✅ Breadboard and Jumper Wires
- ✅ Wi-Fi Network (for UDP communication)

---

## Run the Desktop App

1. Install Python 3.
2. Create and activate a virtual environment:

```bash
python3 -m venv .venv
source .venv/bin/activate
```

3. Install the app dependencies:

```bash
pip install -r requirements.txt
```

4. Start the desktop app:

```bash
python physics_lab_with_angular_velocity.py
```

## Current Connection Model

The M5StickC Plus2 sends UDP packets to the computer running the desktop app.
For now, Wi-Fi credentials and the computer IP address are configured inside
`Physics_Lab.ino`.

Before uploading the Arduino sketch, update:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* udpAddress = "YOUR_COMPUTER_IP";
```

The Python app listens on these UDP ports:

- `4210` acceleration / force from accelerometer
- `4211` angular velocity
- `4213` load cell force
- `4215` ultrasonic motion

