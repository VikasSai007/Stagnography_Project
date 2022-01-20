#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"


Status open_decode_files(DecodeInfo *decInfo)
{
    // dec Image file
    decInfo->fptr_dec_image = fopen(decInfo->dec_image_fname, "r");
    //Do Error handling
    if (decInfo->fptr_dec_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->dec_image_fname);
    	return e_failure;
    }

return e_success;
}
//validating the command line arguments for decoding
Status read_and_validate_decode_args(char *argv[],int argc,DecodeInfo *decInfo){

     if( strstr(argv[2],".bmp") ){//checking for the presence and extensionof file passed through command line 

		  decInfo->dec_image_fname = argv[2];//assigning name to file
				
	 }
	 else{
			return e_failure;
	 }
  
	 if( argc == 4 ){//if output file is present 
				
		 decInfo->decode_fname = argv[3] ;
	 }
	 else
		 decInfo->decode_fname = "decode_secret";//assign a name 

	return e_success;
}

//decode lsb bit of the magic string
Status decode_magic_string(DecodeInfo *decInfo){
		
	char array[8],m_str[]="#*",str[3];
	int iter,iter1,ch,ob=0;
	fseek(decInfo->fptr_dec_image,54L,SEEK_SET);//set pointer after header

    for( iter = 0 ; iter < strlen(m_str)  ; iter++ ){

		 ch   = 0;
	     fread(array,8,1,decInfo->fptr_dec_image);//get 8 bytes
		 for( iter1 = 0 ; iter1 < 8 ; iter1++){

					if( array[iter1] & 0X01 ){ //get lsb bit
						ch = ch + 1 ;
					    if( iter1 != 7 ){
							ch <<= 1;        //updating it from msb to lsb
					     }
					}
		            else{
				         
				        if( iter1 != 7){
					         ch <<= 1;
				         }
			        }
	     }
	     decInfo->magic_string[ob++] = (char)ch;//storing in magic string

   }

   decInfo->magic_string[iter] = '\0';
   strcpy(str,decInfo->magic_string);	  
   if( strcmp(str,m_str) )//comparing whether decoded is magicstring or not
	       return e_failure;
   else
		   return  e_success;//If it is magic string 

}
//Decoding file extension size 
Status decode_secret_file_ext_size( DecodeInfo *decInfo){
	   
   char array[8];
   int iter,temp;
   fread(array,8,1,decInfo->fptr_dec_image);//read 8bytes from file pointer
   for( iter = 0 ; iter <= 7 ; iter++ ){

         	if( array[iter] & 0X01 ){ //get lsb

				  temp = temp + 1 ;
				  if( iter != 7 ){
					   temp <<= 1;//update from msb to lsb
				   }
			}
		    else{
				         
				   if( iter != 7){

					     temp <<= 1;
				    }
			}
   }

   temp >>= 1;	
   decInfo->file_extn_size = temp;//update file extension size 
   return e_success;
		
}
//decodes file extension 
Status decode_secret_file_extn( DecodeInfo *decInfo ){

   int nxt , temp ,pre ,ob = 0;
   char array[8],str1[14],str[5];
   for( nxt = 0 ; nxt < (decInfo->file_extn_size) ; nxt++ ){

        temp = 0;
		fread(array,8,1,decInfo->fptr_dec_image);//get 8bytes
		for( pre = 0 ; pre < 8 ; pre++ ){

						if(  array[pre] & 0X01 ){//get lsb

								temp = temp + 1;
						}
						temp <<= 1;
		}
		temp >>= 1;
		decInfo->secret_file_extn[ob++] = (char)temp;//update extension
	}
	decInfo->secret_file_extn[ob] = '\0';
	strcpy(str1,decInfo->decode_fname);//copy previously assigned name to another  string 
	decInfo->decode_fname = (char*)malloc(strlen(decInfo->decode_fname)+strlen(decInfo->secret_file_extn)+1);//allocate memory inorder to attach the output file with extension 
	strcpy(decInfo->decode_fname,str1);
	strcat(decInfo->decode_fname,decInfo->secret_file_extn);//concatinate extension with file name 	
    decInfo->fptr_decode = fopen(decInfo->decode_fname, "w");//open output file once we got extension in write mode 
	return e_success;
}
//Decoding size of secret file 
Status decode_secret_file_size( DecodeInfo *decInfo ){

    int nxt , temp ,pre ;
    char array[8],ch,str[100];
	//for loop with respect to size of extension
	for( nxt  = 0  ;  nxt  <  (decInfo->file_extn_size) ; nxt++ ){
				
		 temp = 0;
         fread(array,8,1,decInfo->fptr_dec_image);//get 8 bytes from file pointer of decoded image 			
		 for( pre = 0 ; pre <= 7 ; pre++ ){
                      
			  if(  array[pre]  & 0X01 ){//get lsb 

				   temp = temp + 1;
				   if( pre != 7 ){

					   temp <<= 1;//update from msb to lsb

					}
			   }
			   else
			   {

				   if( pre != 7 ){

					  temp <<= 1;
						
					}
		        }
		}
	}

	decInfo->file_size = temp;//update the structure pointer with file size
	return e_success;
}
                
//Decoding secret file data 
Status decode_secret_file_data( DecodeInfo *decInfo ){

	int nxt , temp ,pre ,ob = 0;
	char array[8],ch;
	fseek(decInfo->fptr_decode,0L,SEEK_SET);//setting file pointer pointing to output file to beginning
   //loop withrespect to decoded secret file size 	
	for( nxt  = 0 ; nxt < decInfo->file_size ; nxt++ ){

		 temp = 0;
		 for( pre = 0 ; pre < 8 ; pre++ ){

              ch  = fgetc(decInfo->fptr_dec_image);//get bit from decode image 
			  if(  ch & 0X01 ){//get lsb

				   temp = temp + 1;
				   if( pre !=  7 ){
					   temp <<= 1;//update from msb to lsb
				   }
			   }
			   else{

				   if( pre != 7 ){

					   temp <<= 1;
				   }
	           }

		}

		fputc(temp,decInfo->fptr_decode);//store content of secret file to output i.e decode file 
	}

	return e_success;
}

		
Status do_decoding(DecodeInfo *decInfo){

    //open all files
    if ( open_decode_files(decInfo) == e_success )
    {
    	printf("INFO:Files opened successfully\n");

		if( decode_magic_string(decInfo) == e_success ){
			printf("INFO:Decoded Magic string Successfully\n");

		if( decode_secret_file_ext_size( decInfo) == e_success){
			printf("INFO:Decoded secret file extension size successfully\n");

		if( decode_secret_file_extn(decInfo) == e_success){
			printf("INFO:Decoded secret file extension successfully\n");
			
		if( decode_secret_file_size( decInfo ) == e_success ){
			printf("INFO:Decoded secret file size is successfully\n");

		if( decode_secret_file_data(decInfo) == e_success )
			printf("INFO:Decoded secret file data is successfully\n");

		else
			printf("ERROR:Decoding secret file data  failed\n");
		}
		else
			printf("ERROR:Decoding secret file size failed\n");
		}
		else
			printf("ERROR:Decoding secret file extension failed\n");
		}
		else
			printf("ERROR:Decoding secret file extension size failed\n");
		}
		else
			printf("ERROR:Decoding of Magic string failed\n");
	    }
    else
    {
    	printf("Error: open files failed\n" );
    }

return e_success;
}
