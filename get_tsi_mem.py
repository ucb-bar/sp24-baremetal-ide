import struct
import time
import serial

# port = "/dev/tty.usbserial-2101"
port = "/dev/tty.usbserial-1101"
baudrate = 921600

ser = serial.Serial(port=port, baudrate=baudrate)


CMD_READ = 0x00
CMD_WRITE = 0x01

NUM_MOTORS = 8

BASE = 0x81000000
OFFSET = 0x100


TARGET_POS = 0x0
TARGET_VEL = 0x4
STATE = 0x8
ENABLE = 0xC
DIR = 0x10
PRESC = 0x14 
    
ENC_POS = 0x20
ENC_VEL = 0x28
ENC_RST = 0x30

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


def readJoint(addr, num):
    return readWord(BASE + addr + (num * OFFSET))

def writeJoint(num, addr, value):
    writeWord(BASE + addr + (num * OFFSET), value)


def set_motor_speed(num, value):
    writeJoint(num, STATE, 0)         
    writeJoint(num, PRESC, 200)
    writeJoint(num, TARGET_VEL, value)
    if value == 0:
        writeJoint(num, ENABLE, 0)
    else:
        writeJoint(num, ENABLE, 1)


def set_motor_pos(num, value):
    writeJoint(num, STATE, 1)
    writeJoint(num, TARGET_POS, value)
    writeJoint(num, PRESC, 200)
    writeJoint(num, ENABLE, 1)

def get_motor_status(num):
    pos_target = twos_complement(readJoint(TARGET_POS, num), 32)
    vel_target = twos_complement(readJoint(TARGET_VEL, num), 32)
    mode = twos_complement(readJoint(STATE, num), 32)
    enable = twos_complement(readJoint(ENABLE, num), 32)
    presc = twos_complement(readJoint(PRESC, num), 32)

    print(f"motor number:{num}")
    print(f"pos:{pos_target}, vel:{vel_target}")
    print(f"mode:{mode}, en:{enable}, presc:{presc}\n")

if __name__ == "__main__":

    print("start")

    # print(readWord(0x400C))
    writeWord(0x1600000004, 20)
    writeWord(0x1600000008, 15)
    print(readWord(0x160000000C))

    writeWord(0x1600000004, 17)
    writeWord(0x1600000008, 34)
    print(readWord(0x160000000C))

    writeWord(0x1600000004, 776)
    writeWord(0x1600000008, 16296)
    print(readWord(0x160000000C))

    print("write")
    writeWord(0x80000000, 0xDEADBEEF)
    print("read")
    print(readWord(0x80000000))

    # print("write")
    # writeWord(0x10020000, 0x1)
    # print("read")
    # print(readWord(0x10020000))
