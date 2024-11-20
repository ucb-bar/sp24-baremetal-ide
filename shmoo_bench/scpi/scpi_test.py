## A basic test of interactions with lab equipment via SCPI.

import pyvisa
import time

# Establish connection to the power supply
rm = pyvisa.ResourceManager()

psu = rm.open_resource('TCPIP0::192.168.1.100::INSTR')  # Replace this: Power Supply address

# Need to verify that this is the power supply. psu_verification should be 
# something like "Keysight Technologies,E3646A,0,X.X-Y.Y-Z.Z"
psu_verification = psu.query("*IDN?")

# Reset both devices and configure initial settings
psu.write("*RST")  # Reset the power supply

# Set the current limit (for example, 2A), look into this and see if actually necessary
psu.write("SOURce:CURRent 2") 

# Define the range of voltages
 
start_voltage = 0 # Starting voltage (in volts) 

end_voltage = 5 # Ending voltage (in volts) 

step_size = 0.5 # Step size (volts) 

# Loop through the voltage range and set the voltage
current_voltage = start_voltage 

while current_voltage <= end_voltage: # Set the voltage   
     psu.write(f"SOURce:VOLTage {current_voltage}")

     # Query and print the current voltage setting 
     voltage = psu.query("SOURce:VOLTage?") 
     print(f"Voltage set to: {current_voltage} V") 
 
     # Increment voltage by step size 
     current_voltage += step_size

     # Insert benchmark here to run at every step

# Close the connections
psu.close()