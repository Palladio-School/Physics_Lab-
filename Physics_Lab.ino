#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "HX711.h"

// WiFi settings
const char* ssid = "Palladio_Teachers";
const char* password = "P@11@d10";

// UDP settings
const char* udpAddress = "10.2.0.91";
const int accelPort = 4210;
const int gyroPort  = 4211;
const int forcePort = 4213;

WiFiUDP udp;

// HX711 setup
#define DT 36   // M5StickC Plus2 top GPIO
#define SCK 26
HX711 scale;
const float SCALE_FACTOR = 42235.8984;

// Mode control
int mode = 0; // 0 = accel, 1 = gyro, 2 = force, 3 = WiFi info
const int maxMode = 3;

// Battery update timing
unsigned long lastBatteryUpdate = 0;
float lastBatteryVoltage = 0;

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void drawBattery(float voltage) {
  int width = 30, height = 12;
  int x = M5.Lcd.width() - width - 4;
  int y = 2;

  float percent = (voltage - 3.2) / (4.2 - 3.2);
  percent = constrain(percent, 0.0, 1.0);
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

void drawFooter() {
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(0, M5.Lcd.height() - 20);
  M5.Lcd.print("@Sotiris Kaproulias");
}

void displayMode() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(CYAN, BLACK);
  M5.Lcd.fillRect(0, 0, 160, 20, BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("%s", mode == 0 ? "Accel" : (mode == 1 ? "Gyro" : (mode == 2 ? "Force" : "WiFi Info")));
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

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Imu.begin();
  M5.Power.begin();
  M5.Lcd.setRotation(1);
  showSplashScreen();
  M5.Lcd.fillScreen(BLACK);
  connectToWiFi();

  scale.begin(DT, SCK);
  scale.set_scale(SCALE_FACTOR);
  scale.tare();

  udp.begin(accelPort);
  displayMode();
}

void loop() {
  M5.update();

  unsigned long currentMillis = millis();
  if (currentMillis - lastBatteryUpdate >= 1000 || lastBatteryVoltage == 0) {
    lastBatteryVoltage = M5.Power.getBatteryVoltage() / 1000.0;
    drawBattery(lastBatteryVoltage);
    lastBatteryUpdate = currentMillis;
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

  if (mode == 0) { // Acceleration
    float ax, ay, az;
    M5.Imu.getAccel(&ax, &ay, &az);

    M5.Lcd.setCursor(0, 40);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("X: %.2f\nY: %.2f\nZ: %.2f", ax, ay, az);
    drawFooter();

    char accelBuffer[64];
    snprintf(accelBuffer, sizeof(accelBuffer), "%.2f,%.2f,%.2f,%.2f", ax, ay, az, lastBatteryVoltage);
    udp.beginPacket(udpAddress, accelPort);
    udp.write((const uint8_t*)accelBuffer, strlen(accelBuffer));
    udp.endPacket();
  }

  else if (mode == 1) { // Gyroscope
    float gx, gy, gz;
    M5.Imu.getGyro(&gx, &gy, &gz);

    M5.Lcd.setCursor(0, 40);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Gx: %.2f\nGy: %.2f\nGz: %.2f", gx, gy, gz);
    drawFooter();

    char gyroBuffer[64];
    snprintf(gyroBuffer, sizeof(gyroBuffer), "%.2f,%.2f,%.2f,%.2f", gx, gy, gz, lastBatteryVoltage);
    udp.beginPacket(udpAddress, gyroPort);
    udp.write((const uint8_t*)gyroBuffer, strlen(gyroBuffer));
    udp.endPacket();
  }

  else if (mode == 2) { // Force sensor
    float total = 0;
    for (int i = 0; i < 20; i++) {
      total += scale.get_units(1);
      delay(50);
    }
    float avg_force = abs(total / 20.0);

    M5.Lcd.setCursor(0, 40);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Force: %.2f N", avg_force);
    drawFooter();

    char forceBuffer[32];
    snprintf(forceBuffer, sizeof(forceBuffer), "%.3f,%.2f", avg_force, lastBatteryVoltage);
    udp.beginPacket(udpAddress, forcePort);
    udp.write((const uint8_t*)forceBuffer, strlen(forceBuffer));
    udp.endPacket();
  }

  else if (mode == 3) { // WiFi Info
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.printf("SSID:\n%s\nIP:\n%s", ssid, WiFi.localIP().toString().c_str());
    drawFooter();
  }
}
