/* Time-stamp: <2008-04-03 21:30:58 poser> */
/*
 * Copyright (C) 2008 William J. Poser (billposer@alum.mit.edu)
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
#include <fcntl.h>
#include "unicode.h"
#include "uranges.h"

#define DEFRANDCHARS 1E6
#define MSGSIZE 128

char version[]=PACKAGE_VERSION;
char compdate[]="Compiled " __DATE__ " " __TIME__ ;
char pgname[]="unifuzz";

void
ShowVersion(void){
  fprintf(stderr,"\n%s  %s\n",pgname,PACKAGE_VERSION);
  fprintf(stderr,"%s\n",compdate);
  fprintf(stderr,"Copyright (C) 2008 William J. Poser\n");
  fprintf(stderr,"Released under the terms of the GNU General Public License, version 3.\n\n");
}

void
ShowUsage(void){
  fprintf(stderr,"Generate Unicode test input.\n");
  fprintf(stderr,"Usage: %s [flags]\n",pgname);
  fprintf(stderr,"       -b restrict output to BMP\n");
  fprintf(stderr,"       -g do not emit specific characters (controls, space, non-, etc.)\n");
  fprintf(stderr,"       -h help\n");
  fprintf(stderr,"       -l emit very long lines\n");
  fprintf(stderr,"       -n emit string with embedded null\n");
  fprintf(stderr,"       -q be quiet. Omit commentary.\n");
  fprintf(stderr,"       -r <chars>  specify number of random characters to emit [default: 1e6]\n");
  fprintf(stderr,"       -S scan ranges - emit the middle character from each range\n");
  fprintf(stderr,"       -s <seed>   set seed for random number generator\n");
  fprintf(stderr,"       -u emit ill-formed UTF-8\n");
  fprintf(stderr,"       -v version information\n");
  fprintf(stderr,"Report bugs to: billposer@alum.mit.edu\n");
  putc('\n',stderr);
}

EmitLineOfX (unsigned long len, int c) {
  unsigned long i;
  for (i = 0; i < len; i++) putchar(c);
  putchar('\n');
}

EmitEmbeddedNull() {
  putchar('a');
  putchar('b');
  putchar(0x00);
  putchar('c');
  putchar('d');
  putchar('\n');
}

EmitBadUTF8 () {
  /* invalid first byte */
  putchar(0x80);
  putchar(0x80);
  /* invalid continuation byte */
  putchar(0xC0);
  putchar(0x41);
  /* too few continuation bytes */
  putchar(0xEF);		/* Calls for two continuation bytes */
  putchar(0x80);		/* Valid continuation byte */
  putchar(0x41);		/* Invalid continuation byte */
}

/* Emit the middle character from each range */
EmitAllRanges(short AboveBMPP) {
  int i;
  UTF32 scp;
  extern int Ranges_Defined;
  extern struct cr Range_Table [];
  extern void putu8(UTF32);

  for (i = 0; i < Ranges_Defined; i++) {
    scp = (Range_Table[i].e + Range_Table[i].b) / 2;
    if (!AboveBMPP && (scp > UNI_MAX_BMP)) return;
    putu8(scp);
    putchar('\n');
  }
}

EmitRandomCharacters(unsigned long n,short AboveBMPP) {
  UTF32 c;
  unsigned long k = 0;
  extern void putu8(UTF32);

  while (k < n) {
    c = (UTF32) random();
    if (AboveBMPP) {
      c %=  (UNI_MAX_UTF32 + 1);
    }
    else {
      c %= (UNI_MAX_BMP + 1);
    }
    putu8(c);
    k++;
  }
}


