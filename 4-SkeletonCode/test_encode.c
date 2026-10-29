#include <stdio.h>
#include "encode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    if(argc <2)
    {
        printf("ERROR: Insufficient arguments\n");
        return e_failure;
    }
    OperationType operation = check_operation_type(argv);
    if(operation == e_encode)
    {
        if(argc < 4)
        {
            printf("ERROR: Insufficient arguments for encoding\n");
            printf("Usage: ./a.out -e source.bmp secret.txt [output.bmp]\n");
            return e_failure; 
        }
    
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv, &encInfo) == e_failure)
        {
            return e_failure;
        }
        printf("Enter the Magic String:\n");
        scanf("%19s",encInfo.magic);
        if(do_encoding(&encInfo) == e_failure)
        {
            printf("Encoding Failed\n");
            return e_failure;
        }

        printf("Encoding completed successfully\n ");
        return e_success;
    }
   
    else if(operation == e_decode)
    {
        //do_decode();
    }
    else
    {
        printf("Error: Invalid Input\n");
        printf("For Encode use : ./a.out -e source.bmp secret_file.txt output.bmp\n");
        printf("For Decode use : ./a.out -d output.bmp\n");
        return e_failure;
    }
}
