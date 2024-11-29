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

boraiq:
	cmake -S ./ -B ./build/ -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -DCHIP=bearly24
	cmake --build ./build/ --target boraiq

ifeq (debug,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "debug", ignoring them
  DEBUGGER_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(DEBUGGER_ARGS):;@:)
endif

.PHONY: debug
debug:
	@openocd -f ./platform/$(CHIP)/$(CHIP).cfg & $(DG) $(DEBUGGER_ARGS) --eval-command="target extended-remote localhost:3333" --eval-command="monitor reset"

.PHONY: ocd
ocd:
	openocd -f ./platform/$(CHIP)/$(CHIP).cfg

ifeq (gdb,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "debug", ignoring them
  DEBUGGER_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(DEBUGGER_ARGS):;@:)
endif

.PHONY: gdb
gdb:
	$(DG) $(DEBUGGER_ARGS) --eval-command="target extended-remote localhost:3333" --eval-command="monitor reset"

.PHONY: clean
clean:
	rm -rf build

.PHONY: dump
dump:
	riscv64-unknown-elf-objdump -D  build/app.elf > dump.txt