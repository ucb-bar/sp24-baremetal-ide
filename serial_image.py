import serial

# UART configuration
uart_port = "/dev/ttyUSB3"  # Replace with your UART device
baud_rate = 115200          # Replace with your baud rate
image_width = 256
image_height = 256
pgm_file = "output.pgm"

# Initialize UART
try:
    ser = serial.Serial(uart_port, baud_rate, timeout=10)
    print("Reading bytes from UART...")

    # Read the required number of bytes
    data = ser.read(image_width * image_height)

    if len(data) == image_width * image_height:
        print("Received full image data.")

        # Validate data range
        if all(0 <= byte <= 255 for byte in data):
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
            print("Received data contains invalid byte values.")
    else:
        print(f"Received {len(data)} bytes, which is insufficient for a {image_width}x{image_height} image.")

except serial.SerialException as e:
    print(f"Serial error: {e}")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
