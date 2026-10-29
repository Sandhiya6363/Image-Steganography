#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>


Status do_encoding(EncodeInfo *encInfo)
{
    /* Step 1: Open all required files */
    if (open_files(encInfo) == e_failure)
    {
        printf("ERROR: Unable to open files\n");
        return e_failure;
    }

    printf("Files are opened successfully\n");


    /* Step 2: Get secret file size */
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    printf("Secret file size = %ld bytes\n",
           encInfo->size_secret_file);


    /* Step 3: Calculate magic string size */
    encInfo->magic_size = strlen(encInfo->magic);

    /* Step 4: Calculate secret file extension size */
    encInfo->extn_size = strlen(encInfo->extn_secret_file);


    /* Step 5: Check image capacity */
    if (check_capacity(encInfo) == e_failure)
    {
        printf("ERROR: Insufficient image capacity\n");

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);

        return e_failure;
    }

    printf("Image has sufficient capacity\n");


    /* Step 6: Copy BMP header */
    if (copy_bmp_header(encInfo->fptr_src_image,
                        encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to copy BMP header\n");

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);

        return e_failure;
    }

    printf("BMP header copied successfully\n");


    /* Step 7: Encode magic string */
    if (encode_magic_string(encInfo->magic, encInfo) == e_failure)
    {
        printf("ERROR: Unable to encode magic string\n");

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);

        return e_failure;
    }

    printf("Magic string encoded successfully\n");


    /* Step 8: Encode magic string size */
    if (encode_data_to_image(encInfo->magic_size, encInfo) == e_failure)
    {
        printf("ERROR: Unable to encode magic string size\n");

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);

        return e_failure;
    }

    printf("Magic string size encoded successfully\n");


    /* Step 9: Encode extension size */
    if (encode_int_to_image(encInfo->extn_size, encInfo) == e_failure)
    {
        printf("ERROR: Unable to encode extension size\n");

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);

        return e_failure;
    }

    printf("Extension size encoded successfully\n");


    /* Step 10: Encode secret file extension */
    if (encode_secret_file_extn(encInfo->extn_secret_file,
                                encInfo) == e_failure)
    {
        printf("ERROR: Unable to encode secret file extension\n");

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);

        return e_failure;
    }

    printf("Secret file extension encoded successfully\n");


    /* Step 11: Encode secret file size */
    if (encode_secret_file_size(encInfo->size_secret_file,
                                encInfo) == e_failure)
    {
        printf("ERROR: Unable to encode secret file size\n");

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);

        return e_failure;
    }

    printf("Secret file size encoded successfully\n");


    /* Step 12: Encode secret file data */
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("ERROR: Unable to encode secret file data\n");

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);

        return e_failure;
    }

    printf("Secret file data encoded successfully\n");


    /* Step 13: Copy remaining image data */
    if (copy_remaining_img_data(encInfo->fptr_src_image,
                                encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Unable to copy remaining image data\n");

        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);
        fclose(encInfo->fptr_stego_image);

        return e_failure;
    }

    printf("Remaining image data copied successfully\n");


    /* Step 14: Close files */
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    printf("Encoding completed successfully\n");

    return e_success;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    
    if(strstr(argv[2],".bmp")!=NULL)
    {
        encInfo->src_image_fname = argv[2];
        
    }
    else
    {
        printf("ERROR: The Input file does not have .bmp extension\n");
        return e_failure;
    }

    if(strstr(argv[3],".txt") != NULL)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file,".txt");
       
    }
    else
    {
        printf("ERROR: The Input file does not have .txt extension\n");
        return e_failure;
    }

    if(argv[4]==NULL)
    {
        encInfo->stego_image_fname = "stego.bmp";
        
    }
    else 
    {
        if(strstr(argv[4],".bmp")!=NULL)
        {
            encInfo->stego_image_fname = argv[4];
            
        }
        else
        {
            printf("ERROR: The Output file does not have .bmp extension\n");
            return e_failure;
        }
    }
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname,"rb");

    if(encInfo->fptr_src_image == NULL)
    {
        printf("ERROR: Unable to open the source file\n");
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname,"rb");

    if(encInfo->fptr_secret == NULL)
    {
        printf("ERROR: Unable to open the secret file\n");
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname,"wb");

    if(encInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open the output file\n");
        return e_failure;
    }
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    uint bmp = get_image_size_for_bmp(encInfo->fptr_src_image);

    uint required = (encInfo->magic_size * 8) + 32 + 32 + (encInfo->extn_size * 8) + 32 + (encInfo->size_secret_file * 8);

    printf("Image capacity = %u\n", bmp);
    printf("Required capacity = %u\n", required);

    if (bmp >= required)
        return e_success;

    return e_failure;
}


