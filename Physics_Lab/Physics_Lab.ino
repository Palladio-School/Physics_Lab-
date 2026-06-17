#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include "HX711.h"
#include "web_dashboard.h"
#include "logo_asset.h"

// Classroom wireless mode: the M5StickC Plus2 creates its own Wi-Fi network.
const char* apSsid = "PhysicsLab-M5";
IPAddress broadcastAddress(192, 168, 4, 255);

// UDP Ports
const int accelPort = 4210;
const int gyroPort  = 4211;
const int forcePort = 4213;
const int ultraPort = 4215;

WiFiUDP udp;
WebServer server(80);

// Pins
#define TRIG_PIN 26  // G26
#define ECHO_PIN 0   // G0
#define DT 36        // HX711 DOUT (virtual, reused)
#define SCK 26       // HX711 SCK (shared with TRIG)

HX711 scale;
const float SCALE_FACTOR = 48163.7;

int mode = 0;
const int maxMode = 4;
uint16_t sampleRateHz = 10;
unsigned long sampleIntervalMs = 100;
unsigned long lastSampleDue = 0;
unsigned long sampleTimestampMs = 0;
unsigned long sampleSequence = 0;

struct SampleRecord {
  unsigned long id;
  unsigned long timestampMs;
  uint8_t mode;
  float ax, ay, az;
  float gx, gy, gz;
  float force;
  float distance, velocity, acceleration;
  float battery;
};

const uint16_t sampleBufferSize = 512;
SampleRecord sampleBuffer[sampleBufferSize];
uint16_t sampleBufferHead = 0;
uint16_t sampleBufferCount = 0;

unsigned long lastBatteryUpdate = 0;
float lastBatteryVoltage = 0;

float prev_distance = -1, prev_velocity = 0;
unsigned long prev_time = 0;
float smoothed_distance = -1;
float distanceOffset = 0;

float latestAx = 0, latestAy = 0, latestAz = 0;
float latestGx = 0, latestGy = 0, latestGz = 0;
float latestForce = 0;
float latestDistance = 0, latestVelocity = 0, latestAcceleration = 0;
float rawAx = 0, rawAy = 0, rawAz = 0;
float rawGx = 0, rawGy = 0, rawGz = 0;
float accelOffsetX = 0, accelOffsetY = 0, accelOffsetZ = 0;
float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;

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

const char* getModeName() {
  const char* modes[] = {"Acceleration", "Gyroscope", "Load Cell", "WiFi Info", "Ultrasonic"};
  return modes[mode];
}

void clearSampleBuffer() {
  sampleBufferHead = 0;
  sampleBufferCount = 0;
}

void recordSample(uint8_t sampleMode, unsigned long timestampMs, unsigned long id) {
  SampleRecord& record = sampleBuffer[sampleBufferHead];
  record.id = id;
  record.timestampMs = timestampMs;
  record.mode = sampleMode;
  record.ax = latestAx;
  record.ay = latestAy;
  record.az = latestAz;
  record.gx = latestGx;
  record.gy = latestGy;
  record.gz = latestGz;
  record.force = latestForce;
  record.distance = latestDistance;
  record.velocity = latestVelocity;
  record.acceleration = latestAcceleration;
  record.battery = lastBatteryVoltage;

  sampleBufferHead = (sampleBufferHead + 1) % sampleBufferSize;
  if (sampleBufferCount < sampleBufferSize) sampleBufferCount++;
}

uint16_t sampleBufferIndex(uint16_t chronologicalIndex) {
  uint16_t start = (sampleBufferHead + sampleBufferSize - sampleBufferCount) % sampleBufferSize;
  return (start + chronologicalIndex) % sampleBufferSize;
}

void sendUdpPacket(const char* payload, int port) {
  udp.beginPacket(broadcastAddress, port);
  udp.write((uint8_t*)payload, strlen(payload));
  udp.endPacket();
}

void addCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.sendHeader("Access-Control-Allow-Private-Network", "true");
}

void handleOptions() {
  addCorsHeaders();
  server.send(204);
}

void handleRoot() {
  addCorsHeaders();
  server.send_P(200, "text/html", DASHBOARD_HTML);
}

void handleLogo() {
  addCorsHeaders();
  server.send_P(
    200,
    "image/png",
    reinterpret_cast<const char*>(assets_palladio_logo_png),
    assets_palladio_logo_png_len
  );
}

void handleData() {
  addCorsHeaders();
  char json[640];
  snprintf(
    json,
    sizeof(json),
    "{\"mode\":%d,\"modeName\":\"%s\",\"ap\":\"%s\",\"ip\":\"%s\",\"battery\":%.2f,"
    "\"sampleId\":%lu,\"sampleMs\":%lu,\"sampleRate\":%u,"
    "\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,"
    "\"gx\":%.3f,\"gy\":%.3f,\"gz\":%.3f,"
    "\"force\":%.3f,\"distance\":%.3f,\"velocity\":%.3f,\"acceleration\":%.3f}",
    mode,
    getModeName(),
    apSsid,
    WiFi.softAPIP().toString().c_str(),
    lastBatteryVoltage,
    sampleSequence,
    sampleTimestampMs,
    sampleRateHz,
    latestAx,
    latestAy,
    latestAz,
    latestGx,
    latestGy,
    latestGz,
    latestForce,
    latestDistance,
    latestVelocity,
    latestAcceleration
  );
  server.send(200, "application/json", json);
}

