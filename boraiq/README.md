## Compiling and Running BorAIq (with Int8 Matmul)

```bash
make boraiq
```

This will produce an ELF binary file at `./build/boraiq.elf`. A JTAG interface can then be initialized using

```bash
make debug ./build/boraiq.elf
```