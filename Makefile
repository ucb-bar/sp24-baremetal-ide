# Non-RISCV Makefile
# Credit to: https://github.com/euske/nn1 , this code is a variant of his minimalistic implementation

# Execution Procedure: 
# make mnist --> compile C-binaries
# make test_mnist --> run C-binaries
include src/Makefile


CC=gcc -O -Wall -std=c11 -pedantic
CURL=curl
GZIP=gzip

DOWNLOAD_URL= https://ossci-datasets.s3.amazonaws.com/mnist
DATADIR=data

MNIST_FILES= \
	$(DATADIR)/train-images-idx3-ubyte \
	$(DATADIR)/train-labels-idx1-ubyte \
	$(DATADIR)/t10k-images-idx3-ubyte \
	$(DATADIR)/t10k-labels-idx1-ubyte

all: get_mnist mnist test_mnist

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

.PHONY: test
test: $(TEST_FILES)
	cmake -B build/
	$(MAKE) -C build/tests
	cd build && ctest 

