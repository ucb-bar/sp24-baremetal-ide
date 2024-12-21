# CNN.c 

A scale of [https://github.com/euske/nn1](https://github.com/euske/nn1). 

# How to compile and train a CNN on RISCV (Chipyard edition)

- Requirements: 
    1. spike
    2. RISCV toolchain 
    3. GNU Make
    4. CMake


To run, simply call `make mnist_sim`. Make sure to have sourced `ee290` environment variables to avoid any dependency issues, but the code should theoretically work on any environment containing the requirements I've listed above.

## Organization:

### Adding a new neural network 

"networks" folder contains the list of different neural networks you can train, which is then exported as a 
static library.

### Adding a new neural network application

Place any new applications within the `main` folder. 