void sendSampleJson(const SampleRecord& record, bool comma) {
  char json[384];
  snprintf(
    json,
    sizeof(json),
    "%s{\"mode\":%u,\"sampleId\":%lu,\"sampleMs\":%lu,\"sampleRate\":%u,"
    "\"battery\":%.2f,\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,"
    "\"gx\":%.3f,\"gy\":%.3f,\"gz\":%.3f,"
    "\"force\":%.3f,\"distance\":%.3f,\"velocity\":%.3f,\"acceleration\":%.3f}",
    comma ? "," : "",
    record.mode,
    record.id,
    record.timestampMs,
    sampleRateHz,
    record.battery,
    record.ax,
    record.ay,
    record.az,
    record.gx,
    record.gy,
    record.gz,
    record.force,
    record.distance,
    record.velocity,
    record.acceleration
  );
  server.sendContent(json);
}

void handleSamples() {
  addCorsHeaders();
  unsigned long since = 0;
  bool includeAll = true;
  if (server.hasArg("since")) {
    String sinceArg = server.arg("since");
    long parsed = sinceArg.toInt();
    if (parsed >= 0) {
      since = (unsigned long)parsed;
      includeAll = false;
    }
  }

  char header[256];
  snprintf(
    header,
    sizeof(header),
    "{\"mode\":%d,\"modeName\":\"%s\",\"ap\":\"%s\",\"ip\":\"%s\",\"battery\":%.2f,"
    "\"sampleRate\":%u,\"latestId\":%lu,\"bufferCount\":%u,\"samples\":[",
    mode,
    getModeName(),
    apSsid,
    WiFi.softAPIP().toString().c_str(),
    lastBatteryVoltage,
    sampleRateHz,
    sampleSequence,
    sampleBufferCount
  );

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "application/json", "");
  server.sendContent(header);

  bool comma = false;
  for (uint16_t i = 0; i < sampleBufferCount; i++) {
    const SampleRecord& record = sampleBuffer[sampleBufferIndex(i)];
    if (!includeAll && record.id <= since) continue;
    sendSampleJson(record, comma);
    comma = true;
  }
  server.sendContent("]}");
  server.sendContent("");
}

void handleSetMode() {
  if (server.hasArg("mode")) {
    int requestedMode = server.arg("mode").toInt();
    if (requestedMode >= 0 && requestedMode <= maxMode) {
      mode = requestedMode;
      prev_distance = -1;
      prev_velocity = 0;
      smoothed_distance = -1;
      prev_time = millis();
      lastSampleDue = 0;
      clearSampleBuffer();
      M5.Lcd.fillScreen(BLACK);
      displayMode();
    }
  }
  handleData();
}

void handleSettings() {
  if (server.hasArg("rate")) {
    int requestedRate = server.arg("rate").toInt();
    if (requestedRate == 5 || requestedRate == 10 || requestedRate == 20 ||
        requestedRate == 50 || requestedRate == 100) {
      sampleRateHz = requestedRate;
      sampleIntervalMs = 1000UL / sampleRateHz;
      lastSampleDue = 0;
    }
  }
  handleData();
}

void handleCalibrate() {
  String target = server.arg("target");
  if (target == "accel") {
    accelOffsetX = rawAx;
    accelOffsetY = rawAy;
    accelOffsetZ = rawAz;
    latestAx = latestAy = latestAz = 0;
  } else if (target == "gyro") {
    gyroOffsetX = rawGx;
    gyroOffsetY = rawGy;
    gyroOffsetZ = rawGz;
    latestGx = latestGy = latestGz = 0;
  } else if (target == "force") {
    scale.tare(10);
    latestForce = 0;
  } else if (target == "distance") {
    distanceOffset += latestDistance;
    latestDistance = 0;
    latestVelocity = 0;
    latestAcceleration = 0;
    prev_distance = -1;
    prev_velocity = 0;
    prev_time = millis();
  }
  handleData();
}

void startWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/", HTTP_OPTIONS, handleOptions);
  server.on("/logo.png", HTTP_GET, handleLogo);
  server.on("/logo.png", HTTP_OPTIONS, handleOptions);
  server.on("/data", HTTP_GET, handleData);
  server.on("/data", HTTP_OPTIONS, handleOptions);
  server.on("/samples", HTTP_GET, handleSamples);
  server.on("/samples", HTTP_OPTIONS, handleOptions);
  server.on("/set-mode", HTTP_GET, handleSetMode);
  server.on("/set-mode", HTTP_OPTIONS, handleOptions);
  server.on("/settings", HTTP_GET, handleSettings);
  server.on("/settings", HTTP_OPTIONS, handleOptions);
  server.on("/calibrate", HTTP_GET, handleCalibrate);
  server.on("/calibrate", HTTP_OPTIONS, handleOptions);
  server.begin();
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