EmitSpecificStrings(short AboveBMPP) {
  extern void putu8(UTF32);

  putu8(0x0041);		/* A with acute accent */
  putu8(0x0301);
  putu8(0x0020);		/* Space */
  putu8(0x0301);		/* Isolated combining acute accent */
  putu8(0x0020);		/* Space */
  putu8(0x0062);		/* lower case b with Devanagari vowel sign u */
  putu8(0x0941);
  putu8(0x0020);		/* Space */
  if(AboveBMPP) {
    putu8(0x1FFFE);		/* Non-character */
    putu8(0x1FFFF);		/* Non-character */
    putu8(0x4FFFE);		/* Non-character */
    putu8(0x7FFFF);		/* Non-character */
    putu8(0x10FFFE);		/* Non-character */
    putu8(0x10FFFF);		/* Non-character */
  }
  putu8(0x0004);		/* ^D */
  putu8(0x0020);		/* Space */
  putu8(0x000C);		/* ^L */
  putu8(0x0020);		/* Space */
  putu8(0x0011);		/* ^Q */
  putu8(0x0020);		/* Space */
  putu8(0x0013);		/* ^S */
  putu8(0x0020);		/* Space */
  putu8(0x001A);		/* ^Z */
  putu8(0x0020);		/* Space */
  putu8(0x0007);		/* Bell */
  putu8(0x0020);		/* Space */
  putu8(0x0008);		/* Backspace */
  putu8(0x1361);		/* Ethiopic wordspace */
  putu8(0x3000);		/* Ideographic space */
  putu8(0xFFFC);		/* Object replacement character */
  putu8(0xFFFD);		/* Replacement character */
  putu8(0xFFFE);		/* Non-character */
  putu8(0xFFFF);		/* Non-character */
  putu8(0x0020);		/* Space */
}

int main (int ac, char *av[])
{
  short AboveBMPP = 1;

  short BadUTF8P = 0;
  short LongLinesP = 0;
  short SpecificStringsP = 1;
  short ScanRangesP = 1;
  short EmbeddedNullP = 0;

  short VerboseP = 1;

  int oc;			/* Command line option flag */
  unsigned int Seed = 0;
  long RandomCharactersToEmit = DEFRANDCHARS;
  char msg [MSGSIZE];

  extern void putu8(UTF32);

  Seed = time(NULL);
  /* Handle command line arguments */
  while( (oc = getopt(ac,av,"bghlnqr:s:Suv")) != EOF){
    switch(oc){
    case 'b':
      AboveBMPP = 0;
      break;
    case 'g':
      SpecificStringsP =  0;
      break;
    case 'h':
      ShowUsage();
      exit(2);
      break; 			/* NOTREACHED */
    case 'l':
      LongLinesP = 1;
      break;
    case 'n':
      EmbeddedNullP = 1;
      break;
    case 'q':
      VerboseP = 0;
      break;
    case 'r':
      RandomCharactersToEmit = atol(optarg);
      if (RandomCharactersToEmit < 0) RandomCharactersToEmit *= (-1);
      break;
    case 's':
      Seed = atoi(optarg);
      if (Seed < 0) Seed = 0;
      break;
    case 'S':
      ScanRangesP = 0;
      break;
    case 'u':
      BadUTF8P = 1;
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


#ifdef HAVE_SETLOCALE
   setlocale(LC_ALL,"");
#endif
#ifdef HAVE_LIBINTL_H
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
#endif

  if (BadUTF8P) EmitBadUTF8();
  if (SpecificStringsP) EmitSpecificStrings(AboveBMPP);
  if (ScanRangesP) EmitAllRanges(AboveBMPP);
  if(RandomCharactersToEmit) {
    srandom(Seed);
    if(VerboseP) fprintf(stderr,"Generating %d random characters.\n",RandomCharactersToEmit);
    printf("Seed = %u\n",Seed);
    EmitRandomCharacters(RandomCharactersToEmit,AboveBMPP);
  }
  if (EmbeddedNullP) EmitEmbeddedNull();
  if(LongLinesP) {
    EmitLineOfX(257,'a');
    EmitLineOfX(1025,'b');
    EmitLineOfX(1024000,'c');
  }
  exit(0);
}


