# Non-RISCV Makefile
# Credit to: https://github.com/euske/nn1 , this code is a variant of his minimalistic implementation

# Execution Procedure: 
# make mnist --> compile C-binaries
# make test_mnist --> run C-binaries

RM=rm -f
CC=cc -O -Wall -std=c11 -pedantic
CURL=curl
GZIP=gzip

LIBS=-lm endian.h convolution-api.h

DATADIR=./data
MNIST_FILES= \
	$(DATADIR)/train-images-idx3-ubyte \
	$(DATADIR)/train-labels-idx1-ubyte \
	$(DATADIR)/t10k-images-idx3-ubyte \
	$(DATADIR)/t10k-labels-idx1-ubyte

all: test_rnn

clean:
	-$(RM) ./bnn ./mnist ./rnn *.o

get_mnist:
	-mkdir ./data
	-$(CURL) http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz | \
		$(GZIP) -dc > ./data/train-images-idx3-ubyte
	-$(CURL) http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz | \
		$(GZIP) -dc > ./data/train-labels-idx1-ubyte
	-$(CURL) http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz | \
		$(GZIP) -dc > ./data/t10k-images-idx3-ubyte
	-$(CURL) http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz | \
		$(GZIP) -dc > ./data/t10k-labels-idx1-ubyte

mnist: mnist.c cnn.c
	rm -rf mnist
	$(CC) -o $@ $^ $(LIBS)

test_mnist: ./mnist $(MNIST_FILES)
	./mnist $(MNIST_FILES)
