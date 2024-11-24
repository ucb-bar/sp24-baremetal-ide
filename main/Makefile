# Makefile
# Credit to: https://github.com/euske/nn1 , this code is a variant of his minimalistic implementation

# Execution Procedure: 
# make mnist --> compile C-binaries
# make test_mnist --> run C-binaries

# Include all directories with headers
INCLUDE_DIR := ../include

RM=rm -rf
CC=cc -O -Wall -std=c11 -pedantic

# RISC-V Compiler related steps
RVCC=riscv64-unknown-elf-gcc -O -Wall -std=c11 -pedantic
libgloss_specs := htif_nano.specs
CFLAGS := -std=gnu11 -specs=$(libgloss_specs) -O2 -fno-common -fno-builtin-printf -Wall -Wextra $(CFLAGS)
LDFLAGS := -static -specs=$(libgloss_specs) $(LDFLAGS)

LIBS=-lm include/convolution-api.h

DATADIR=data
MNIST_FILES= \
	$(DATADIR)/train-images-idx3-ubyte \
	$(DATADIR)/train-labels-idx1-ubyte \
	$(DATADIR)/t10k-images-idx3-ubyte \
	$(DATADIR)/t10k-labels-idx1-ubyte

all: mnist

clean:
	-$(RM) mnist *.o
	-$(RM) *.riscv

clean_data: 
	-$(RM) data


mnist: ./src/mnist.c ./src/cnn.c
	$(CC) -o $@ $^ -I$(INCLUDE_DIR) $(LIBS)

test_mnist: ./mnist $(MNIST_FILES)
	./mnist $(MNIST_FILES)

mnist_riscv: ./src/mnist.c ./src/cnn.c
	$(RVCC) $(LDFLAGS) -o mnist.riscv $^ -I$(INCLUDE_DIR) $(LIBS)


