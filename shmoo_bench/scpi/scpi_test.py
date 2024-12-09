import matplotlib.pyplot as plt
import numpy as np

def read_data(file_name):
    freq_values = []
    voltage_values = []
    valid_values = []
    power_values = []

    with open(file_name, 'r') as file:
        for line in file:
            parts = line.strip().split(', ')
            freq = float(parts[0].split(': ')[1])
            voltage = round(float(parts[1].split(': ')[1]), 2)
            valid = parts[2].split(': ')[1]
            power = float(parts[3].split(': ')[1])

            freq_values.append(freq)
            voltage_values.append(voltage)
            valid_values.append(valid)
            power_values.append(power)
    
    return freq_values, voltage_values, valid_values, power_values

file_name = 'output.txt'

freq_values, voltage_values, valid_values, power_values = read_data(file_name)

unique_freqs = np.unique(freq_values)
unique_voltages = np.unique(voltage_values)

shmoo_plot = np.zeros((len(unique_voltages), len(unique_freqs)))
power_plot = np.zeros((len(unique_voltages), len(unique_freqs)))

for i, freq in enumerate(unique_freqs):
    for j, voltage in enumerate(unique_voltages):
        validity = valid_values[i * len(unique_voltages) + j]
        if validity == "Valid":
            shmoo_plot[j, i] = 1
            power_plot[j, i] = power_values[i * len(unique_voltages) + j]

plt.figure(figsize=(12, 8))

cmap = plt.cm.colors.ListedColormap(['red', 'springgreen'])

plt.imshow(shmoo_plot, cmap=cmap, aspect='auto', origin='lower')

for i in range(len(unique_freqs)):
    for j in range(len(unique_voltages)):
        if shmoo_plot[j, i] == 1:  # Valid
            plt.text(i, j, f'{power_plot[j, i]:.3f}W', ha='center', va='center', fontsize=8, color='black')

plt.xlabel('Frequency (MHz)')
plt.ylabel('Voltage (V)')
plt.title('Shmoo Plot with Power Consumption')

plt.xticks(np.arange(len(unique_freqs)), unique_freqs, rotation=45)
plt.yticks(np.arange(len(unique_voltages)), unique_voltages)

plt.tight_layout()
# plt.savefig("output.jpg")
plt.show()
