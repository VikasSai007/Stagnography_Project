#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types
#include "common.h"

/* Structure to store information required to
 * encode secret file to source image.
 */
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


typedef struct _EncodeInfo
{
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char secret_file_extn[MAX_SECRET_FILE_EXTN];
    long secret_file_size;
    char secret_data[MAX_SECRET_BUF_SIZE];

    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;//file size 
    uint width, height;
    uint bpp; //bits per pixel
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;


/* Encode function prototype */

/*check operation type*/
OperationType check_operation_type( char *argv[]);

/* Read and validate args from argv */
Status read_and_validate_encode_args(char *argv[],int argc,EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_encode_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
long get_image_size_for_bmp(EncodeInfo *encInfo);

/* Get file size */
long get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_stego_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/*Encode secret file extension size */
Status encode_secret_file_extn_size( char *exten , EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(char *file_name , EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
