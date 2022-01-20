#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"


int main(int argc, char *argv[])
{
  EncodeInfo encInfo ;
  DecodeInfo decInfo ;
 
  int type,state;
  
  type =  check_operation_type(argv);
  if ( type  == e_encode)//If the operation type is encoding 
  {

    
    state = read_and_validate_encode_args(argv,argc,&encInfo);
    if ( state  ==  e_success )
    {
        printf("INFO: Validation is success");
        state = do_encoding(&encInfo);
       if( state == e_success )
        {
            printf("INFO :Encoding is done Successfully\n");
        }
        else
        {
          printf("ERROR: Encode Failure");
          return -1;
        }

    }
    else
    {
      printf("./lsb_steg -e File_name.bmp    secret.txt <output.bmp>\n");
      return -1;
    }

  }
  else if ( type == e_decode )//If operation type is decoding 
  {
	state = read_and_validate_decode_args(argv,argc,&decInfo);
    if (state == e_success)
    {
        printf("INFO: Validation is success\n");
        state = do_decoding(&decInfo);
        if( state == e_success)
        {
            printf("INFO :Decoding is done Successfully\n");
        }
        else
        {
          printf("ERROR: Decode Failure\n");
          return -1;
        }

    }
    else
    {
      printf("./lsb_steg -d File_name.bmp  <output.txt>\n");
      return -1;
    }

  }
  else//If operation type  is neither encoding nor decoding 
  {
    printf("Unsupported\n./lsb_steg -e File_name.bmp    secret.txt <output.bmp>\n./lsb_steg -d File_name.bmp    <decode.txt>\n");
  }
  return 0;
}


