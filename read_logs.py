import serial
from datetime import datetime

# Configure serial port settings
port = '/dev/cu.usbmodemNRAQBQER2'  # Replace with your specific serial port
baud_rate = 115200  # Replace with the appropriate baud rate
timeout = 1  # Specify the timeout for reading from the serial port

# Open the serial port
ser = serial.Serial(port, baud_rate, timeout=timeout)

# Generate the filename based on current date and time
now = datetime.now()
date_time = now.strftime("%Y-%m-%d_%H-%M")
filename = f"{date_time}_SHOH.txt"

# Open a file to write the logs
with open(filename, 'w') as file:
    # Read and write the logs from the serial port
    while True:
        line = ser.readline().decode().strip()
        if line:
            file.write(line + '\n')
            print(line)  # Optional: Print the logs to the console as well

# Close the serial port when done
ser.close()

