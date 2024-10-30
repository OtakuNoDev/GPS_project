#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>

#define RX_PIN 4  // GPS TX to ESP8266 RX (GPIO 4)
#define TX_PIN 3  // ESP8266 TX to GPS RX (GPIO 3, usually not used)

// Wi-Fi credentials
const char* ssid = "GoJo";        // Replace with your Wi-Fi SSID
const char* password = "gojo@123"; // Replace with your Wi-Fi password

// Create a SoftwareSerial object
SoftwareSerial GPS_SoftSerial(RX_PIN, TX_PIN);
TinyGPSPlus gps;
WiFiServer server(80); // Create a web server on port 80

void setup() {
    Serial.begin(115200);           // Start Serial Monitor
    GPS_SoftSerial.begin(9600);    // Start SoftwareSerial for GPS
    WiFi.begin(ssid, password);     // Connect to Wi-Fi

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    server.begin();                 // Start the server
    Serial.println("Server started");
}

void loop() {
    while (GPS_SoftSerial.available()) {
        gps.encode(GPS_SoftSerial.read());
        if (gps.location.isUpdated()) {
            double latitude = gps.location.lat();
            double longitude = gps.location.lng();
            String url = "http://maps.google.com/?q=" + String(latitude,6) + "," + String(longitude,6);
            Serial.println(url); // Print URL to Serial Monitor

            // Handle client connections
            WiFiClient client = server.available();
            if (client) {
                Serial.println("New client connected");
                String response = "HTTP/1.1 302 Found\r\n";
                response += "Location: " + url + "\r\n";
                response += "Connection: close\r\n";
                response += "\r\n";
                client.print(response);
                delay(100); // Give the client time to receive the data
                client.stop(); // Close the connection
            }
        }
    }
}
