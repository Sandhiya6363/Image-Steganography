#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char *output_fname;
    FILE *fptr_output;

    char extn_secret_file[10];

    long size_secret_file;

    char *magic_string;

    int magic_size;
    int extn_size;

} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);
Status open_files(DecodeInfo *decInfo);
Status skip_bmp_header(FILE *fptr_stego_image);

char decode_byte_from_lsb(char *image_buffer);

Status decode_string_from_image(char *str, int size,FILE *fptr_stego_image);

Status decode_int_from_image(int *size, FILE *fptr_stego_image);

Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif