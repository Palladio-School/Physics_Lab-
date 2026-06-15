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
- ✅ Laptop with Wi-Fi (connects directly to the M5StickC Plus2 hotspot)

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

## Wireless Classroom Mode

The M5StickC Plus2 creates its own Wi-Fi network, so no school Wi-Fi
credentials or fixed computer IP address are needed.

1. Upload `Physics_Lab/Physics_Lab.ino` to the M5StickC Plus2.
2. Power on the M5StickC Plus2.
3. Connect the laptop to this Wi-Fi network:

```text
PhysicsLab-M5
```

4. Start the desktop app.
5. Select the matching experiment page and press `Start`.

The M5StickC Plus2 sends UDP broadcast packets to `192.168.4.255`.
The Python app listens on these UDP ports:

- `4210` acceleration / force from accelerometer
- `4211` angular velocity
- `4213` load cell force
- `4215` ultrasonic motion

Use the `WiFi Info` mode on the M5StickC Plus2 to confirm the hotspot name and
device IP address.
