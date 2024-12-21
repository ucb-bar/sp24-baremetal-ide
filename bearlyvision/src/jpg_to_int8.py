from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

def image_to_grayscale_int8(image_path, switch=0):
    # Open the image
    if ( switch == 0):
        img = Image.open(image_path).convert('L')  # Convert to grayscale
    else:
        img = np.loadtxt(image_path, dtype=np.int16)
        img = (img + 32768).astype(np.uint16)
        
    
    
    # Convert the image to a numpy array
    grayscale_array = np.array(img, dtype=np.int16)  # Convert to a larger int to handle signed conversion
    
    # Map unsigned 8-bit (0-255) grayscale values to signed int8 (-128 to 127)
    grayscale_signed_int8 = grayscale_array - 128  # Shift the range to signed int8
    
    # Ensure dtype is int8
    grayscale_signed_int8 = grayscale_signed_int8.astype(np.int8)
    
    return grayscale_signed_int8


def view_int8_image(int8_image):
    # Convert the signed int8 array to unsigned 8-bit (0-255) for viewing
    uint8_image = (int8_image + 128).astype(np.uint8)
    
    # Convert to a PIL image for display
    img = Image.fromarray(uint8_image, mode='L')
    
    # Display the image using matplotlib or any other library
    plt.imshow(img, cmap='gray')
    plt.axis('off')  # Hide axes for a cleaner view
    plt.show()

def view_int16_image(filename):
    
    int16_image = np.loadtxt(filename, dtype=np.int16)

    print(f"Image Size: {int16_image.shape[0]} x {int16_image.shape[1]} (Height x Width)")

    # Convert the signed int16 array to unsigned 16-bit for viewing
    uint16_image = (int16_image + 100).astype(np.uint16)
    
    # Convert to a PIL image for display
    #img = Image.fromarray(uint16_image, mode='L')
    
    # Display the image using matplotlib or any other library
    plt.imshow(uint16_image, cmap='gray')
    plt.axis('off')  # Hide axes for a cleaner view
    plt.show()

def view_int8_output_image(filename):
    
    int8_image = np.loadtxt(filename, dtype=np.int8)

    #print(f"Image Size: {int8_image.shape[0]} x {int8_image.shape[1]} (Height x Width)")

    # Convert the signed int16 array to unsigned 16-bit for viewing
    uint8_image = (int8_image).astype(np.uint8)
    
    # Convert to a PIL image for display
    #img = Image.fromarray(uint16_image, mode='L')
    
    # Display the image using matplotlib or any other library
    plt.imshow(uint8_image, cmap='gray')
    plt.axis('off')  # Hide axes for a cleaner view
    plt.show()
    return int8_image

def save_array_as_c_format(array, filename, var_name="testImage"):
    height, width = array.shape  # Get dimensions of the array
    with open(filename, 'w') as file:
        file.write(f"#define HEIGHT {height}\n")
        file.write(f"#define WIDTH {width}\n\n")
        file.write(f"int8_t {var_name}[HEIGHT][WIDTH] = {{\n")
        
        for row in array:
            # Convert row to a string of comma-separated values
            row_str = ", ".join(f"{value}" for value in row)
            # Write the row to the file, wrapped in curly braces
            file.write(f"    {{{row_str}}},\n")
        
        file.write("};\n")
    print(f"C-style array saved to {filename}")


input_image_path = 'img.jpeg'  # Replace with your image path
output_image_path = ''  # Replace with desired output path if saving the image

grayscale_int8_data = image_to_grayscale_int8(input_image_path, 0)

#save_array_as_c_format(grayscale_int8_data, 'grayscale_image.c')

view_int8_image(grayscale_int8_data)

view_int16_image("output_image_original.txt")

output_int8_image = view_int8_output_image("output_image_int8.txt")

save_array_as_c_format(output_int8_image, 'grayscale_image.c')
