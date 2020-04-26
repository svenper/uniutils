/* Time-stamp: <2006-07-12 23:12:12 poser>
 *
 * This function reads from a file descriptor presumed to contain text encoded
 * in native order UTF-32 and returns the next UTF-32 character.
 * No conversion is necessary in this case, so the function is trivial.
 * It is provided for compatibility with functions that actually
 * perform a conversion.
 * 
 * Author: Bill Poser (wjposer@unagi.cis.upenn.edu)
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include "utf8error.h"

#define FALSE 0
#define TRUE 1


typedef unsigned long	UTF32;	/* At least 32 bits */

#define UNI_MAX_UTF32 (UTF32)0x7FFFFFFF


UTF32 Get_UTF32 (int fd, int *bytes,unsigned char **rb)
{
  int BytesRead;
  UTF32 ch;

  BytesRead = read(fd,(void *) &ch,sizeof(UTF32));
  if (BytesRead == 0) return (UTF8_ENDOFFILE);
  if (BytesRead < 0) return (UTF8_IOERROR); 
  *bytes = 4;

  *rb=(unsigned char *)&ch;
  if(ch <= UNI_MAX_UTF32) return(ch);
  else return(UTF8_BADOUTCODE);
}
