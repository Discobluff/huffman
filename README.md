# Huffman Project

This project implements Huffman coding, a lossless data compression algorithm. The goal is to encode and decode data efficiently using Huffman trees. The project includes implementations in both C and OCaml.

## Features
- Build Huffman trees from input data.
- Encode data into compressed binary format.
- Decode compressed data back to its original form.

---

## Executing the C Implementation

### Prerequisites
- CLANG compiler installed.
- Makefile

### Compilation
```bash
make main
```

### Compression
```bash
./main c "input.txt" "sortie"
```
- `entree.txt`: The file to compress.
- `sortie` : The compressed output file.

### Decompression
```bash
./main d "sortie" "decompression.txt"
```
- `sortie` : The compressed output file.
- `decompression.txt`: The decompressed file.

---

## Executing the OCaml Implementation

### Prerequisites
- OCaml installed.

### Compilation
```bash
ocamlc -c buffers.ml
ocamlc -c heap.ml 
ocamlc -c huff_compr.ml
ocamlc -o main buffers.cmo heap.cmo huff_compr.cmo
```

### Execution
```bash
./main
```
- `entree.txt`: The file to compress.
- `sortie` : The compressed output file.
- `decompression.txt`: The decompressed file.