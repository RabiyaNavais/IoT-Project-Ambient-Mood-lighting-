import requests # Library for handling HTTP requests
import serial  # Library for handling serial communication with Arduino
import time # Library for introducing delay(to the thingspeak limit)

# ThingSpeak API details
api_key = "770WZ8EUSUADZWFQ"  # ThingSpeak Channel Read API Key
channel_id = "2777765"        # ThingSpeak Channel ID
field = 1                     # Field number to fetch data from (only 1 feild for temprature)

# Arduino Serial Port Config
serial_port = "/dev/tty.usbmodem101"  # Arduino serial port
baud_rate = 9600 # Speed for the serial commumnication 

# Setting up serial connection with the Arduino
arduino = serial.Serial(serial_port, baud_rate)
time.sleep(2)  # Wait for the connection to initialize


# Function to fetch data from ThingSpeak
def fetch_data():
    # URL to fetch data with all channel details 
    url = f"https://api.thingspeak.com/channels/{channel_id}/fields/{field}.json?api_key={api_key}&results=1"
    
    # Fetch the data from Thingspeak
    response = requests.get(url)

    # Checking if the connection established was successful 
    if response.status_code == 200: #200 is when the connection was succeccful 
        data = response.json() # Parse the JSON response
        latest_entry = data["feeds"][-1]  # Get the latest data entry
        value = latest_entry[f"field{field}"] # Extracting the value from the temprature field
        print(f"Sending to Arduino: {value}")
        arduino.write(f"{value}\n".encode())  # Send the data to Arduino
    else:
        print("Failed to fetch data. HTTP Status Code:", response.status_code)


# Infinite loop to fetch and send data to Arduino at regular intervals
while True:
    fetch_data()
    time.sleep(15)  # Fetch data every 15 seconds
