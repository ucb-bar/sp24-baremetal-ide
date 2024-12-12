### Executables ###
add_compile_options(-g)
add_executable(borai_qtdp
  qtdp/src/main.c
)

target_include_directories(borai_qtdp PUBLIC qtdp/include)

### Dependencies ###

target_link_libraries(borai_qtdp PRIVATE 
  -L${CMAKE_BINARY_DIR}/glossy -Wl,--whole-archive glossy -Wl,--no-whole-archive -Wl,--defsym=__boot_hart=$2 -Wl,--defsym=__main=main
)