uint get_image_size_for_bmp(FILE *fptr_src_image)
{
    uint width=0,length=0;

    fseek(fptr_src_image,18,SEEK_SET);
    fread(&width,4,1,fptr_src_image);
    printf("Width = %u\n",width);
    fread(&length,4,1,fptr_src_image);
    printf("Length = %u\n",length);
    int size_of_bmp = width * length * 3;
    return size_of_bmp;
}

uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret,0,SEEK_END);
    uint tell = ftell(fptr_secret);
    rewind(fptr_secret);
    return tell;
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];
    rewind(fptr_src_image);
    if(fread(header,54,1,fptr_src_image) !=1)
    {
        return e_failure;
    }
    if(fwrite(header,54,1,fptr_dest_image)!=1)
    {
    return e_failure;
    }
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        if(fread(image_buffer,8,1,fptr_src_image) != 1)
        {
            return e_failure;
        }

        if(encode_byte_to_lsb(data[i],image_buffer) == e_failure)
        {
            return e_failure;
        }

        if(fwrite(image_buffer,8,1,fptr_stego_image) != 1)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i] = image_buffer[i] & 0xFE;

        image_buffer[i] = image_buffer[i] | ((data >> i) &1);
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int size = strlen(magic_string);
    if(encode_data_to_image((char *)magic_string,size,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int size = strlen(file_extn);
    if(encode_data_to_image((char *)file_extn,size,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    return e_success;

}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    if (fread(image_buffer, 32, 1, encInfo->fptr_src_image) != 1)
    {
        return e_failure;
    }

    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] &= 0xFE;
        image_buffer[i] |= ((file_size >> i) & 1);
    }

    if (fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image) != 1)
    {
        return e_failure;
    }

    return e_success;

}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    size_t bytes_read;
    while ((bytes_read=fread(encInfo->secret_data,1,MAX_SECRET_BUF_SIZE,encInfo->fptr_secret)) > 0)
    {
        if (encode_data_to_image(encInfo->secret_data,bytes_read,encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
        {
            return e_failure;
        }
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
   
    char buffer[1024];
    int bytes;

    while ((bytes = fread(buffer, 1, 1024, fptr_src)) > 0)
    {
        if (fwrite(buffer, 1, bytes, fptr_dest) != bytes)
        {
            return e_failure;
        }
    }

    return e_success;

}

Status encode_int_to_image(int value, EncodeInfo *encInfo)
{
    char image_buffer[32];

    if (fread(image_buffer, 32, 1,
              encInfo->fptr_src_image) != 1)
    {
        return e_failure;
    }

    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] &= 0xFE;
        image_buffer[i] |= ((value >> i) & 1);
    }

    if (fwrite(image_buffer, 32, 1,
               encInfo->fptr_stego_image) != 1)
    {
        return e_failure;
    }

    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    if (encode_data_to_image(magic_string,encInfo->magic_size,encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    return e_success;
}

Status encode_secret_file_extn(char *extn, EncodeInfo *encInfo)
{
    if (encode_data_to_image(extn,encInfo->extn_size,encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    return e_success;
}

