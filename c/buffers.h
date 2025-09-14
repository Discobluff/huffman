#ifndef BUFFERS_H
#define BUFFERS_H
#include <stdio.h>
#include <stdint.h>

typedef struct InputBuffer {
    FILE *input_file;
    int file_end;
    uint8_t value;
    int bits_number;
    uint8_t next_value;
    uint8_t after_next_value;
} InputBuffer;

typedef struct OutputBuffer {
    FILE *output_file;
    uint8_t value;
    int bits_number;
} OutputBuffer;

void init_InputBuffer(InputBuffer *buffer, FILE *file);
void reset_InputBuffer(InputBuffer *buffer, FILE *file);
int read_bit(InputBuffer *buffer);
uint8_t read_byte(InputBuffer *buffer);
void init_OutputBuffer(OutputBuffer *buffer, FILE *file);
void reset_OutputBuffer(OutputBuffer *buffer, FILE *file);
void write_bit(OutputBuffer *buffer, int bit);
void write_byte(OutputBuffer *buffer, uint8_t next_byte);
void finish(OutputBuffer *buffer);


#endif // BUFFERS_H