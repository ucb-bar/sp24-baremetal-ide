#################################
# Build
#################################

add_executable(mnist
  src/mnist.c
)

target_compile_options(mnist PRIVATE
    -O
    -Wall
    -std=c11
    -pedantic
)

# Add glossy as a private library 
target_link_libraries(mnist PRIVATE 
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)

# Add as a private library 
target_link_libraries(mnist PRIVATE 
  -L${CMAKE_BINARY_DIR}/mlnetworks -Wl,--whole-archive mlnetworks -Wl,--no-whole-archive
)

