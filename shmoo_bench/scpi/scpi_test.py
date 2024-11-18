## A basic test of interactions with lab equipment via SCPI.

import pyvisa

rm = pyvisa.ResourceManager()

resources = rm.list_resources()

# https://pyvisa.readthedocs.io/en/latest/introduction/communication.html#getting-the-instrument-configuration-right

my_instrument = rm.open_resource(resources[0])          # currently just using the first device that appears in the tuple
my_instrument_type = my_instrument.query("*IDN?")       # to help identify what this instrument is