/* Time-stamp: <2008-05-20 16:24:49 poser>
 *
 * Describes the content of a Unicode (UTF-8) text file by reporting
 * the character ranges to which different portions of the text belong.
 * 
 * Copyright (C) 2003-2008 William J. Poser (billposer@alum.mit.edu)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 3 the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 * or go to the web page:  http://www.gnu.org/licenses/gpl.txt.
 */

#include "config.h"
#include <stdio.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#else
#define gettext(x) (x)
#endif
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "utf8error.h"
#include "unicode.h"
#include "uniftypes.h"
#include "unirange.h"

#define MSGSIZE 128

char compdate[]="Compiled " __DATE__ " " __TIME__ ;
char pgname[]="unidesc";

char *Indents[]={
  "",
  " ",
  "  ",
  "   ",
  "    ",
  "     ",
  "      ",
  "       ",
  "        ",
  "         "
};


char msg [MSGSIZE];


int
Is_ASCII_Whitespace(UTF32 c)
{
  switch(c){
  case 0x0020:			/* space */
  case 0x0009:			/* tab */
  case 0x000A:			/* newline */
  case 0x000D:			/* carriage return */
    return 1;
  default:
    return 0;
  }
}

int
Is_ASCII_Punctuation(UTF32 c)
{
  switch(c){
  case 0x0021:
  case 0x0022:
  case 0x0023:
  case 0x0024:
  case 0x0025:
  case 0x0026:
  case 0x0027:
  case 0x0028:
  case 0x0029:
  case 0x002A:
  case 0x002B:
  case 0x002C:
  case 0x002D:
  case 0x002E:
  case 0x002F:
  case 0x003A:
  case 0x003B:
  case 0x003C:
  case 0x003D:
  case 0x003E:
  case 0x003F:
  case 0x0040:
  case 0x005B:
  case 0x005C:
  case 0x005D:
  case 0x005E:
  case 0x005F:
  case 0x0060:
  case 0x007B:
  case 0x007C:
  case 0x007D:
  case 0x007E:
  case 0x007F:
     return 1;
  default:
    return 0;
  }
}

int
Is_ASCII_Digit(UTF32 c)
{
  if((c >= 0x0030) && (c <= 0x0039) ) return(1);
  else return(0);
}

void
ShowVersion(void){
  fprintf(stderr,"\n%s  %s\n",pgname,PACKAGE_VERSION);
  fprintf(stderr,"%s\n",compdate);
  fprintf(stderr,"Copyright (C) 2003-2008 William J. Poser\n");
  fprintf(stderr,"Released under the terms of the GNU General Public License, version 3.\n\n");
}

void
ShowUsage(void){
  fprintf(stderr,"Describe the content of a Unicode text file by reporting\n");
  fprintf(stderr,"the character ranges to which different portions of the text belong.\n");
  fprintf(stderr,"Usage: %s [options] (<file name>)\n",pgname);
  fprintf(stderr,"       -L List the Unicode ranges in alphabetical order\n");
  fprintf(stderr,"       -l List the Unicode ranges in numerical order\n");
  fprintf(stderr,"       -r List the Unicode ranges detected.\n");
  fprintf(stderr,"       -m Check file's magic number to determine Unicode subtype\n");
  fprintf(stderr,"       -u Input is native-order UTF-32 [default is UTF-8].\n");
  fprintf(stderr,"       -b Give file offsets in bytes rather than characters\n");
  fprintf(stderr,"       -d Treat ASCII digits as exclusive to Basic Latin range\n");
  fprintf(stderr,"       -p Treat ASCII punctuation as exclusive to Basic Latin range\n");
  fprintf(stderr,"       -w Treat ASCII whitespace as exclusive to Basic Latin range\n");
  fprintf(stderr,"       -h Print help information.\n");
  fprintf(stderr,"       -v Print version information.\n");
  putc('\n',stderr);
  fprintf(stderr,"Report bugs to: billposer@alum.mit.edu\n");
  putc('\n',stderr);
}

