#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "decode.h"
#include "types.h"

/* Read and validate command-line arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR: Input file does not have .bmp extension\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    if (argv[3] == NULL)
    {
        decInfo->output_fname = "Output.txt";
    }
    else
    {
        decInfo->output_fname = argv[3];
    }

    return e_success;
}

/* Open stego image and output file */
Status open_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image =
        fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open stego image\n");
        return e_failure;
    }

    decInfo->fptr_output =
        fopen(decInfo->output_fname, "wb");

    if (decInfo->fptr_output == NULL)
    {
        printf("ERROR: Unable to open output file\n");
        fclose(decInfo->fptr_stego_image);
        decInfo->fptr_stego_image = NULL;
        return e_failure;
    }

    return e_success;
}

/* Skip 54-byte BMP header */
Status skip_bmp_header(FILE *fptr_stego_image)
{
    if (fseek(fptr_stego_image, 54, SEEK_SET) != 0)
    {
        return e_failure;
    }

    return e_success;
}

/* Decode one byte from 8 image bytes */
char decode_byte_from_lsb(char *image_buffer)
{
    unsigned char data = 0;

    for (int i = 0; i < 8; i++)
    {
        data |= ((image_buffer[i] & 1) << i);
    }

    return data;
}

/* Decode a string from image */
Status decode_string_from_image(char *str, int size,
                                FILE *fptr_stego_image)
{
    char image_buffer[8];

    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, 8, 1,
                  fptr_stego_image) != 1)
        {
            return e_failure;
        }

        str[i] = decode_byte_from_lsb(image_buffer);
    }

    str[size] = '\0';

    return e_success;
}

/* Decode a 32-bit integer from image */
Status decode_int_from_image(int *size,
                             FILE *fptr_stego_image)
{
    char image_buffer[32];
    unsigned int value = 0;

    if (fread(image_buffer, 32, 1,
              fptr_stego_image) != 1)
    {
        return e_failure;
    }

    for (int i = 0; i < 32; i++)
    {
        value |= ((unsigned int)(image_buffer[i] & 1) << i);
    }

    *size = (int)value;

    return e_success;
}

/* Decode and validate magic string */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char decoded_magic[100];

    if (decInfo->magic_size <= 0 ||
        decInfo->magic_size >= sizeof(decoded_magic))
    {
        printf("ERROR: Invalid magic string size\n");
        return e_failure;
    }

    if (decode_string_from_image(decoded_magic,
                                 decInfo->magic_size,
                                 decInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    if (strcmp(decoded_magic, decInfo->magic_string) != 0)
    {
        printf("ERROR: Magic string mismatch\n");
        return e_failure;
    }

    printf("Magic string: %s\n", decoded_magic);

    return e_success;
}

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    if (decInfo->extn_size <= 0 ||
        decInfo->extn_size >= sizeof(decInfo->extn_secret_file))
    {
        printf("ERROR: Invalid extension size\n");
        return e_failure;
    }

    if (decode_string_from_image(decInfo->extn_secret_file,
                                 decInfo->extn_size,
                                 decInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    printf("Secret file extension: %s\n",
           decInfo->extn_secret_file);

    return e_success;
}

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    int size;

    if (decode_int_from_image(&size,
                              decInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    if (size < 0)
    {
        printf("ERROR: Invalid secret file size\n");
        return e_failure;
    }

    decInfo->size_secret_file = size;

    printf("Secret file size: %ld bytes\n",
           decInfo->size_secret_file);

    return e_success;
}

/* Decode secret file data and write to output file */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];

    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(image_buffer, 8, 1,
                  decInfo->fptr_stego_image) != 1)
        {
            printf("ERROR: Unable to read encoded secret data\n");
            return e_failure;
        }

        char data = decode_byte_from_lsb(image_buffer);

        if (fwrite(&data, 1, 1, decInfo->fptr_output) != 1)
        {
            printf("ERROR: Unable to write decoded data\n");
            return e_failure;
        }
    }

    return e_success;
}

/* Main decoding process */
Status do_decoding(DecodeInfo *decInfo)
{
    if (open_files(decInfo) == e_failure)
    {
        return e_failure;
    }

    printf("Files opened successfully\n");

    /* Step 1: Skip BMP header */
    if (skip_bmp_header(decInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to skip BMP header\n");
        goto failure;
    }

    /* Step 2: Decode magic string size */
    if (decode_int_from_image(&decInfo->magic_size,
                              decInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to decode magic string size\n");
        goto failure;
    }

    if (decInfo->magic_size <= 0 || decInfo->magic_size >= 100)
    {
        printf("ERROR: Invalid magic string size\n");
        goto failure;
    }

    /* Step 3: Decode and validate magic string */
    if (decode_magic_string(decInfo) == e_failure)
    {
        goto failure;
    }

    /* Step 4: Decode extension size */
    if (decode_int_from_image(&decInfo->extn_size,
                              decInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to decode extension size\n");
        goto failure;
    }

    /* Step 5: Decode extension */
    if (decode_secret_file_extn(decInfo) == e_failure)
    {
        goto failure;
    }

    /* Step 6: Decode secret file size */
    if (decode_secret_file_size(decInfo) == e_failure)
    {
        goto failure;
    }

    /* Step 7: Decode secret file data */
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        goto failure;
    }

    printf("Secret data decoded successfully\n");

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    decInfo->fptr_stego_image = NULL;
    decInfo->fptr_output = NULL;

    return e_success;

failure:
    if (decInfo->fptr_stego_image != NULL)
    {
        fclose(decInfo->fptr_stego_image);
        decInfo->fptr_stego_image = NULL;
    }

    if (decInfo->fptr_output != NULL)
    {
        fclose(decInfo->fptr_output);
        decInfo->fptr_output = NULL;
    }

    return e_failure;
}