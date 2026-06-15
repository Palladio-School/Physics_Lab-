#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "HX711.h"

// Classroom wireless mode: the M5StickC Plus2 creates its own Wi-Fi network.
const char* apSsid = "PhysicsLab-M5";
IPAddress broadcastAddress(192, 168, 4, 255);

// UDP Ports
const int accelPort = 4210;
const int gyroPort  = 4211;
const int forcePort = 4213;
const int ultraPort = 4215;

WiFiUDP udp;

// Pins
#define TRIG_PIN 26  // G26
#define ECHO_PIN 0   // G0
#define DT 36        // HX711 DOUT (virtual, reused)
#define SCK 26       // HX711 SCK (shared with TRIG)

HX711 scale;
const float SCALE_FACTOR = 48163.7;

int mode = 0;
const int maxMode = 4;

unsigned long lastBatteryUpdate = 0;
float lastBatteryVoltage = 0;

float prev_distance = -1, prev_velocity = 0;
unsigned long prev_time = 0;
float smoothed_distance = -1;

// Display helpers
void drawFooter() {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(0, M5.Lcd.height() - 20);
  M5.Lcd.print("@Sotiris Kaproulias");
}

void drawBattery(float voltage) {
  int width = 30, height = 12;
  int x = M5.Lcd.width() - width - 4, y = 2;
  float percent = constrain((voltage - 3.2) / (4.2 - 3.2), 0.0, 1.0);
  int fill = percent * (width - 4);

  M5.Lcd.fillRect(x - 65, y - 2, 100, height + 4, BLACK);
  M5.Lcd.drawRect(x, y, width, height, WHITE);
  M5.Lcd.fillRect(x + width, y + 3, 2, 6, WHITE);
  M5.Lcd.fillRect(x + 2, y + 2, fill, height - 4, percent < 0.2 ? RED : GREEN);
  M5.Lcd.setTextSize(1.5);
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setCursor(x - 50, y);
  M5.Lcd.printf("%.2fV", voltage);
}

void displayMode() {
  const char* modes[] = {"Accel", "Gyro", "Force", "WiFi Info", "Ultrasonic"};
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(CYAN, BLACK);
  M5.Lcd.fillRect(0, 0, 160, 20, BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("%s", modes[mode]);
  drawFooter();
}

void showSplashScreen() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(20, 30);
  M5.Lcd.println("PALLADIO LAB");
  M5.Lcd.setCursor(20, 60);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.println("Physics Tools");
  drawFooter();
  delay(3000);
}

void startAccessPoint() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid);
}

void sendUdpPacket(const char* payload, int port) {
  udp.beginPacket(broadcastAddress, port);
  udp.write((uint8_t*)payload, strlen(payload));
  udp.endPacket();
}

// ULTRASONIC FILTERED READING
float getFilteredDistanceCM() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;

  float d = duration * 0.0343 / 2.0;
  if (d < 2 || d > 200) return -1;

  if (smoothed_distance < 0) smoothed_distance = d;
  else smoothed_distance = 0.6 * smoothed_distance + 0.4 * d;

  return smoothed_distance;
}

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Imu.begin();
  M5.Power.begin();
  M5.Lcd.setRotation(1);
  showSplashScreen();
  M5.Lcd.fillScreen(BLACK);

  startAccessPoint();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  scale.begin(DT, SCK);
  scale.set_scale(SCALE_FACTOR);
  scale.tare();

  udp.begin(accelPort);
  displayMode();
  prev_time = millis();
}

void loop() {
  M5.update();
  unsigned long now = millis();

  if (now - lastBatteryUpdate >= 1000 || lastBatteryVoltage == 0) {
    lastBatteryVoltage = M5.Power.getBatteryVoltage() / 1000.0;
    drawBattery(lastBatteryVoltage);
    lastBatteryUpdate = now;
  }

  if (M5.BtnA.wasPressed()) {
    mode = (mode + 1) % (maxMode + 1);
    M5.Lcd.fillScreen(BLACK);
    displayMode();
  }

  if (M5.BtnB.wasPressed()) {
    mode--;
    if (mode < 0) mode = maxMode;
    M5.Lcd.fillScreen(BLACK);
    displayMode();
  }

  if (mode == 0) {
    float ax, ay, az;
    M5.Imu.getAccel(&ax, &ay, &az);
    M5.Lcd.setCursor(0, 40); M5.Lcd.setTextColor(YELLOW, BLACK); M5.Lcd.setTextSize(2);
    M5.Lcd.printf("X: %.2f\nY: %.2f\nZ: %.2f", ax, ay, az);
    drawFooter();

    char buf[64];
    snprintf(buf, sizeof(buf), "%.2f,%.2f,%.2f,%.2f", ax, ay, az, lastBatteryVoltage);
    sendUdpPacket(buf, accelPort);
  }

  else if (mode == 1) {
    float gx, gy, gz;
    M5.Imu.getGyro(&gx, &gy, &gz);
    M5.Lcd.setCursor(0, 40); M5.Lcd.setTextColor(YELLOW, BLACK); M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Gx: %.2f\nGy: %.2f\nGz: %.2f", gx, gy, gz);
    drawFooter();

    char buf[64];
    snprintf(buf, sizeof(buf), "%.2f,%.2f,%.2f,%.2f", gx, gy, gz, lastBatteryVoltage);
    sendUdpPacket(buf, gyroPort);
  }

  else if (mode == 2) {
    float force = scale.get_units(5);
    M5.Lcd.setCursor(0, 40); M5.Lcd.setTextColor(YELLOW, BLACK); M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Force: %.2f N", force);
    drawFooter();

    char buf[32];
    snprintf(buf, sizeof(buf), "%.3f,%.2f", force, lastBatteryVoltage);
    sendUdpPacket(buf, forcePort);
  }

  else if (mode == 3) {
    M5.Lcd.setTextSize(2); M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.printf("AP:\n%s\nIP:\n%s", apSsid, WiFi.softAPIP().toString().c_str());
    drawFooter();
  }

  else if (mode == 4) {
    float d_cm = getFilteredDistanceCM();
    if (d_cm < 0) return;

    float dt = (now - prev_time) / 1000.0;
    prev_time = now;

    float d_m = d_cm / 100.0;
    float v = (prev_distance >= 0) ? (d_m - prev_distance) / dt : 0;
    float a = (prev_distance >= 0) ? (v - prev_velocity) / dt : 0;

    prev_velocity = v;
    prev_distance = d_m;

    M5.Lcd.fillRect(0, 20, 160, 100, BLACK);
    M5.Lcd.setTextColor(YELLOW, BLACK); M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.printf("d: %.2fm\nv: %.2fm/s\na: %.2fm/s^2", d_m, v, a);
    drawFooter();

    char buf[64];
    snprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", d_m, v, a);
    sendUdpPacket(buf, ultraPort);
  }

  delay(50);
}
