#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "sima_ftadi";  // Replace with your Wi-Fi SSID
const char* password = "XXX";  // 3X haahahaha

WebServer server(80);

const int gasSensorPin = 34;  // Analog pin for MQ-5 sensor

void setup() {
  Serial.begin(115200);
  pinMode(gasSensorPin, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Define the root URL and attach handler
  server.on("/", handleRoot);
  
  // Start the server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Handle client requests
  server.handleClient();
}

void handleRoot() {
  // Read gas sensor value
  int gasValue = analogRead(gasSensorPin);
  
  // Convert analog value to a range (0-1023)
  float gasConcentration = map(gasValue, 0, 4095, 0, 1023);

  // Print the gas concentration to the Serial Monitor
  Serial.print("Gas Value: ");
  Serial.println(gasConcentration);

  // Determine alert level and provide detailed information
  String alertLevel, alertInfo, safetyTips;
  String color;
  
  if (gasConcentration < 300) {
    alertLevel = "Normal";
    color = "green";
    alertInfo = "The gas concentration is within safe limits.";
    safetyTips = "No immediate actions required. Ensure good ventilation and continue monitoring.";
  } else if (gasConcentration < 600) {
    alertLevel = "Warning";
    color = "orange";
    alertInfo = "Gas levels are elevated. This could pose a risk if levels continue to rise.";
    safetyTips = "Increase ventilation immediately. Check for possible gas leaks or sources.";
  } else {
    alertLevel = "Danger";
    color = "red";
    alertInfo = "High gas concentration detected! Immediate action is required.";
    safetyTips = "Evacuate the area immediately. Shut off gas sources and contact emergency services.";
  }

  // Get Wi-Fi signal strength
  int rssi = WiFi.RSSI();
  String signalStrength = String(rssi) + " dBm";

  // Create an enhanced HTML page to display the gas value, alert level, detailed information, and safety tips
  String htmlPage = "<!DOCTYPE html><html><head>";
  htmlPage += "<meta http-equiv='refresh' content='5'/>";
  htmlPage += "<title>Gas Sensor Data</title>";
  htmlPage += "<style>";
  htmlPage += "body { font-family: Arial, sans-serif; background-color: #f4f4f9; color: #333; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }";
  htmlPage += ".container { text-align: center; border: 2px solid #ddd; padding: 20px; border-radius: 10px; box-shadow: 0 0 20px rgba(0, 0, 0, 0.1); background: white; max-width: 500px; }";
  htmlPage += ".alert { padding: 10px; border-radius: 5px; color: white; margin-top: 20px; font-size: 20px; transition: background-color 0.5s ease; }";
  htmlPage += ".info { text-align: left; margin-top: 20px; font-size: 14px; }";
  htmlPage += ".safety { background-color: #ffefc4; padding: 10px; border-radius: 5px; margin-top: 10px; }";
  htmlPage += ".signal { margin-top: 10px; font-size: 14px; color: #777; }";
  htmlPage += "</style></head><body>";
  htmlPage += "<div class='container'>";
  htmlPage += "<h1>Gas Concentration Monitor</h1>";
  htmlPage += "<p>Current Gas Value: <strong>" + String(gasConcentration) + "</strong></p>";
  htmlPage += "<div class='alert' style='background-color:" + color + ";'>Alert Level: <strong>" + alertLevel + "</strong></div>";
  htmlPage += "<div class='info'><strong>Information:</strong> " + alertInfo + "</div>";
  htmlPage += "<div class='safety'><strong>Safety Tips:</strong> " + safetyTips + "</div>";
  htmlPage += "<p class='signal'>Wi-Fi Signal Strength: " + signalStrength + "</p>";
  htmlPage += "</div>";
  htmlPage += "</body></html>";

  // Send the HTML page
  server.send(200, "text/html", htmlPage);
}
