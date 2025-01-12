import struct
import time
import serial

port = "/dev/tty.usbserial-101"
# port = "/dev/tty.usbserial-1101"
# baudrate = 921600
baudrate = 115200

ser = serial.Serial(port=port, baudrate=baudrate)

CMD_READ = 0x00
CMD_WRITE = 0x01


def twos_complement(hexstr, bits):
    value = int(hexstr, 16)
    if value & (1 << (bits - 1)):
        value -= 1 << bits
    return value

def readWord(address):
    command = CMD_READ
    header = struct.pack("<LQQ", command, address, 0)
    ser.write(header)

    received = ser.read(4)
    rx_data, = struct.unpack("<L", received)
    # for i in range(len(received)):
    #   print("ret:", hex(received[i]))
    return hex(rx_data)

def writeWord(address, data):
    command = CMD_WRITE
    header = struct.pack("<LQQ", command, address, 0)
    payload = struct.pack("<L", data)
    buffer = header + payload
    # print("write:", buffer)
    
    # for i in range(len(buffer)):
    #   print("write:", hex(buffer[i]))
    ser.write(buffer)


if __name__ == "__main__":

    print("start")

    print("\nTest GCD")
    print(readWord(0x90000000))
    writeWord(0x90000004, 20)
    writeWord(0x90000008, 15)
    print(readWord(0x9000000C))

    print("\nTest GCD 2")
    writeWord(0x90000004, 17)
    writeWord(0x90000008, 34)
    print(readWord(0x9000000C))

    print("\nTest Scratch")
    writeWord(0x91000000, 0xdeadbeef)
    print(readWord(0x91000000))

    print("\nTest GCD 3")
    writeWord(0x90000004, 776)
    writeWord(0x90000008, 16296)
    print(readWord(0x9000000C))

    print("\nTest DSP")
    writeWord(0x8000000, 0xddee)
    print(readWord(0x8000000))
