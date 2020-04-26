/* Time-stamp: <2008-05-20 16:24:25 poser>
 *
 * Prints the names of the characters in a Unicode file.
 * 
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
#include <stdlib.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#else
#define gettext(x) (x)
#endif
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "unicode.h"
#include "utf8error.h"
#include "uranges.h"
#include "offsettbl.h"
#include "nelson.h"

#define MSGSIZE 128

char version[]=PACKAGE_VERSION;
char compdate[]="Compiled " __DATE__ " " __TIME__ ;
char pgname[]="uniname";

char msg [MSGSIZE];

extern int Offset_Entries;
extern struct oft Offset_Table[];

static short CharacterOffsetP = 1;	/* Show character offset? */
static short ByteOffsetP = 1;	/* Show byte offset? */
static short UTF32CodeP = 1;		/* Show UTF32 code? */
static short EncodingP = 1;		/* Show encoding? */
static short UnicodeRangeP =0; 	/* Show Unicode range? */
static short UnicodeNameP =1; 	/* Show Unicode name? */
static short ShowGlyphP = 1;		/* Display glyph? */
static short LineNumberP = 0;	/* Display line number? */
static short SkipAsciiP = 0;	/* Skip ASCII characters */
static short SkipAsciiSpaceP = 0;	/* Skip ASCII whitespace characters */
static short SkipBMPP =0;	/* Skip characters within BMP */


void
ShowVersion(void){
  fprintf(stderr,"\n%s  %s\n",pgname,PACKAGE_VERSION);
  fprintf(stderr,"%s\n",compdate);
  fprintf(stderr,"Copyright (C) 2003-2008 William J. Poser\n");
  fprintf(stderr,"Released under the terms of the GNU General Public License, version 3.\n\n");
}

void
ShowUsage(void){
  fprintf(stderr,"Name the characters in a Unicode file.\n");
  fprintf(stderr,"For each character, prints the character and byte offsets,\n");
  fprintf(stderr,"the hex UTF32 character code, the encoding as a sequence of hex byte values\n");
  fprintf(stderr,"the glyph, the range, and the character's Unicode name.\n");
  fprintf(stderr,"Usage: %s [flags] (<file name>)\n",pgname);
  fprintf(stderr,"       -a Skip ASCII characters.\n");
  fprintf(stderr,"       -A Skip ASCII whitespace characters.\n");
  fprintf(stderr,"       -B Skip characters within the BMP.\n");
  fprintf(stderr,"       -s <character offset>  skip to specified character.\n");
  fprintf(stderr,"       -S <byte offset>  skip to specified byte.\n");
  fprintf(stderr,"       -l Supply line number.\n");
  fprintf(stderr,"       -r Supply Unicode range.\n");
  fprintf(stderr,"       -b Suppress printing of byte offset.\n");
  fprintf(stderr,"       -c Suppress printing of character offset.\n");
  fprintf(stderr,"       -e Suppress printing of encoding.\n");
  fprintf(stderr,"       -g Suppress printing of glyph.\n");
  fprintf(stderr,"       -p Suppress printing of headers every screenfull.\n");
  fprintf(stderr,"       -n Suppress printing of Unicode name.\n");
  fprintf(stderr,"       -u Suppress printing of UTF-32 code.\n");
  fprintf(stderr,"       -h Print help information.\n");
  fprintf(stderr,"       -v Print version information.\n");
  fprintf(stderr,"       -q Input is native-order UTF-32 [default is UTF-8].\n");
  fprintf(stderr,"       -V Validate input (determine whether it is valid UTF-8).\n");
  fprintf(stderr,"Report bugs to: billposer@alum.mit.edu\n");
  putc('\n',stderr);
}

long GetOffset(UTF32 ch)
{
  int l;			/* Lower bound of region */
  int u;			/* Upper bound of region */
  int m;			/* Midpoint of region */
  
  l = 0;
  u = Offset_Entries -1;

  /* Standard binary search */
  while(l <= u){
    m = (l + u) / 2;
    if(ch < Offset_Table[m].b) u = m - 1;
    else if(ch > Offset_Table[m].e)  l = m + 1;
    else return(Offset_Table[m].offset);
    /* NOTREACHED */
  }
}


/* Used for transliterating Hangul blocks */

static char *Choseong[]={
"G","KK","N","D","TT","L","M","B","PP","S","SS","","J","CC","C","K","T","P","H"};

