#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi settings
const char* ssid = "Palladio_Teachers";
const char* password = "P@11@d10";

// UDP settings
const char* udpAddress = "10.2.0.44";  // Replace with your PC/server IP
const int accelPort = 4210;
const int gyroPort  = 4211;
const int centripetalPort = 4212;

WiFiUDP udp;

void connectToWiFi() {
  WiFi.begin(ssid, password);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.fillRect(0, 20, 160, 30, BLACK);
  M5.Lcd.println("\nWiFi connected!");
  M5.Lcd.println(WiFi.localIP());
}

void drawBattery(float voltage) {
  int width = 30, height = 12;
  int x = M5.Lcd.width() - width - 4;
  int y = 2;
  int gap = 5;
  int voltageTextWidth = 46;
  int textX = x - voltageTextWidth - gap;

  float percent = (voltage - 3.2) / (4.2 - 3.2);
  percent = constrain(percent, 0.0, 1.0);
  int fill = percent * (width - 4);

  M5.Lcd.fillRect(textX, y, width + voltageTextWidth + gap, height, BLACK);
  M5.Lcd.drawRect(x, y, width, height, WHITE);
  M5.Lcd.fillRect(x + width, y + 3, 2, 6, WHITE);
  M5.Lcd.fillRect(x + 2, y + 2, fill, height - 4, percent < 0.2 ? RED : GREEN);
  M5.Lcd.setCursor(textX, y);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.printf("%.2fV", voltage);
}

void setup() {
  Serial.begin(115200);
  M5.begin();
  M5.Imu.begin();
  M5.Power.begin();
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE);
  connectToWiFi();
  udp.begin(accelPort); // Just to initialize, not strictly required on sender
}

void loop() {
  float ax, ay, az;
  float gx, gy, gz;
  M5.Imu.getAccel(&ax, &ay, &az);
  M5.Imu.getGyro(&gx, &gy, &gz);

  float battVoltage = M5.Power.getBatteryVoltage() / 1000.0;

  // Battery indicator
  M5.Lcd.setTextSize(1);
  drawBattery(battVoltage);

  // Acceleration Display
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(CYAN, BLACK);
  M5.Lcd.setCursor(0, 60);
  M5.Lcd.fillRect(0, 60, 160, 40, BLACK);
  M5.Lcd.printf("X: %.2f g\nY: %.2f g\nZ: %.2f g", ax, ay, az);

  // --- Send acceleration data ---
  char accelBuffer[64];
  snprintf(accelBuffer, sizeof(accelBuffer), "%.2f,%.2f,%.2f,%.2f", ax, ay, az, battVoltage);
  udp.beginPacket(udpAddress, accelPort);
  udp.write((const uint8_t*)accelBuffer, strlen(accelBuffer));
  udp.endPacket();

  // --- Send angular velocity data ---
  char gyroBuffer[64];
  snprintf(gyroBuffer, sizeof(gyroBuffer), "%.2f,%.2f,%.2f,%.2f", gx, gy, gz, battVoltage);
  udp.beginPacket(udpAddress, gyroPort);
  udp.write((const uint8_t*)gyroBuffer, strlen(gyroBuffer));
  udp.endPacket();

  // --- Send gyro data to centripetal force page (new port) ---
  const int centripetalPort = 4212;
  udp.beginPacket(udpAddress, centripetalPort);
  udp.write((const uint8_t*)gyroBuffer, strlen(gyroBuffer));
  udp.endPacket();
  
  Serial.printf("Accel: %.2f %.2f %.2f | Gyro: %.2f %.2f %.2f | V: %.2fV\n",
                ax, ay, az, gx, gy, gz, battVoltage);

  delay(200);
}
