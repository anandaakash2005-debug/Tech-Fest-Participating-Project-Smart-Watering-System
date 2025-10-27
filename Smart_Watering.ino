#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ================= WiFi Credentials =================
const char* ssid = "SB WORLD // realme";      // Your WiFi SSID
const char* password ="h77sctgk";      // Your WiFi Password

// ================= Pin Setup =================
const int soilPin = A0;    // Soil moisture sensor (Analog pin A0)
const int relayPin = 5;    // GPIO5 = D1 (Relay control pin)

// ================= Web Server =================
ESP8266WebServer server(80);

int soilValue = 0;
bool pumpManual = false;   // false = auto mode, true = manual override
String pumpStatus = "OFF";

// ================= Read Soil Function =================
int readSoil() {
  soilValue = analogRead(soilPin);   // Range: 0 - 1023
  return soilValue;
}

// ================= Control Pump =================
void controlPump() {
  if (pumpManual) {
    // Manual mode: status already set by user
    return;
  }

  int moisture = readSoil();
  if (moisture < 500) {   // Adjust threshold as per your soil sensor
    digitalWrite(relayPin, HIGH);   // Turn ON Pump
    pumpStatus = "OFF (Auto)";
  } else {
    digitalWrite(relayPin, LOW);  // Turn OFF Pump
    pumpStatus = "ON (Auto)";
  }
}

// ================= Web Page =================
String buildWebPage() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta charset='UTF-8'><meta http-equiv='refresh' content='5'>";
  html += "<title>Smart Plant Watering System</title></head>";
  html += "<body style='font-family:Arial; text-align:center;'>";
  html += "<h2>🌱 Smart Plant Watering System</h2>";
  html += "<p><b>Soil Moisture:</b> " + String(soilValue) + "</p>";
  html += "<p><b>Pump Status:</b> " + pumpStatus + "</p>";
  html += "<form action=\"/on\" method=\"GET\"><button style='padding:10px 20px;'>Turn Pump ON</button></form><br>";
  html += "<form action=\"/off\" method=\"GET\"><button style='padding:10px 20px;'>Turn Pump OFF</button></form><br>";
  html += "<form action=\"/auto\" method=\"GET\"><button style='padding:10px 20px;'>Set AUTO Mode</button></form><br>";
  html += "<p>🔄 Page refreshes every 5s for real-time update.</p>";
  html += "</body></html>";
  return html;
}

// ================= Web Handlers =================
void handleRoot() {
  controlPump();
  server.send(200, "text/html", buildWebPage());
}

void handlePumpOn() {
  pumpManual = true;
  digitalWrite(relayPin, LOW);
  pumpStatus = "ON (Manual)";
  handleRoot();
}

void handlePumpOff() {
  pumpManual = true;
  digitalWrite(relayPin, HIGH);
  pumpStatus = "OFF (Manual)";
  handleRoot();
}

void handlePumpAuto() {
  pumpManual = false;
  handleRoot();
}

// ================= Setup =================
void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);   // Pump OFF initially

  // WiFi Connection
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected!");
  Serial.print("📶 IP Address: ");
  Serial.println(WiFi.localIP());

  // Start Web Server
  server.on("/", handleRoot);
  server.on("/on", handlePumpOn);
  server.on("/off", handlePumpOff);
  server.on("/auto", handlePumpAuto);
  server.begin();
  Serial.println("🌍 Web Server started!");
}

// ================= Loop =================
void loop() {
  server.handleClient();
}
