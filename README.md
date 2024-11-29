![](docs/logo_b.png)

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

## Compiling and Running BorAIq (with Int8 Matmul)

```bash
make boraiq
```

This will produce an ELF binary file at `./build/boraiq.elf`. A JTAG interface can then be initialized using

```bash
make debug ./build/boraiq.elf
```

# Chipyard Baremetal-IDE

![CI-status](https://img.shields.io/github/actions/workflow/status/ucb-bar/Baremetal-IDE/make-examples.yaml?branch=main&style=flat-square&label=CI&logo=githubactions&logoColor=fff) ![API-Docs-status](https://img.shields.io/github/actions/workflow/status/ucb-bar/Baremetal-IDE/build-docs.yaml?branch=main&style=flat-square&label=Docs&logo=googledocs&logoColor=fff)

> **WARNING⚠️**
> Baremetal-IDE is still under heavy development at the moment, so we don't guarantee the stability and backward-compatibility among versions.

Baremetal-IDE is an all-in-one tool for baremetal-level C/C++ program developments. It is part of the Chipyard ecosystem.

Baremetal-IDE features peripheral configuration, code generation, code compilation, and debugging tools for multiple RISC-V SoCs. With the board support package, user can use either the hardware-abstraction layer (HAL) functions to quickly configure and use the various supported peripheral devices, or can use the low-level (LL) macro definitions to generate code with minimal memory footprint and high performance. The modularity of the framework structure also allows fast integration of new SoCs. 


## Documentation and Getting Started

Please refer to the [Tutorial Website](https://ucb-bar.gitbook.io/chipyard/baremetal-ide/getting-started-with-baremetal-ide) for getting started with Baremetal-IDE, and refer to the [API Docs](https://ucb-bar.github.io/Baremetal-IDE/index.html) for more detailed information on the APIs.


## Simple examples

### Compiling for Spike

```bash
cmake -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake
cmake --build ./build/ --target app
```

### Compiling for FE310

```bash

```

### Compiling example programs

```bash

```

