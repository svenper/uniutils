/* Time-stamp: <2008-04-03 19:48:48 poser> */
/*
 * Copyright (C) 2005-2008 William J. Poser (billposer@alum.mit.edu)
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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#else
#define gettext(x) (x)
#endif
#include "unicode.h"
#include "utf8error.h"

#ifdef BMPONLY
#define CHARSETSIZE 65536 	/* 16 bits */
#else
#define CHARSETSIZE 2097152	/* Full Unicode = 21 bits */
#endif

#define MSGSIZE 128

char pgname[]="unihist";
char compdate[]=__DATE__ " " __TIME__ ;

char msg [MSGSIZE];

static long CharacterCounts[CHARSETSIZE];

void
ShowUsage(void){
  fprintf(stderr,
	  gettext("Filter to generate a histogram of the Unicode characters in the input.\n"));
  fprintf(stderr,
	  gettext("       -c Suppress printing of counts and percentages.\n"));
  fprintf(stderr,
	  gettext("       -g Suppress printing of glyphs.\n"));
  fprintf(stderr,
	  gettext("       -u Suppress printing of code as text.\n"));
  fprintf(stderr,
	  gettext("       -h Print this help message.\n"));
  fprintf(stderr,
	  gettext("       -v Print version.\n"));
}

void
ShowVersion(void){
  fprintf(stderr,"\n%s  %s\n",pgname,PACKAGE_VERSION);
#ifdef BMPONLY
  fprintf(stderr,gettext("This version compiled to handle only the BMP.\n"));
#endif
  fprintf(stderr,gettext("Compiled %s.\n"),compdate);
  fprintf(stderr,"Copyright 2005-2008 William J. Poser\n");
  fprintf(stderr,gettext("Released under the terms of the GNU General Public License, version 3.\n"));
  fprintf(stderr,gettext("Report bugs to: billposer@alum.mit.edu.\n"));
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

/* Decides whether a character is displayable */
short DisplayableP(wchar_t c){
  if(c <= 0x20) return 0;	/* ASCII control characters */
  if((c >= 0x7F) && (c <= 0xA0) ) return 0;	/* ASCII DEL and Unicode control characters */
  if(c == 0x3000) return 0;	/* Ideographic space */
  if(c == 0xFEFF) return 0; 	/* Zero width no break space */
  if((c >= 0x2000) && (c <= 0x200F) ) return 0;	/* Various spaces and direction codes */
  return 1;
}

int
main(int ac, char **av)
{
   wchar_t c;			/* Input character as UTF-32 */
   long i;			/* Loop variable */
   int infd;			/* Input file descriptor */
   int oc;			/* Getopt option flag  */
   long TotalChars;		/* Total number of characters in input */
   long ByteOffset = 0L;	/* Offset from beginning of input in bytes, counting from zero */
   long LineNumber = 0L;
   int UCBytes;			/* Bytes of input occupied by the current input character */
   unsigned char *rawptr;
   short CntP    =1;		/* Output counts and percentages? */
   short CodeP  = 1;		/* Output UTF-32 codes in ascii hex?*/
   short GlyphP = 1;		/* Output UTF-8 glyphs? */


#ifdef HAVE_NUMBER_GROUP_SEPARATOR
   char *fmtstr = "\t%7.3f\t%'8ld";
#else
   char *fmtstr = "\t%7.3f\t%8ld";
#endif

   extern int optopt;
   extern int opterr;
   extern UTF32 Get_UTF32_From_UTF8 (int,int *,unsigned char **);
   extern void putu8 (wchar_t);

   opterr = 0;			/* We'll handle errors ourselves */
   while( (oc = getopt(ac,av,"cghuv")) != EOF){
     switch(oc){
     case 'c':
       CntP = 0;
       break;
     case 'g':
       GlyphP = 0;
       break;
     case 'h':
       ShowUsage();
       exit(1);
     case 'u':
       CodeP = 0;
       break;
     case 'v':
       ShowVersion();
       exit(1);
     default:
       fprintf(stderr,gettext("%s: invalid option flag %c\n"),pgname,optopt);
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

   infd = fileno(stdin);

   /* Initialize counts */
   for(i=0L;i<CHARSETSIZE;i++) CharacterCounts[i] = 0L;
   TotalChars = 0L;
   
   /* Count */
   while ( (c = Get_UTF32_From_UTF8(infd,&UCBytes,&rawptr)) <= UNI_MAX_UTF32){  
     ByteOffset += UCBytes;
     if (c == 0x000A) LineNumber++;
#ifdef BMPONLY
     if(c > CHARSETSIZE){
       fprintf(stderr,
	       gettext("Encountered input outside the BMP (plane 0): 0x%06X at byte offset %ld.\n"),
	       c,ByteOffset-UCBytes);
       fprintf(stderr,
	       gettext("This version of %s has been compiled to handle only characters in the BMP.\n"),pgname);
       fprintf(stderr,
	       gettext("Recompile with BMPONLY undefined to handle all of Unicode.\n"));
       exit(3);
     }
#endif
     CharacterCounts[c]+=1L;
     ++TotalChars;
   }
   (void)HandleReadError(c,rawptr,LineNumber,TotalChars,ByteOffset);

   /* Output */
   for(i=0L;i < CHARSETSIZE; i++){
     if(CharacterCounts[i] == 0) continue;
     if(CntP) printf(fmtstr,100.0 * (((double)CharacterCounts[i])/(double)TotalChars),CharacterCounts[i]);
     if(CodeP) printf("\t0x%06lX",i);
     if(GlyphP && DisplayableP(i)){ putchar('\t');putu8(i);}
     putchar('\n');
   }

   exit(0);
}
