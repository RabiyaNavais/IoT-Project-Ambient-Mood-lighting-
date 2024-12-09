import requests  # Library for handling HTTP requests
import serial  # Library for handling serial communication with Arduino
import time  # Library for introducing delay (to comply with ThingSpeak's rate limit)

# ThingSpeak API details
write_api_key = "PRNTW7PN8SURK3TL"  # ThingSpeak Write API Key
read_api_key = "770WZ8EUSUADZWFQ"  # ThingSpeak Read API Key
channel_id = "2777765"  # ThingSpeak Channel ID

# Arduino Serial Port Config
serial_port = "/dev/tty.usbmodem101"  # Arduino serial port
baud_rate = 9600  # Speed for the serial communication

# Setting up serial connection with the Arduino
arduino = serial.Serial(serial_port, baud_rate)
time.sleep(2)  # Wait for the connection to initialize


# Function to fetch temperature from ThingSpeak (field 1)
def fetch_temperature():
    url = f"https://api.thingspeak.com/channels/{channel_id}/fields/1.json?api_key={read_api_key}&results=1"
    response = requests.get(url)

    if response.status_code == 200:
        data = response.json()
        feeds = data.get("feeds", [])
        if feeds:
            latest_entry = feeds[-1]
            temperature = latest_entry.get("field1")
            if temperature:  # Check if field1 has a value
                try:
                    return float(temperature)
                except ValueError:
                    print("Invalid temperature value received.")
                    return None
            else:
                print("field1 is missing or empty in the latest entry.")
                return None
        else:
            print("No data available in the channel feeds.")
            return None
    else:
        print("Failed to fetch temperature. HTTP Status Code:", response.status_code)
        return None


# Function to send BPM to ThingSpeak (field 2)
def send_bpm_to_thingspeak(bpm):
    url = f"https://api.thingspeak.com/update?api_key={write_api_key}&field2={bpm}"
    response = requests.get(url)

    if response.status_code == 200:
        print(f"Data sent to ThingSpeak: BPM {bpm}")
    else:
        print(f"Failed to send data to ThingSpeak. HTTP {response.status_code}")


# Infinite loop to fetch and send data
while True:
    if arduino.in_waiting > 0:
        line = arduino.readline().decode().strip()
        if "BPM:" in line:
            bpm = int(line.split(":")[1])
            print(f"Heartbeat BPM: {bpm}")

            # Send BPM to ThingSpeak (field 2)
            send_bpm_to_thingspeak(bpm)

            # Fetch temperature from ThingSpeak (field 1)
            temperature = fetch_temperature()
            if temperature is not None:
                print(f"Temperature fetched: {temperature}")

                # Send temperature back to Arduino
                arduino.write(f"{temperature}\n".encode())

    time.sleep(15)  # 15-second delay to comply with ThingSpeak rate limit
