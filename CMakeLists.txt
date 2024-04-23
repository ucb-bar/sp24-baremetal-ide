cmake_minimum_required(VERSION 3.10)
project(ConvolutionTest)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Add include directory
include_directories(include)

# Source files
set(SRC_DIR src)
set(SRC_FILES
    ${SRC_DIR}/cnn.c
    ${SRC_DIR}/mnist.c
    # Add more source files as needed
)

# Create library
add_library(mylibrary STATIC ${SRC_FILES})

# Tests
enable_testing()
add_subdirectory(tests)