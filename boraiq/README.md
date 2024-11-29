## Compiling and Running BorAIq (with Int8 Matmul)

```bash
make boraiq
```

This will produce an ELF binary file at `./build/boraiq.elf`. A JTAG interface can then be initialized using

```bash
make debug ./build/boraiq.elf
```

## Converting Binary Files to Headers

Within the `borai_scripts` directory up one level, you can use the `bin2array.py` command-line tool to convert a binary file into a C header array.

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
python3 bin2array.py -b ./stories260K.bin -o ./weights.h -n WEIGHTS
```

This will create a header file with a single variable of the name `WEIGHTS`, which contains the data stored within `./stories260K.bin`.
