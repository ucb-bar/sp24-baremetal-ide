PREFIX = riscv64-unknown-elf-

CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
DG = $(PREFIX)gdb
SIZE = $(PREFIX)size

.PHONY: build
dsp24:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=dsp24
	cmake --build ./build/ --target app

bearly24:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=bearly24
	cmake --build ./build/ --target app

borai:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=bearly24
	cmake --build ./build/ --target borai

.PHONY: debug
debug: $(TARGET_BIN)
	@openocd -f ./debug/$(CHIP).cfg & $(DG) --eval-command="target extended-remote localhost:3333"

.PHONY: upload
upload: $(TARGET_BIN)
	@openocd -f ./debug/$(CHIP).cfg $(UPLOAD_COMMANDS_SRAM)

.PHONY: clean
clean:
	rm -rf build

.PHONY: dump
dump:
	riscv64-unknown-elf-objdump -D  build/app.elf > dump.txt