int
HandleReadError(UTF32 c, unsigned char *rp, long LineCnt, long CharCnt, long ByteCnt,int indent)
{

  extern void ExplicateBadUTF8(FILE *, unsigned char *);

  switch (c)
    { 
    case UTF8_NOTENOUGHBYTES:
      fprintf(stderr,gettext("Truncated UTF-8 sequence encountered at line %ld, character %ld, byte %ld.\n"),LineCnt, CharCnt, ByteCnt);
      exit(1);
      break;
    case UTF8_BADINCODE:
      fprintf(stderr,gettext("%sInvalid UTF-8 code encountered at line %ld, character %ld, byte %ld.\n"),Indents[indent],LineCnt, CharCnt, ByteCnt);
      ExplicateBadUTF8(stderr,rp);
      exit(1);
      break;
    case UTF8_BADOUTCODE:
      fprintf(stderr,gettext("Encountered invalid Unicode at line %ld, character %ld, byte %ld.\n"),LineCnt, CharCnt, ByteCnt);
      exit(1);
      break;
    case UTF8_IOERROR:
      snprintf(msg,MSGSIZE-1,gettext("Error reading input at line %ld, character %ld, byte %ld.\n"),LineCnt,CharCnt,ByteCnt);
      perror(msg);
      exit(1);
      break;
    default:			/* Normal EOF */
      return(0);
      break;			/* NOTREACHED */
    }
}


#define INITIAL_RANGE 0

