# tests/CMakeLists.txt

# Test files
set(TEST_FILES
    bare_conv_test.c
    complete_conv_test.c
    padding_test.c
    # Add more test files as needed
)

# Build each test
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} mylibrary)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()