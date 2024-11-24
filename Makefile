# Non-RISCV Makefile
# Credit to: https://github.com/euske/nn1 , this code is a variant of his minimalistic implementation

# Execution Procedure: 
# make mnist --> compile C-binaries
# make test_mnist --> run C-binaries

CC=gcc -O -Wall -std=c11 -pedantic
CURL=curl
GZIP=gzip
CMAKE=cmake

DOWNLOAD_URL= https://ossci-datasets.s3.amazonaws.com/mnist
DATADIR=data

MNIST_FILES= \
	$(DATADIR)/train-images-idx3-ubyte \
	$(DATADIR)/train-labels-idx1-ubyte \
	$(DATADIR)/t10k-images-idx3-ubyte \
	$(DATADIR)/t10k-labels-idx1-ubyte

all: get_mnist mnist 

get_mnist:
	-mkdir ./data
	-$(CURL) $(DOWNLOAD_URL)/train-images-idx3-ubyte.gz | \
		$(GZIP) -dc > ./data/train-images-idx3-ubyte
	-$(CURL) $(DOWNLOAD_URL)/train-labels-idx1-ubyte.gz | \
		$(GZIP) -dc > ./data/train-labels-idx1-ubyte
	-$(CURL) $(DOWNLOAD_URL)/t10k-images-idx3-ubyte.gz | \
		$(GZIP) -dc > ./data/t10k-images-idx3-ubyte
	-$(CURL) $(DOWNLOAD_URL)/t10k-labels-idx1-ubyte.gz | \
		$(GZIP) -dc > ./data/t10k-labels-idx1-ubyte

mnist_unix:
	$(CC) -S ./ -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmake -D CHIP=labchip
	cmake --build ./build/ --target blinky

mnist_windows: 
	$(CC) -S ./ -B ./build/ -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=./riscv-gcc.cmakec.o
	cmake --build ./build/ --target blinky
