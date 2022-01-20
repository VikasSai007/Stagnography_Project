#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in case of RGB color image)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 * bpp is at offset 28, size is 2 bytes
 */
long get_image_size_for_bmp(EncodeInfo *encInfo)
{
    FILE *fptr_src_image = encInfo->fptr_src_image; // Copy to local var. Just to shorten the code
    uint width, height;
    unsigned short bpp;
    long img_size;

    // Seek to 18th byte
    fseek(fptr_src_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_src_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_src_image);
    printf("height = %u\n", height);

    // Get bits per pixel
    fseek(fptr_src_image, 28, SEEK_SET);
    fread(&bpp, sizeof(short), 1, fptr_src_image);
    printf("bpp = %hu\n", bpp);

    // Store values in encInfo
    encInfo->width = width;
    encInfo->height = height;
    encInfo->bpp = bpp;

    img_size = width * height * bpp / 8; // Calculate image size
    return img_size;
}

/* Description: Open files
 * Inputs: Source image filename, Secret filename and Stego Image file name
 * Output: FILE pointers for above files
 * Return Value:
 *  e_success - If all files are opened,
 *  e_failure - On file errors
 */
Status open_encode_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    //Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    //Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }

    // All files opened, return e_success
    return e_success;

}

Status do_encoding( EncodeInfo *encInfo )
{
	long img_size; 
    // Open all files
    if ( open_encode_files(encInfo) == e_success ){

    	 printf("INFO:Files opened successfully\n");

         if( check_capacity(encInfo) == e_success ){
			 printf("INFO:Check capacity success\n");

		 if( copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success){
			 printf("INFO:BMP Successfully copied\n");

		 if( encode_magic_string(MAGIC_STRING,encInfo) == e_success){
			 printf("INFO:Encoded Magic string Successfully\n");
            //extracting extension and passing 
			 char *extn1 = strrchr( encInfo->secret_fname,'.');
		if( encode_secret_file_extn_size( extn1 ,encInfo) == e_success){
			printf("INFO:Encoded secret file extension size successfully\n");          //extracting extension and passing 
			char *extn = strrchr( encInfo->secret_fname,'.');
        if( encode_secret_file_extn( extn , encInfo ) == e_success){
			printf("INFO:Encoding secret file extension is successful\n");
			
		if( encode_secret_file_size( encInfo->secret_file_size,encInfo ) == e_success ){
			printf("INFO:Encoding secret file size is successful\n");

		if( encode_secret_file_data(encInfo) == e_success ){
			printf("INFO:Encoding secret file data is successful\n");

		if( copy_remaining_img_data( encInfo->fptr_src_image, encInfo->fptr_stego_image ) == e_success )
			printf("INFO:Successfully copied remaining image data\n");

		else
			printf("ERROR:Copying remaining image data failed\n");
		}
		else
			printf("ERROR:Encoding secret file data is failed\n");
		}
		else
			printf("ERROR:Encoding secret file size failed\n");
		}
		else
			printf("ERROR:Encoding secret file extension failed\n");
		}
		else
			printf("ERROR:Encoding secret file extension size failed\n");
		}
		else
			printf("ERROR:Encoding of Magic string failed\n");
		}
		else
			printf("ERROR:Copy of BMP is failure\n");
		}
		else
			printf("ERROR:Check capacity failed\n");
		}
    else
    {
    	printf("Error: open files failed\n" );
    }

    // Get  Image size after encoding
    img_size = get_image_size_for_bmp(encInfo);
    printf("Image size after encoding = %ld\n", img_size);
    return e_success;
}
//checking which operation to perform
OperationType check_operation_type( char *argv[1] ){
        //checking the command line argument passed depicts which operation
	if( strcmp(argv[1],"-e") == 0 )
    	return e_encode;
	else if( strcmp(argv[1],"-d") == 0 )
		return e_decode;
	else
		return e_unsupported;
}
//read and validate command line arguments 
Status read_and_validate_encode_args( char *argv[] ,int argc,EncodeInfo *encInfo){

	char *exten;
	if( (argc == 4) || (argc == 5)){ //check required args are passed or not
        //checks files with particular extension and update the structure
        if( strstr(argv[2],".bmp") ){
			
			encInfo->src_image_fname = argv[2];
				
		}
		else {
		    return e_failure;
		}
  
		exten = strstr(argv[3],".");
		if( exten != NULL ){
					
			encInfo->secret_fname = argv[3];

		}
		else{
			return e_failure;
		}

		if( argc == 5  ){//check whether output file is passed or not 

			encInfo->stego_image_fname = argv[4];
		}
		else{

			encInfo->stego_image_fname = "stego_image.bmp";//update structure with above output file name 
		}
        }
	   return e_success;
}
//cheks the capacity of outfile for encoding 
Status check_capacity( EncodeInfo *encInfo ){

       encInfo->image_capacity  = get_image_size_for_bmp(encInfo);//get ouput image size 

       encInfo->secret_file_size = get_file_size(encInfo->fptr_secret);//get size of secret file 

       if(encInfo->image_capacity > ( 54 + 32 + 32+ 32+ ( encInfo->secret_file_size ) ) )
				return e_success;
       else
                return e_failure;
}
//for getting file size 
long get_file_size(FILE *fptr)
{

       fseek(fptr,0L,SEEK_END);//set pointer to end 
       int size = ftell(fptr);//as pointer is at end it gives size 
       rewind(fptr);//to make pointer pointing to beginning 
       return size;

}
//copying the header 
Status copy_bmp_header( FILE *fptr_src_image , FILE *fptr_dest_image ){

       char array[54];
       fseek(fptr_src_image,0L,SEEK_SET);//set pointer to beginning
       fread(array,54,1,fptr_src_image);//get first 54 bytes
       fseek(fptr_dest_image,0L,SEEK_SET);//set pointer to beginning
       fwrite(array,54,1,fptr_dest_image);//copy 54 bytes to stego image

       return e_success;
}
//encoding magic string 
Status encode_magic_string( const char *magic_string , EncodeInfo *encInfo){
	   int iter;
	   for( iter = 0 ; iter < 2 ; iter++ ) {

		  fread( encInfo->image_data , 8 , 1 , encInfo->fptr_src_image);//get 8 bytes from source image 
		  encode_byte_to_lsb(magic_string[iter],encInfo->image_data);//encode this 8bytes lsb bits with magic string data 
		  encode_data_to_image(encInfo->image_data,1,encInfo->fptr_src_image,encInfo->fptr_stego_image);//store the encoded data to output image

	    }

	    return e_success;
}
//encoding data to image 
Status encode_data_to_image(const char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image){

		fwrite(data,8,size,fptr_stego_image);//stores 8 byte to output file
		return e_success;
}
//encoding byte to lsb 
Status encode_byte_to_lsb( char data , char *image_buffer){

       int iter , iter1, array[100];
       for( iter = 7 ; iter >= 0 ; --iter  ){//converting to binary 

		    array[iter] = ( ( data  >>  iter ) & 1 ) ;
       }
       iter = 7 ;
       for( iter1 = 0 ; iter1 <= 7  ; iter1++ ){//storing in lsb bit
            //0XFE = 1111 1110 keeping first 7 bits unchanged
	        image_buffer[iter1] = ( (image_buffer[iter1] & 0XFE) | array[iter] );
	        iter--;
       }  
   return e_success;
}
//encoding secret file size
Status encode_secret_file_extn_size( char *exten , EncodeInfo *encInfo){

	  char *ptr, array[8] ;
      int value = sizeof(exten);//getting length of extention
	  ptr = (char *)&value;//storing it in char pointer
	  fread(array,8,1,encInfo->fptr_src_image);//getting 8bytes from source
      encode_byte_to_lsb( *ptr , array);//storing this info in lsb
	  fwrite(array,8,1,encInfo->fptr_stego_image);//storing the encoded info to stego image 
  return e_success;

}
//encoding secret file extension
Status encode_secret_file_extn( char *exten , EncodeInfo *encInfo){

	  int iter,iter1,iter2,array[100];
      int size = strlen(exten);
	  for( iter = 0 ; iter < size ; iter++){
           //getting 8 bytes ffrom source 
           fread(encInfo->secret_file_extn,8,1,encInfo->fptr_src_image);
           for( iter1 = 7 ; iter1 >= 0 ; iter1--  ){//converting to binary

		           array[iter1] = ( ( exten[iter] >>  iter1 ) & 1 ) ;
            }

            iter1 = 7 ;

            for( iter2 = 0 ; iter2 <= 7  ; iter2++ ){

	             encInfo->secret_file_extn[iter2] = ( (encInfo->secret_file_extn[iter2] & 0XFE) | array[iter1] );
	               iter1--;

              }
			  fwrite(encInfo->secret_file_extn,8,1,encInfo->fptr_stego_image);
		}
       
	  
      return e_success;
}
//encoding secret file size 
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){

	char *ptr , array[32] ,temp[32];
	ptr =(char *)&file_size+3;//storing the file size in character pointer +3 to get msb
	int iter;

	for( iter = 0 ; iter < 4 ; iter++ ){

			fread(array,8,1,encInfo->fptr_src_image);//get 8 bytes frm src
            encode_byte_to_lsb( *ptr , array);//store in lsb
			fwrite( array , 8 ,1 , encInfo->fptr_stego_image);//store encoded info  in output file
			*(ptr--);//move from msb to lsb 
	}

  return e_success;

}
//encoding secret file dat 
Status encode_secret_file_data( EncodeInfo *encInfo ){
 
	int iter;
	char array[8] , ch[1] ;

	for( iter = 0 ; iter < (encInfo->secret_file_size) ; iter++ ){
            //get 8 bytes from source
		    fread( array , 8 , 1 , encInfo->fptr_src_image );
			//get 1 byte from secret file
			fread( ch , 1 , 1 , encInfo->fptr_secret );
			encode_byte_to_lsb( ch[0] , array );//encode the info of secretfile to lsb 
			fwrite( array , 8 , 1 , encInfo->fptr_stego_image);//store the encoded info in stego image 

	}

  return e_success;
}
//copy remaining data to output image 
Status copy_remaining_img_data( FILE *fptr_src , FILE *fptr_dest ){

    char ch;
    while( !feof(fptr_src) ){

          ch = fgetc(fptr_src);//get byte from source
	      fputc(ch,fptr_dest);//copy the byte to output file 
    }

 return e_success;
}
