
add_library(mlnetworks STATIC
    src/cnn.c
    src/conv_accel.c
)

target_include_directories(mlnetworks PUBLIC include)
target_link_libraries(mlnetworks PUBLIC m)


message(STATUS "================ ML Networks Library Building ================")

set_target_properties(mlnetworks PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/networks")

message(STATUS "================ ML Networks Library Build Complete ================")