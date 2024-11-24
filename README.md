# CNN.c 

A scale of [https://github.com/euske/nn1](https://github.com/euske/nn1). 

# How to compile and train a CNN on mnists's dataset:

One command. `make`

# How to compile and train a CNN on RISCV (Chipyard edition)

1. `cd src`
2. `make mnist_riscv`

# How to run tests

1. `make test`


# How to add tests to run

1. Add your test under the `TEST_FILES` variable in `tests/CMakeLists` 
2. `make test`

## Organization:

"networks" folder contains the list of different neural networks you can train, which is then exported as a 
static library.