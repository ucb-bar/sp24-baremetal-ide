
#################################
# Build
#################################

add_executable(bvision
  src/main.c
  src/bearlyconv_lib.c
)

target_include_directories(bvision PUBLIC include)

target_link_libraries(bvision PRIVATE 
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive
)

