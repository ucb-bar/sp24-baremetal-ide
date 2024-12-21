def scale_and_normalize_int16_to_int8(value):
    """
    Scales and normalizes a 16-bit integer to fit within the 8-bit integer range (-128 to 127),
    with clipping for overflow.
    """
    
    # Scale and normalize to the int8 range
    scaled_value = value / 32768.0 * 128.0
    print (scaled_value)

    # Clip the scaled value to the int8 range
    return max(-128, min(127, int(scaled_value)))

def process_file(input_file):
    """
    Reads a file with int16 values separated by spaces, converts them to int8 values,
    and writes the transformed values back to the same file.
    """
    try:
        # Read the file
        with open(input_file, 'r') as file:
            lines = file.readlines()

        # Transform each line
        transformed_lines = []
        for line in lines:
            int16_values = list(map(int, line.split()))
            int8_values = [scale_and_normalize_int16_to_int8(value) for value in int16_values]
            transformed_lines.append(' '.join(map(str, int8_values)))

        # Write the transformed lines back to the output file
        with open(output_file, 'w') as file:
            file.write('\n'.join(transformed_lines))

        print("Successfully processed the file, preserving row structure.")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage
input_file = "output_image_int16.txt"  # Replace with your file name
output_file = "output_image_int8.txt"  # Replace with your file name
process_file(input_file)
