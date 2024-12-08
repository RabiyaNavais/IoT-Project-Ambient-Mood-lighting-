#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PulseSensorPlayground.h>

// IR Transmitter Config
IRsend irsend; // Creating an object for IR transmission

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address is 0x27 ,with 2rows, 16Columns 

// Pulse Sensor Configuration
const int PulseWire = A0;   // PulseSensor connected to A0
int Threshold = 550;       // Threshold for heartbeat detection
PulseSensorPlayground pulseSensor; // Object for Heartbeat Sensor 

// Variables for temperature and mood
float temperature = 0.0; // For Temp readings
String transmittedCode; // Stroes the transmitted hex IR code
String mood; // Stores the mood based of given conditions 

// Setup function; only runs at the start
void setup() {
    // Initializing Serial Monitor
    Serial.begin(9600);

    // Initializing LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Initializing..."); // Initializing message
    delay(2000);

    // Initializing Heartrate sensor
    pulseSensor.analogInput(PulseWire); // Setting the sensor input pin
    pulseSensor.setThreshold(Threshold); // Setting the heartbeat detection threshold

    // Starting the PulseSensor/HeartRate Sensor
    if (pulseSensor.begin()) {
        Serial.println("PulseSensor initialized!");
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready..."); // Just a ready message on the lcd
    delay(2000);
}

// Main Loop function; runs constantly 
void loop() {
    // Fetch temperature from ThingSpeak via Serial
    if (Serial.available()) { // Checking if there is data available on the Serial port
        String receivedData = Serial.readStringUntil('\n'); // Reading the data until a newline
        if (receivedData.length() > 0) {
            temperature = receivedData.toFloat(); // Converting received data to float
        }
    }

    // Get heart rate (BPM) from PulseSensor
    int myBPM = pulseSensor.getBeatsPerMinute();

    if (pulseSensor.sawStartOfBeat()) { // Check if heartbeat is detected
        Serial.println("Heartbeat detected!"); //Log Detetction 
        Serial.print("BPM: ");
        Serial.println(myBPM); // Log BMP

        // Determine mood and IR code to transmit based on temperature and BPM
        if (myBPM < 60 && temperature < 20) {
            mood = "Relaxing,Calm Setting(Blue)";
            transmittedCode = "0xF750AF"; // Blue
            irsend.sendNEC(0xF750AF, 32); //Transmitting code for Blue light 
        } else if (myBPM < 60 && temperature >= 20 && temperature <= 25) {
            mood = "Calm, Slightly Warm(White)";
            transmittedCode = "0xF7E01F"; // White
            irsend.sendNEC(0xF7E01F, 32);
        } else if (myBPM >= 80 && myBPM <= 110 && temperature < 20) {
            mood = "Neutral,Refreshing touch(Green)";
            transmittedCode = "0xF7A05F"; // Green
            irsend.sendNEC(0xF7A05F, 32);
        } else if (myBPM >= 80 && myBPM <= 110 && temperature >= 20 && temperature <= 25) {
            mood = "Comfortable,Balanced(Warm Wht)";
            transmittedCode = "0xF730CF"; // Warm White
            irsend.sendNEC(0xF730CF, 32);
        } else if (myBPM >= 80 && myBPM <= 110 && temperature > 25) {
            mood = "Slightly Energizing(Yellow)";
            transmittedCode = "0xF708F7"; // Yellow
            irsend.sendNEC(0xF708F7, 32);
        } else if (myBPM >= 110 && temperature < 20) {
            mood = "High Energy,Cool Balance(Purple)";
            transmittedCode = "0xF76897"; // Purple
            irsend.sendNEC(0xF76897, 32);
        } else if (myBPM >= 110 && temperature >= 20 && temperature <= 25) {
            mood = "Warm & Energizing(Orange)";
            transmittedCode = "0xF710EF"; // Orange
            irsend.sendNEC(0xF710EF, 32);
        } else if (myBPM >= 110 && temperature > 25) {
            mood = "Very Energetic(Re)";
            transmittedCode = "0xF720DF"; // Red
            irsend.sendNEC(0xF720DF, 32);
        } else {
            mood = "Not detected";
            transmittedCode = "0xF7E817"; // Smooth transition if no mood is detected/Sensor Faliure 
            irsend.sendNEC(0xF7E817, 32);
        }

        // Update LCD with mood
        lcd.clear();

        // Display the first part of the mood on the first row
        lcd.setCursor(0, 0);
        lcd.print("Mood: ");
        if (mood.length() > 10) {
            lcd.print(mood.substring(0, 10)); // Displaying first 10 characters
        }

        // Print the remaining part of the mood in the second line
        lcd.setCursor(0, 1);
        if (mood.length() > 10) {
            lcd.print(mood.substring(10)); // Print the remaining characters in the second line
        } else {
            lcd.print(mood); // If mood is short, print the full mood in the second line
        }
    }

    delay(20); // Small delay for stability
}
