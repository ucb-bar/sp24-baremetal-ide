import serial

# UART configuration
uart_port = "/dev/ttyUSB0"  # Replace with your UART device
baud_rate = 115200          # Replace with your baud rate
image_width = 373
image_height = 498
pgm_file = "output.pgm"

# Initialize UART
ser = serial.Serial(uart_port, baud_rate, timeout=1000000)

try:
    print("Reading bytes from UART...")
    # Read the required number of bytes
    data = ser.read(image_width * image_height)
    
    if len(data) == image_width * image_height:
        print("Received full image data.")
        # Write to PGM file
        with open(pgm_file, "wb") as f:
            # Write PGM header
            f.write(b"P5\n")
            f.write(f"{image_width} {image_height}\n".encode())
            f.write(b"255\n")
            # Write pixel data
            f.write(data)
        print(f"PGM image written to {pgm_file}.")
    else:
        print(f"Received {len(data)} bytes, which is insufficient for a {image_width}x{image_height} image.")
finally:
    ser.close()