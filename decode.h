#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include "common.h"

/* Structure to store information required to
 * decode secret file from stego image.
 */
#define MAX_SECRET_BUFF_SIZE 1
#define MAX_IMAGE_BUFF_SIZE (MAX_SECRET_BUFF_SIZE *8)
#define MAX_FIL_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Retrieved Secret File Info */
    char *dec_image_fname;
    FILE *fptr_dec_image;
    char image_data[MAX_IMAGE_BUFF_SIZE];
    long secret_file_size;
    uint file_size;
    uint file_extn_size;
    char secret_file_extn[MAX_FIL_SUFFIX];
    char magic_string[3];

    /* Decode text Info */
    char *decode_fname;
    FILE *fptr_decode;
   
} DecodeInfo;


/* decode function prototype */

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/*Read and validate args from argv */
Status read_and_validate_decode_args(char *argv[],int argc,DecodeInfo *decInfo);

/* decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* decode secret file extension size */
Status decode_secret_file_ext_size( DecodeInfo *decInfo);

/* decode secret file extension */
Status decode_secret_file_extn( DecodeInfo *decInfo );

/* decode secret file size */
Status decode_secret_file_size( DecodeInfo *decInfo );

/* decode secret file data */
Status decode_secret_file_data( DecodeInfo *decInfo );
#endif
