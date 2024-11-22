# A basic test of interactions with lab equipment via SCPI. Frequency will be adjusted via the PLL

import pyvisa
import time

# Establish connection to the power supply
rm = pyvisa.ResourceManager("@py")

psu = rm.open_resource('TCPIP::169.254.58.10::gpib0,7::INSTR')

# Need to verify that this is the power supply. psu_verification should be 
# something like "Keysight Technologies,E3646A,0,X.X-Y.Y-Z.Z"
psu_verification = psu.query("*IDN?")

psu.write("*RST")  # Reset the power supply

# Set the current limit (for example, 2A), look into this and see if actually necessary
psu.write("SOURce:CURRent 2") 
 
start_voltage = 0.5

end_voltage = 1.2

step_size = 0.05  

# Loop through the voltage range and set the voltage
current_voltage = start_voltage 

# Only channel 2 works
psu.write("INST:SEL CH2")

# Set current to 100mA
psu.write("CURR 0.1")

while current_voltage <= end_voltage: # Set the voltage   

     psu.write(f"VOLT {current_voltage}")

     voltage = psu.query("VOLT?") 
     print(f"Voltage set to: {voltage}V") 
 
     current_voltage += step_size

     # Insert benchmark here to run at every step.
     # Write a benchmark that uses the PLL to adjust frequency, then have the Pico relay the frequency over to the PC.
     # Or can think of another way to retrieve the PLL CLKOUT.

psu.close()