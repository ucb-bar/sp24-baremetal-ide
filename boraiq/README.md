## Compiling and Running BorAIq (with Int8 Matmul)

```bash
make boraiq
```

This will produce an ELF binary file at `./build/boraiq.elf`. A JTAG interface can then be initialized using

```bash
make debug ./build/boraiq.elf
```

## Converting Binary Files to Headers

Within the `borai_scripts` directory up one level, you can use the `bin2array.py` command-line tool in the following manner:

```bash
python3 bin2array.py -b ./stories260K.bin -o ./weights.h -n WEIGHTS
```

This will create a header file with a single variable of the name `WEIGHTS`, which contains the data stored within `./stories260K.bin`.