static char *Jungseong[]={"A","AE","YA","YAE","EO","E","YEO","YE","O","WA","WAE","OE","YO","U","WEO","WE","WI","YU","EU","YI","I"};
static char *Jongseong[]={"","G","KK","GS","N","NJ","NH","D","L","LG","LM","LB","LS","LT","LP","LH","M","B","BS","S","SS","NG","J","C","K","T","P","H"};


static int JongseongSize = sizeof(Jongseong)/sizeof(char *);
static int MiddleBlockSize = (sizeof(Jungseong)/sizeof(char *)) * (sizeof(Jongseong)/sizeof(char *));
#define HANGUL_BLOCK_BASE 0xAC00

char *
TransliterateHangulSyllable(UTF32 c)
{
  unsigned long sc;
  unsigned long ChoseongIndex;
  unsigned long JungseongIndex;
  unsigned long JongseongIndex;
  unsigned long Next;

  static char buf[9];

  sc = c - HANGUL_BLOCK_BASE;
  ChoseongIndex = sc/MiddleBlockSize;
  Next = sc % MiddleBlockSize;
  JungseongIndex = Next / JongseongSize;
  JongseongIndex = Next % JongseongSize;
  strcpy(buf,Choseong[ChoseongIndex]);
  strcat(buf,Jungseong[JungseongIndex]);
  strcat(buf,Jongseong[JongseongIndex]);

  return(buf);
}

