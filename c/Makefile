CC = clang

main: buffers.o heap.o huffman.o
	$(CC) -o $@ $^



%.o: %.c
	$(CC) -c $^

clean:
	rm -f *.o main