int main (int ac, char *av[])
{
  UTF32 c;
  int oc;			/* Command line option flag */
  int UCBytes;
  unsigned char *rawptr;
  short Current_Range;
  short Previous_Range;
  long int Current_Byte_Offset;
  long int Current_Char_Offset;
  long int Beginning_Of_Range_Bytes;
  long int End_Of_Range_Bytes;
  long int Beginning_Of_Range_Chars;
  long int End_Of_Range_Chars;
  long int LineNumber = 0L;
  int infd;
  long int NeutralChars;
  int UnicodeType;
  int indent;
  int i;
  
  int *Ranges_Detected;

  short Check_Magic_Number_P = 0;
  short Ignore_ASCII_Punctuation_P = 1;
  short Ignore_ASCII_Digits_P = 1;
  short Ignore_ASCII_Whitespace_P = 1;
  short Use_Byte_Offset_P;
  short Just_List_Ranges_Detected_P = 0;

  UTF32 (*rfunc)(int,int *,unsigned char **);

  extern UTF32 Get_UTF32_From_UTF8 (int,int *,unsigned char **);
  extern UTF32 Get_UTF32(int,int *,unsigned char **);
  extern int Read_UMagic_Number(int);
  extern int GetRange(UTF32);
  extern void ListRanges (FILE *, short);
  extern void ListSelectedRanges (FILE *, int *);
  extern int Ranges_Defined;
  extern int optind;
  extern int optopt;
  extern int opterr;
  extern struct cr Range_Table [];

  opterr = 0;

  rfunc = Get_UTF32_From_UTF8;
  
  NeutralChars = 0L;
  Current_Byte_Offset = 0L;
  Current_Char_Offset = 0L;
  Beginning_Of_Range_Bytes = 0L;
  End_Of_Range_Bytes = 0L;
  Beginning_Of_Range_Chars = 0L;
  End_Of_Range_Chars = 0L;
  Current_Range=INITIAL_RANGE;
  Previous_Range=INITIAL_RANGE;
  Use_Byte_Offset_P = 0;


  /* Handle command line arguments */

  while( (oc = getopt(ac,av,"bdhLlmpruvw")) != EOF){
    switch(oc){
    case 'b':
      Use_Byte_Offset_P = 1;
      break;
    case 'd':
      Ignore_ASCII_Digits_P = 0;
      break;
    case 'h':
      ShowUsage();
      exit(2);
      break; 			/* NOTREACHED */
    case 'L':
      ListRanges(stdout,1);
      exit(2);
      break; 			/* NOTREACHED */
    case 'l':
      ListRanges(stdout,0);
      exit(2);
      break; 			/* NOTREACHED */
    case 'm':
      Check_Magic_Number_P = 1;
      break;
    case 'p':
      Ignore_ASCII_Punctuation_P = 0;
      break;
    case 'r':
      Just_List_Ranges_Detected_P = 1;
      break;
    case 'u':
      rfunc = Get_UTF32;
      break;
    case 'v':
      ShowVersion();
      exit(2);
      break; 			/* NOTREACHED */
    case 'w':
      Ignore_ASCII_Whitespace_P = 0;
      break;
    default:
      fprintf(stderr,"%s: invalid option flag %c\n",pgname,optopt);
      ShowVersion();
      ShowUsage();
      exit(2);
    }
  } 


#ifdef HAVE_SETLOCALE
   setlocale(LC_ALL,"");
#endif
#ifdef HAVE_LIBINTL_H
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
#endif

  if(optind < ac){
    infd = open(av[optind],O_RDONLY);
    if(infd < 0){
      snprintf(msg,MSGSIZE-1,"%s: unable to open input file.\n%s",pgname,av[optind]);
      perror(msg);
      exit(3);
    }
  }
  else infd = fileno(stdin);

  if(Check_Magic_Number_P){
    UnicodeType = Read_UMagic_Number(infd);
    switch(UnicodeType){
      case FT_UTF_8:
	rfunc = Get_UTF32_From_UTF8;
	fprintf(stderr,"UTF-8 magic number detected.\n");
	break;
      case FT_UTF_32:
	rfunc = Get_UTF32;
	fprintf(stderr,"UTF-32 magic number detected.\n");
	break;
      case NOMAGICNUMBER:
	fprintf(stderr,"No magic number detected.\n");
	exit(2);
	break;			/* NOTREACHED */
      case FT_UTF_32BS:
	fprintf(stderr,"Input is byte-swapped UTF-32.\n");
	fprintf(stderr,"I only handle UTF-8 and native order UTF-32.\n");
	exit(2);
	break;			/* NOTREACHED */
      case FT_UTF_16:
	fprintf(stderr,"Input is UTF-16.\n");
	fprintf(stderr,"I only handle UTF-8 and native order UTF-32.\n");
	exit(2);
	break;			/* NOTREACHED */
      case FT_UTF_16BS:
	fprintf(stderr,"Input is byte-swapped UTF-16.\n");
	fprintf(stderr,"I only handle UTF-8 and native order UTF-32.\n");
	exit(2);
	break;			/* NOTREACHED */
      case FT_SCSU:
	fprintf(stderr,"Input is SCSU compressed Unicode.\n");
	fprintf(stderr,"I only handle UTF-8 and native order UTF-32.\n");
	exit(2);
	break;			/* NOTREACHED */
      case FT_UTF_EBCDIC:
	fprintf(stderr,"Input is UTF EBCDIC.\n");
	fprintf(stderr,"I only handle UTF-8 and native order UTF-32.\n");
	exit(2);
	break;			/* NOTREACHED */
    default:
      fprintf(stderr,"Read_UMagic_Number returned an impossible value.\n");
      exit(1);
    }
  }

  if(Just_List_Ranges_Detected_P) {
    Ranges_Detected = malloc(sizeof(int) * Ranges_Defined);
    if(Ranges_Detected == NULL) {
      fprintf(stderr,"Failed to allocate storage for list of detected ranges.\n");
      exit(3);
    }
    for(i=0; i < Ranges_Defined; i++) Ranges_Detected[i] = 0;
  }

  while ( (c = (*rfunc)(infd,&UCBytes,&rawptr)) <= UNI_MAX_UTF32){
    Current_Char_Offset++;
    Current_Byte_Offset+=UCBytes;
    if(c == 0x000A) LineNumber++;
    Current_Range = GetRange(c);
    if(Just_List_Ranges_Detected_P) {
      Ranges_Detected[Current_Range] = 1;
      continue;
    }

    if(Current_Range == 0){
      if(Ignore_ASCII_Punctuation_P){
	if(Is_ASCII_Punctuation(c)){
	  Current_Range = Previous_Range;
	  NeutralChars++;
	}
      }
      if(Ignore_ASCII_Digits_P){
	if(Is_ASCII_Digit(c)){
	  Current_Range = Previous_Range;
	  NeutralChars++;
	}
      }
      if(Ignore_ASCII_Whitespace_P){
	if(Is_ASCII_Whitespace(c)){
	  Current_Range = Previous_Range;
	  NeutralChars++;
	}
      }
    }

    if(Current_Range != Previous_Range){
      End_Of_Range_Chars = Current_Char_Offset -2L;
      End_Of_Range_Bytes = Current_Byte_Offset -UCBytes -1;
      if(NeutralChars != Current_Char_Offset -1L){
	/* Write out information about range just terminated */
	if(Use_Byte_Offset_P){
	  printf("%9ld\t%9ld\t%s\n",
		 Beginning_Of_Range_Bytes,
		 End_Of_Range_Bytes,
		 Range_Table[Previous_Range].name);
	}
	else{
	  printf("%8ld\t%8ld\t%s\n",
		 Beginning_Of_Range_Chars,
		 End_Of_Range_Chars,
		 Range_Table[Previous_Range].name);
	}
      
	/* Record beginning of new range */
	Beginning_Of_Range_Chars = Current_Char_Offset-1L;
	Beginning_Of_Range_Bytes = Current_Byte_Offset-UCBytes;
      }
      else{			/* The first chunk consists entirely of neutral characters */
				/* So no transition - attach the neutral characters to this chunk*/
	Beginning_Of_Range_Bytes = 0L;
	Beginning_Of_Range_Chars = 0L;
      }
    }
    Previous_Range = Current_Range;
  } /* End of while */


  if(Just_List_Ranges_Detected_P) {
    printf("Ranges detected:\n");
    ListSelectedRanges(stdout,Ranges_Detected);
    goto hre;
  }

  /* Take care of final range */

  End_Of_Range_Chars = Current_Char_Offset-1L;
  End_Of_Range_Bytes = Current_Byte_Offset-1L;
  if(Beginning_Of_Range_Chars <= End_Of_Range_Chars){
    if(NeutralChars == Current_Char_Offset){
      if(Use_Byte_Offset_P){
	printf("%9ld\t%9ld\tNeutral [Basic Latin]\n",
	       Beginning_Of_Range_Bytes,
	       End_Of_Range_Bytes);
      }
      else{
	printf("%8ld\t%8ld\tNeutral [Basic Latin]\n",
	       Beginning_Of_Range_Chars,
	       End_Of_Range_Chars);
      }
    }
    else{
      if(Use_Byte_Offset_P){
	printf("%9ld\t%9ld\t%s\n",
	       Beginning_Of_Range_Bytes,
	       End_Of_Range_Bytes,
	       Range_Table[Previous_Range].name);
      }
      else{
	printf("%8ld\t%8ld\t%s\n",
	       Beginning_Of_Range_Chars,
	       End_Of_Range_Chars,
	       Range_Table[Previous_Range].name);
      }
    }
  } /* End of test for non-null current range */

  /* Check for error flags from input functions  */

  if(Use_Byte_Offset_P){
    sprintf(msg,"%ld",Beginning_Of_Range_Bytes);
    indent = 9 - strlen(msg);
  }
  else{
    sprintf(msg,"%ld",Beginning_Of_Range_Chars);
    indent = 8 - strlen(msg);
  }

 hre: HandleReadError(c,rawptr,LineNumber+1L,Current_Char_Offset,Current_Byte_Offset,indent);
  exit(0);
}

