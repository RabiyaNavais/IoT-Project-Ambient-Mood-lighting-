#include <WiFi.h> // Library for WiFi functionality
#include <HTTPClient.h>  // Library for HTTP communication
#include "DHT.h" // Library for DHT sensor

// WiFi credentials
const char* ssid = "New_Virus_Found_UAE_2.4GEXT";   // WiFi network name
const char* password = "wajeedm123$$";             // WiFi password

// ThingSpeak API details
const char* server = "http://api.thingspeak.com/update"; // ThingSpeak API endpoint
String apiKey = "PRNTW7PN8SURK3TL";                     //ThingSpeak Write API Key

// DHT11 setup
#define DHTPIN 21       // Pin where the DHT11 DATA pin is connected
#define DHTTYPE DHT11   // Specifying DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE); // Create a DHT object with the specified pin and type

void setup() {
    Serial.begin(9600); // Starting Serial communication for debugging
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password); // Starting WiFi connection

    // Waiting for WiFi to connect
    while (WiFi.status() != WL_CONNECTED) { // Continue looping until connected
        delay(500); // Wait 500ms between retries
        Serial.print("."); // Keeps printing . until connected
    }

    // WiFi connected
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // Print the assigned IP address

    dht.begin(); // Initializing the DHT sensor
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) { // Checking if WiFi is connected
        // Reading temperature from DHT11
        float temperature = dht.readTemperature(); // Celsius by default

        if (isnan(temperature)) { // Checking if the reading is valid
            Serial.println("Failed to read from DHT sensor!");
            delay(10000); // Retry after 10 seconds
            return;
        }

        // Print temperature to Serial Monitor
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");

        // Send temperature to ThingSpeak
        HTTPClient http;
        String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(temperature);
        http.begin(url); // Initialize HTTP request

        int httpResponseCode = http.GET(); // Send HTTP GET request
        if (httpResponseCode > 0) {
            Serial.print("HTTP Response Code: ");
            Serial.println(httpResponseCode); // Should return 200 if successful
        } else {
            Serial.print("Error Code: ");
            Serial.println(httpResponseCode);
        }

        http.end(); // Close the HTTP connection
    } else {
        Serial.println("WiFi Disconnected!");
    }

    delay(15000); // Wait 15 seconds to comply with ThingSpeak's rate limit
}
