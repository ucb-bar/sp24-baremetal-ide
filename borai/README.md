# BearlyML'24 BorAI (Baremetal Llama2 for RISC-V)

This is a port of the [llama2.c](https://github.com/karpathy/llama2.c/tree/master) inferencing system created by the Fall 2024 EE 194 Berkeley IC Design Project Bringup course to work with the BearlyML'24 tapeout chip from Spring 2024.

## Compiling and Running BorAI (with Float32 Matmul)

```bash
make borai
```

This will produce an ELF binary file at `./build/borai/borai.elf`. A JTAG interface can then be initialized using

```bash
make debug ./build/borai/borai.elf
```

This command will automatically start an OpenOCD server with the proper BearlyML'24 chip configuration, as well as a GDB window with a server connection initialized. Upon running `load` in GDB, the binary will be loaded to the chip and ready for testing.

## Compiling and Running BorAIq (with Int8 Matmul)

```bash
make boraiq
```

This will produce an ELF binary file at `./build/borai/boraiq.elf`. A JTAG interface can then be initialized using

```bash
make debug ./build/borai/boraiq.elf
```

## Converting Binary Files to Headers

Within the `scripts` directory, you can use the `bin2array.py` command-line tool to convert a binary file into a C header array.

```
usage: bin2array [-h] -b BINARY -o OUTPUT -n VARNAME [-c ROWCOUNT]

Converts a binary file into a C array.

options:
  -h, --help            show this help message and exit
  -b BINARY, --binary BINARY
                        Path to the binary file to convert.
  -o OUTPUT, --output OUTPUT
                        Output path for the C header file.
  -n VARNAME, --varname VARNAME
                        Name of the C array variable to declare inside the header file.
  -c ROWCOUNT, --rowcount ROWCOUNT
                        Optional number of elements to store per line within the C array.
```

We can, for example, convert weights in the following manner:

```bash
./bin2array.py -b ./stories260K.bin -o ./weights.h -n WEIGHTS
```

This will create a header file with a single variable of the name `WEIGHTS`, which contains the data stored within `./stories260K.bin`.