int
HandleReadError(UTF32 c, unsigned char *rp, long LineCnt, long CharCnt, long ByteCnt)
{

  extern void ExplicateBadUTF8(FILE *, unsigned char *);

  switch (c)
    { 
    case UTF8_NOTENOUGHBYTES:
      fprintf(stderr,gettext("Truncated UTF-8 sequence encountered at line %ld, character %ld, byte %ld.\n"),LineCnt, CharCnt, ByteCnt);
      exit(1);
      break;
    case UTF8_BADINCODE:
      fprintf(stderr,gettext("Invalid UTF-8 code encountered at line %ld, character %ld, byte %ld.\n"),LineCnt, CharCnt, ByteCnt);
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

long SlurpBytes (long StartByte, long *InputLine) {
  int c;
  long CurrentByteOffset;
  long InputLineNumber;

  CurrentByteOffset = 0L;
  InputLineNumber = 1L;

  while( (c = getc(stdin)) != EOF){
    if(c == 0x0A) InputLineNumber++;
    CurrentByteOffset++;
    if(CurrentByteOffset == StartByte) break;
  }
  if(c == EOF){
    fprintf(stderr,"Reached end of file before specified byte offset.\n");
    exit(3);
  }
  *InputLine = InputLineNumber;
  return CurrentByteOffset;
}

/* Decides whether a character is displayable */
short DisplayableP(UTF32 c){
  if(c <= 0x20) return 0;	/* ASCII control character */
  if((c >= 0x7F) && (c <= 0xA0) ) return 0;	/* ASCII DEL and Unicode control characterss */
  if(c == 0x3000) return 0;	/* Ideographic space */
  if(c == 0xFEFF) return 0; 	/* Zero width no break space */
  if((c >= 0x2000) && (c <= 0x200F) ) return 0;	/* Various spaces and direction codes */
  else return 1;
}

void PutHeader(){
  if(CharacterOffsetP) printf("character  ");
  if(ByteOffsetP) printf("byte       ");
  if(LineNumberP) printf("line      ");
  if(UTF32CodeP) printf("UTF-32   ");
  if(EncodingP) printf("encoded as     ");
  if(ShowGlyphP) printf("glyph   ");
  if(UnicodeRangeP) printf("range                                   ");
  if(UnicodeNameP) printf("name");
  fputs("\n",stdout);
}

int main (int ac, char *av[])
{
  UTF32 c;
  int oc;			/* Command line option flag */
  int UCBytes;
  long int Current_Byte_Offset;
  long int Current_Char_Offset;
  int infd;
  unsigned char *rawptr;
  char rawstr[24];
  char *rawstrptr;
  char *nameptr;
  char *endptr;			/* Used by strtoul */
  char *tmpstr;			/* Temp string */
  int prpos;
  int tmpint;			/* Temporary value */
  int i;
  long NameOffset;		/* Must be signed */
  int RangeIndex;
  UTF32 Maximum_Known_Code;
  unsigned int FirstNelson;	/* Index of first entry in array of Nelson codes */
  int NelsonEntries;		/* The number of Nelson codes matching a Unicode code */
  long StartCharacterOffset = 0L;
  long StartByteOffset = 0L;
  long OutputLineNumber;
  long PreviousHeaderOutputLineNumber;
  long InputLineNumber = 1L;

  int Screen_Lines = 24;

  short NelsonP = 1;		/* Show Nelson code for kanji if available? */
  short Header_Each_Page_P = 1;	/* Print column headers every screenfull? */
  short ValidateP = 0;		/* Just check to see if the file is all valid UTF-8? */

  char *udefstr = "Undefined";
  char *nospecinfstr = "No character name available"; 
  char *charudefrangestr = "Character in undefined range"; 
  char *beyondstrptr ="Codepoint beyond known range";
  /*  char *privateusestr ="Unknown character in Private Use Area"; */

  char *eptr;			/* For checking strtol conversions */

  UTF32 (*rfunc)(int,int *,unsigned char **);
  
  extern char *unames[];

  extern UTF32 Get_UTF32_From_UTF8 (int,int *,unsigned char **);
  extern UTF32 Get_UTF32(int,int *,unsigned char **);
  extern int optind;
  extern int opterr;
  extern int optopt;
  extern int GetRange(UTF32);
  extern struct cr Range_Table [];
  extern int GetNelson(UTF32,unsigned int *);
  extern void putu8(UTF32);

  extern struct unmap NelsonTbl[];

  opterr = 0;

  rfunc = Get_UTF32_From_UTF8;

  CharacterOffsetP = 1;	/* Show character offset? */
  ByteOffsetP = 1;	/* Show byte offset? */
  UTF32CodeP = 1;	/* Show UTF32 code? */
  EncodingP = 1;	/* Show encoding? */
  UnicodeRangeP =0; 	/* Show Unicode range? */
  UnicodeNameP =1; 	/* Show Unicode name? */
  ShowGlyphP = 1;	/* Display glyph? */
  
  Current_Byte_Offset = 0L;
  Current_Char_Offset = 0L;
  Maximum_Known_Code = Offset_Table[Offset_Entries-1].e;

#ifdef HAVE_SETLOCALE
   setlocale(LC_ALL,"");
#endif
#ifdef HAVE_LIBINTL_H
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
#endif

  /* Handle command line arguments */

  while( (oc = getopt(ac,av,"aAbBceghlnpqrs:S:uvV")) != EOF){
    switch(oc){
    case 'a':
      SkipAsciiP = 1;
      break;
    case 'A':
      SkipAsciiSpaceP = 1;
      break;
    case 'b':
      ByteOffsetP = 0;
      break;
    case 'B':
      SkipBMPP =1;
      break;
    case 'c':
      CharacterOffsetP = 0;
      break;
    case 'e':
      EncodingP = 0;
      break;
    case 'g':
      ShowGlyphP = 0;
      break;
    case 'h':
      ShowUsage();
      exit(2);
      break; 			/* NOTREACHED */
    case 'l':
      LineNumberP = 1;
      break;
    case 'n':
      UnicodeNameP = 0;
      break;
    case 'p':
      Header_Each_Page_P = 0;
      break;
    case 'q':
      rfunc = Get_UTF32;
      EncodingP = 0;
      break;
    case 'r':
      UnicodeRangeP = 1;
      break;
    case 's':
      StartCharacterOffset = strtol(optarg, &eptr,0);
      if( (*eptr != '\0') || (errno == EINVAL)){
	fprintf(stderr,"Start character offset %s ill-formed.\n",optarg); exit(2);
      }
      if(errno == ERANGE){
	fprintf(stderr,"Start character offset out of range.\n"); exit(2);
      }
      if(StartCharacterOffset < 0){
	fprintf(stderr,"Invalid negative start character offset %ld\n",StartCharacterOffset); exit(2);
      }
      break;
    case 'S':
      StartByteOffset = strtol(optarg, &eptr,0);
      if( (*eptr != '\0') || (errno == EINVAL)){
	fprintf(stderr,"Start byte offset %s ill-formed.\n",optarg);
	exit(2);
      }
      if(errno == ERANGE){
	fprintf(stderr,"Start byte offset out of range.\n");
	exit(2);
      }
      if(StartByteOffset < 0L){
	fprintf(stderr,"Invalid negative start byte offset.\n");
	exit(2);
      }
      break;
    case 'u':
      UTF32CodeP = 0;
      break;
    case 'V':
      ValidateP = 1;
      break;
    case 'v':
      ShowVersion();
      exit(2);
      break; 			/* NOTREACHED */
    default:
      fprintf(stderr,"%s: invalid option flag %c\n",pgname,optopt);
      ShowVersion();
      ShowUsage();
      exit(2);
    }
  } 

  if(optind < ac){
    infd = open(av[optind],O_RDONLY);
    if(infd < 0){
      snprintf(msg,MSGSIZE-1,"%s: unable to open input file.\n%s",pgname,av[optind]);
      perror(msg);
      exit(3);
    }
  }
  else infd = fileno(stdin);

  if(StartByteOffset > 0L) {
    CharacterOffsetP = 0;
    fprintf(stderr,
    "Disabling printing of character offset since initial bytes are being skipped.\n");
  }

  if(ValidateP) {
    while ( (c = (*rfunc)(infd,&UCBytes,&rawptr)) <= UNI_MAX_UTF32){
      Current_Char_Offset++;
      Current_Byte_Offset+=UCBytes;
      if(c == 0x000A) InputLineNumber++;
    }
    if(c > UNI_MAX_UTF32){
      (void)HandleReadError(c,rawptr,InputLineNumber,Current_Char_Offset,Current_Byte_Offset-UCBytes);
      /*
       * HandleReadError will return only if we reach the end of file so it's
       * save to exit with a 0 status.
       */
      exit(0);
    }
    exit(0);
  }

  /* Skip any characters to be skipped  */
  if(StartCharacterOffset > 0){
    while ( (c = (*rfunc)(infd,&UCBytes,&rawptr)) <= UNI_MAX_UTF32){
      Current_Char_Offset++;
      Current_Byte_Offset+=UCBytes;
      if(c == 0x000A) InputLineNumber++;
      if(Current_Char_Offset == StartCharacterOffset) break;
    }
    if(c > UNI_MAX_UTF32){
  
      if(HandleReadError(c,rawptr,InputLineNumber,Current_Char_Offset,Current_Byte_Offset-UCBytes) == 0){
	fprintf(stderr,"Reached end of file before specified character offset.\n");
	exit(3);
      }
    }
  }

  if(StartByteOffset > 0L){
    if(LineNumberP) Current_Byte_Offset = SlurpBytes(StartByteOffset,&InputLineNumber); 
   else {
     /* Attempt to seek. If that fails because input is a pipe, slurp bytes */
     if(lseek(infd,(off_t) StartByteOffset,SEEK_SET) < 0){
       if(errno == ESPIPE) Current_Byte_Offset = SlurpBytes(StartByteOffset,&InputLineNumber); 
       else{ 
	 fprintf(stderr,"Error seeking on input.\n");
	 exit(3);
       }
     }
     /* If we get here seek was successful */
     Current_Byte_Offset = StartByteOffset;
   }
  }

  /* Find out how many lines per screenfull */

  if(Header_Each_Page_P){
    if( (tmpstr = getenv("LINES")) != NULL){
      tmpint = strtoul(tmpstr,&endptr,0);
      if( (errno == EINVAL) || (errno == ERANGE)){
	fprintf(stderr,"Invalid value in LINES environment variable.");
	fprintf(stderr,"Using default of %d.\n",Screen_Lines);
      }
      else Screen_Lines = tmpint;
    }
    else{
      fprintf(stderr,"No LINES variable in environment so unable to determine lines per page.\n");
      fprintf(stderr,"Using default of %d.\n",Screen_Lines);
    }
  }

  if(!Header_Each_Page_P) PutHeader();
  PreviousHeaderOutputLineNumber = OutputLineNumber = 0L;
  while ( (c = (*rfunc)(infd,&UCBytes,&rawptr)) <= UNI_MAX_UTF32){
    if(c == 0x000A) InputLineNumber++;
    Current_Char_Offset++;
    Current_Byte_Offset+=UCBytes;
    OutputLineNumber++;
    if(Header_Each_Page_P && 
       ( (OutputLineNumber != PreviousHeaderOutputLineNumber) || (OutputLineNumber == 0)) &&
       (OutputLineNumber % (Screen_Lines - 2) == 1)) {
      PreviousHeaderOutputLineNumber = OutputLineNumber;
      PutHeader();
    }
    if(c > Maximum_Known_Code){
      if(CharacterOffsetP) printf("%9ld  ",Current_Char_Offset-1);
      if(ByteOffsetP) printf("%9ld  ",Current_Byte_Offset-UCBytes);
      if(LineNumberP) printf("%8ld  ",InputLineNumber);
      if(UTF32CodeP) printf("%06lX  ",c);
      if(EncodingP) printf("%-14s  ",rawstr);
      if(ShowGlyphP){
	if(DisplayableP(c)){
	  putu8(c);
	  printf("    ");
	}
	else printf("       ");
      }
      if(UnicodeRangeP)printf("%-39s ",beyondstrptr);
      if(UnicodeNameP && !UnicodeRangeP) printf("%-35s",beyondstrptr);
    }
    else{
      if(SkipAsciiP && (c < 0x80)) {
	OutputLineNumber--;
	continue;
      }
      if(SkipBMPP && (c <= UNI_MAX_BMP)) {
	OutputLineNumber--;
	continue;
      }
      if(SkipAsciiSpaceP && (c <= 0xFF && isspace (c))) {
       OutputLineNumber--;
       continue;
      }
      if(EncodingP){
	rawstrptr=rawstr;prpos=0;
	for(i=0; i<UCBytes; i++){
	  sprintf(rawstrptr+prpos," %02X",rawptr[i]);
	  prpos+=3;
	}
      }
      
      NameOffset = GetOffset(c);
      RangeIndex = GetRange(c);

      switch(NameOffset){
      case OFT_UNDEF:
	if(UnicodeRangeP) nameptr = udefstr;
	else{
	  if(strcmp(Range_Table[RangeIndex].name,"Undefined") == 0) nameptr = charudefrangestr;
	  else {
	    sprintf(msg,"Undefined in range %s",Range_Table[RangeIndex].name);
	    nameptr = msg;
	  }
	}
	break;
      case OFT_NOINF:
	if(NelsonP && (NelsonEntries = GetNelson(c,&FirstNelson))){
	  switch(NelsonEntries){
	  case 1:
	    sprintf(msg,"CJK character Nelson %4hu",NelsonTbl[FirstNelson].ne);
	    break;
	  case 2:
	    sprintf(msg,"CJK character Nelson %4hu,%4hu",NelsonTbl[FirstNelson].ne,
		    NelsonTbl[FirstNelson+1].ne);
	    break;
	  case 3:
	    sprintf(msg,"CJK character Nelson %4hu,%4hu,%4hu",NelsonTbl[FirstNelson].ne,
		    NelsonTbl[FirstNelson+1].ne,NelsonTbl[FirstNelson+2].ne);
	    break;
	  default:
	    sprintf(msg,"CJK character Nelson %4hu,%4hu,%4hu,%4hu",
		    NelsonTbl[FirstNelson].ne,
		    NelsonTbl[FirstNelson+1].ne,
		    NelsonTbl[FirstNelson+2].ne,
		    NelsonTbl[FirstNelson+3].ne);
	    break;
	  }
	  nameptr = msg;
	}
	else if((c >= 0xAC00) && (c <= 0xD7A3) ){ /* Hangul syllable */
	  sprintf(msg,"HANGUL SYLLABLE %s",TransliterateHangulSyllable(c));
	  nameptr = msg;
	}
	else{
	  if(UnicodeRangeP) nameptr = nospecinfstr;
	  else{
	    sprintf(msg,"Unknown character in range %s",Range_Table[RangeIndex].name);
	    nameptr = msg;
	  }
	}

	break;
      default:
	nameptr = unames[c - NameOffset];
	break;
      }

      if(CharacterOffsetP) printf("%9ld  ",Current_Char_Offset-1);
      if(ByteOffsetP)printf("%9ld  ",Current_Byte_Offset-UCBytes);
      if(LineNumberP) printf("%8ld  ",InputLineNumber);
      if(UTF32CodeP)printf("%06lX  ",c);
      if(EncodingP)printf("%-14s  ",rawstr);
      if(ShowGlyphP){
	if(DisplayableP(c)){
	  putu8(c);
	  printf("      ");
	}
	else printf("        ");	/* Edited 2004/12/11 */
      }
      if(UnicodeRangeP)printf("%-39s ",Range_Table[RangeIndex].name);
      /*      if(UnicodeNameP)printf("%-35s",nameptr); */
      if(UnicodeNameP)printf("%s",nameptr);
    }

    fputs("\n",stdout);
  } /* End of while */

  (void)HandleReadError(c,rawptr,InputLineNumber,Current_Char_Offset,Current_Byte_Offset-UCBytes);
  exit(0);
}


