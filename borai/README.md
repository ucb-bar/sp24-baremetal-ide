# BearlyML'24 BorAI (Baremetal Llama2 for RISC-V)

This is a port of the [llama2.c](https://github.com/karpathy/llama2.c/tree/master) inferencing system created by the Fall 2024 EE 194 Berkeley IC Design Project Bringup course to work with the BearlyML'24 tapeout chip from Spring 2024.

## Compiling and Running BorAI (with Float32 Matmul)

```bash
make borai
```

This will produce an ELF binary file at `./build/borai.elf`. A JTAG interface can then be initialized using

```bash
make debug ./build/borai.elf
```

This command will automatically start an OpenOCD server with the proper BearlyML'24 chip configuration, as well as a GDB window with a server connection initialized. Upon running `load` in GDB, the binary will be loaded to the chip and ready for testing.

