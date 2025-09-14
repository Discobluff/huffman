#include <stdio.h>
#include <stdint.h>
#include "buffers.h"



void init_InputBuffer(InputBuffer *buffer, FILE *file) {
    buffer->input_file = file;
    buffer->file_end = 0;
    buffer->value = 0;
    buffer->bits_number = 0;
    buffer->next_value = fgetc(buffer->input_file);
    if (buffer->next_value == EOF) {
        printf("Empty file.\n");
        buffer->file_end = 1;
    }
    buffer->after_next_value = fgetc(buffer->input_file);
}

void reset_InputBuffer(InputBuffer *buffer, FILE *file) {
    init_InputBuffer(buffer, file);
}

int read_bit(InputBuffer *buffer) {
    int result;
    if (buffer->bits_number <= 0) {
        if (buffer->file_end) {
            printf("Error: End of file reached.\n");
            return EOF;
        }
        buffer->value = buffer->next_value;
        buffer->next_value = buffer->after_next_value;
        buffer->bits_number = 8;
        buffer->after_next_value = fgetc(buffer->input_file);        
        if(feof(buffer->input_file)){
            buffer->file_end = 1;
            buffer->bits_number = buffer->next_value;
        }
    }
    result = buffer->value >> 7;
    buffer->value = (buffer->value & 127) << 1;
    buffer->bits_number--;
    return result;
}

uint8_t read_byte(InputBuffer *buffer) {
    uint8_t next_byte = 0;
    for (int i = 0; i < 8; i++) {
        int next_bit = read_bit(buffer);
        next_byte = (next_bit << (7 - i)) | next_byte;
    }
    return next_byte;
}

void init_OutputBuffer(OutputBuffer *buffer, FILE *file) {
    buffer->output_file = file;    
    buffer->value = 0;
    buffer->bits_number = 0;
}

void reset_OutputBuffer(OutputBuffer *buffer, FILE *file) {
    init_OutputBuffer(buffer, file);
}

void write_bit(OutputBuffer *buffer, int bit) {    
    buffer->value = (buffer->value << 1) | (bit & 1);
    buffer->bits_number++;
    if (buffer->bits_number >= 8) {
        fputc(buffer->value, buffer->output_file);
        reset_OutputBuffer(buffer, buffer->output_file);
    }    
}

void write_byte(OutputBuffer *buffer, uint8_t next_byte) {       
    if(next_byte == 130){      
        write_byte(buffer,195);
        write_byte(buffer,169);
    }
    else{
    uint8_t tmp = next_byte;
    for (int i = 0; i < 8; i++) {
        int bit = tmp >> 7;
        write_bit(buffer, bit);
        tmp = (tmp << 1) & 0xFF;
    }
    }
}

void finish(OutputBuffer *buffer) {    
    if (buffer->bits_number > 0) {
        buffer->value <<= (8 - buffer->bits_number);
        fputc(buffer->value, buffer->output_file);
    }
    if (buffer->bits_number == 0) {
        buffer->bits_number = 8;
    }
    fputc(buffer->bits_number, buffer->output_file);
}