bool sampleCurrentMode(unsigned long now) {
  if (mode == 0) {
    float ax, ay, az;
    M5.Imu.getAccel(&ax, &ay, &az);
    rawAx = ax * 9.81;
    rawAy = ay * 9.81;
    rawAz = az * 9.81;
    latestAx = rawAx - accelOffsetX;
    latestAy = rawAy - accelOffsetY;
    latestAz = rawAz - accelOffsetZ;

    M5.Lcd.setCursor(0, 40);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("X: %.2f\nY: %.2f\nZ: %.2f", latestAx, latestAy, latestAz);
    drawFooter();

    char buf[64];
    snprintf(
      buf,
      sizeof(buf),
      "%.2f,%.2f,%.2f,%.2f",
      latestAx,
      latestAy,
      latestAz,
      lastBatteryVoltage
    );
    sendUdpPacket(buf, accelPort);
    return true;
  }

  if (mode == 1) {
    M5.Imu.getGyro(&rawGx, &rawGy, &rawGz);
    latestGx = rawGx - gyroOffsetX;
    latestGy = rawGy - gyroOffsetY;
    latestGz = rawGz - gyroOffsetZ;

    M5.Lcd.setCursor(0, 40);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Gx: %.2f\nGy: %.2f\nGz: %.2f", latestGx, latestGy, latestGz);
    drawFooter();

    char buf[64];
    snprintf(
      buf,
      sizeof(buf),
      "%.2f,%.2f,%.2f,%.2f",
      latestGx,
      latestGy,
      latestGz,
      lastBatteryVoltage
    );
    sendUdpPacket(buf, gyroPort);
    return true;
  }

  if (mode == 2) {
    if (!scale.is_ready()) return false;
    latestForce = scale.get_units(1);
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("Force: %.2f N", latestForce);
    drawFooter();

    char buf[32];
    snprintf(buf, sizeof(buf), "%.3f,%.2f", latestForce, lastBatteryVoltage);
    sendUdpPacket(buf, forcePort);
    return true;
  }

  if (mode == 3) {
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.printf("AP:\n%s\nIP:\n%s", apSsid, WiFi.softAPIP().toString().c_str());
    drawFooter();
    return true;
  }

  if (mode == 4) {
    float d_cm = getFilteredDistanceCM();
    if (d_cm < 0) return false;

    float dt = (now - prev_time) / 1000.0;
    prev_time = now;
    if (dt <= 0) dt = sampleIntervalMs / 1000.0;

    float d_m = d_cm / 100.0 - distanceOffset;
    float v = (prev_distance >= 0) ? (d_m - prev_distance) / dt : 0;
    float a = (prev_distance >= 0) ? (v - prev_velocity) / dt : 0;
    latestDistance = d_m;
    latestVelocity = v;
    latestAcceleration = a;

    prev_velocity = v;
    prev_distance = d_m;

    M5.Lcd.fillRect(0, 20, 160, 100, BLACK);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.printf("d: %.2fm\nv: %.2fm/s\na: %.2fm/s^2", d_m, v, a);
    drawFooter();

    char buf[64];
    snprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", d_m, v, a);
    sendUdpPacket(buf, ultraPort);
    return true;
  }

  return false;
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
  startWebServer();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  scale.begin(DT, SCK);
  scale.set_scale(SCALE_FACTOR);
  scale.tare();

  udp.begin(accelPort);
  displayMode();
  prev_time = millis();
  lastSampleDue = 0;
}

void loop() {
  M5.update();
  server.handleClient();
  unsigned long now = millis();

  if (now - lastBatteryUpdate >= 1000 || lastBatteryVoltage == 0) {
    lastBatteryVoltage = M5.Power.getBatteryVoltage() / 1000.0;
    drawBattery(lastBatteryVoltage);
    lastBatteryUpdate = now;
  }

  if (M5.BtnA.wasPressed()) {
    mode = (mode + 1) % (maxMode + 1);
    prev_distance = -1;
    prev_velocity = 0;
    smoothed_distance = -1;
    prev_time = now;
    lastSampleDue = 0;
    clearSampleBuffer();
    M5.Lcd.fillScreen(BLACK);
    displayMode();
  }

  if (M5.BtnB.wasPressed()) {
    mode--;
    if (mode < 0) mode = maxMode;
    prev_distance = -1;
    prev_velocity = 0;
    smoothed_distance = -1;
    prev_time = now;
    lastSampleDue = 0;
    clearSampleBuffer();
    M5.Lcd.fillScreen(BLACK);
    displayMode();
  }

  if (lastSampleDue == 0 || now - lastSampleDue >= sampleIntervalMs) {
    if (lastSampleDue == 0 || now - lastSampleDue > sampleIntervalMs * 4) {
      lastSampleDue = now;
    } else {
      lastSampleDue += sampleIntervalMs;
    }
    if (sampleCurrentMode(now)) {
      sampleTimestampMs = now;
      sampleSequence++;
      recordSample(mode, sampleTimestampMs, sampleSequence);
    }
  }

  delay(1);
}
