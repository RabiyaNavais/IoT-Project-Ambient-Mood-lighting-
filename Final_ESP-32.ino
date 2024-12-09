#include <Wire.h>                // I2C communication library
#include <LiquidCrystal_I2C.h>   // Library for I2C LCD
#include <WiFi.h>                // Library for WiFi functionality
#include <HTTPClient.h>          // Library for HTTP communication
#include "DHT.h"                 // Library for DHT sensor

// WiFi credentials
const char* ssid = "MDX welcomes you"; // WiFi name
const char* password = "MdxL0vesyou";  // WiFi password

// ThingSpeak API details
const char* server = "http://api.thingspeak.com/update"; // ThingSpeak API endpoint
String apiKey = "PRNTW7PN8SURK3TL";                     // ThingSpeak Write API Key

// DHT11 setup
#define DHTPIN 13       // Pin where the DHT11 DATA pin is connected
#define DHTTYPE DHT11   // Specifying DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE); // Create a DHT object with the specified pin and type

// I2C LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27, 16 columns, 2 rows

// Custom I2C pins
#define SDA_PIN 21 // Use GPIO2 for SDA
#define SCL_PIN 22 // Use GPIO4 for SCL

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
    
    // Initialize the LCD with new I2C pins
    Wire.begin(SDA_PIN, SCL_PIN);  // Specify new SDA and SCL pins
    lcd.init();                    // Initialize the LCD
    lcd.backlight();               // Turn on the LCD backlight
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    delay(2000);
    lcd.clear();
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) { // Checking if WiFi is connected
        // Reading temperature from DHT11
        float temperature = dht.readTemperature(); // Celsius by default

        if (isnan(temperature)) { // Checking if the reading is valid
            Serial.println("Failed to read from DHT sensor!");
            lcd.setCursor(0, 0);
            lcd.print("Sensor Error!");
            delay(10000); // Retry after 10 seconds
            return;
        }

        // Print temperature to Serial Monitor
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");

        // Display temperature on the LCD
        lcd.clear();
        lcd.setCursor(0, 0); // Set the cursor to the first row, first column
        lcd.print("Temp: ");
        lcd.print(temperature);
        lcd.print(" C");

        // Send temperature to ThingSpeak
        HTTPClient http;
        String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(temperature);
        http.begin(url); // Initialize HTTP request

        int httpResponseCode = http.GET(); // Send HTTP GET request
        if (httpResponseCode > 0) {
            Serial.print("HTTP Response Code: ");
            Serial.println(httpResponseCode); // Should return 200 if successful
            lcd.setCursor(0, 1); // Set cursor to the second row
            lcd.print("Uploaded!");
        } else {
            Serial.print("Error Code: ");
            Serial.println(httpResponseCode);
            lcd.setCursor(0, 1); // Set cursor to the second row
            lcd.print("Upload Error");
        }

        http.end(); // Close the HTTP connection
    } else {
        Serial.println("WiFi Disconnected!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Disconnected");
    }

    delay(15000); // Wait 15 seconds to comply with ThingSpeak's rate limit
}
