/* Time-stamp: <2008-04-03 19:47:59 poser>
 *
 * This program is a filter that reverses its input character by character.
 * It works on both ASCII and UTF-8 Unicode.
 * 
 * Copyright (C) 2007 William J. Poser (billposer@alum.mit.edu)
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
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#else
#define gettext(x) (x)
#endif

char compdate[]="Compiled " __DATE__ " " __TIME__ ;
char pgname[]="unireverse";

void
ShowUsage(void){
  fprintf(stderr,"Read UTF-8 input line-by-line and emit reversed character-by-character.\n");
  fprintf(stderr,"       -h Print help information.\n");
  fprintf(stderr,"       -v Print version information.\n");
  putc('\n',stderr);
}

void
ShowVersion(void){
  fprintf(stderr,"\n%s  %s\n",pgname,PACKAGE_VERSION);
  fprintf(stderr,"%s\n",compdate);
  fprintf(stderr,"Copyright (C) 2007 William J. Poser\n");
  fprintf(stderr,"This program is free software; you can redistribute it and/or modify\n");
  fprintf(stderr,"it under the terms of version 3 of the GNU General Public License\n");
  fprintf(stderr,"as published by the Free Software Foundation.\n");
  fprintf(stderr,"Report bugs to: billposer@alum.mit.edu\n");
}

static const char TrailingBytes[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

int main(int ac, char **av) {
  int len;
  int i;
  int j;
  int PreviousLeadByte;
  char *obuf;
  char *optr;
  int BytesInChar;
  char *ibuf;
  unsigned long LineCnt = 0L;

  char * GetLine(FILE *, int *);

  if (ac > 1) {
    if (av[1][0] == '-') {
      if(av[1][1] == 'v') {ShowVersion(); exit(1);}
      else if(av[1][1] == 'h') {ShowUsage(); exit(1);}
      else {fprintf(stderr,"Option %c not recognized.\n",av[1][1]); exit(2);}
    }
    ShowUsage();exit(1);
  }

  /* 
   * The stratagey here is to work from the end of the line looking for
   * lead bytes. On finding one, we emit it plus the requisite number of
   * continuation bytes.
   */
  while(1) {
    ibuf = GetLine(stdin,&len);
    if(len < 0) break;
    LineCnt++; 
    if(len == 0) putchar('\n'); 

    obuf = malloc((len + 1) * sizeof(char));
    if(!obuf) {
      fprintf(stderr,"unirev: failed to allocate storage.\n");
      exit(2);
    }
    optr = obuf;
    PreviousLeadByte = len;
    for (i = len-1; i >= 0; i--) {
      if (ibuf[i] >= 0) {
	*optr++ = ibuf[i]; /* ASCII character */
	PreviousLeadByte = i;
      }
      else if (ibuf[i] & 0x40)  {
	BytesInChar = 1 + (int) TrailingBytes[(unsigned char) ibuf[i]];
	if (i + BytesInChar > PreviousLeadByte) {
	  fprintf(stderr,"Truncated UTF-8 sequence at byte %d of line %lu\n",i+1,LineCnt);
	  fprintf(stderr,"%d continuation bytes %s required but only %d %s present.\n",
		  BytesInChar-1,
		  (BytesInChar-1) >1?"are":"is",
		  PreviousLeadByte-i-1,
		  (PreviousLeadByte-i-i)>1?"are":"is");
	  exit(3);
	}
	PreviousLeadByte = i;
	for(j=0; j < BytesInChar; j++) *optr++ = ibuf[i+j];
      }
    }
    *optr = '\0';
    puts(obuf);
    free(obuf);
    free(ibuf);
  }
  exit(0);
}


/* 
 * Read a line of arbitrary length from a file.
 *
 * Return a pointer to the null-terminated string allocated, or null on failure
 * to allocate sufficient storage.
 * It is the responsibility of the caller to free the space allocated.
 *
 * The length of the line is placed in the variable pointed to by
 * the second argument. (-1) is placed in this variable on EOF.
 */

#define INITLENGTH 32

char * GetLine(FILE *fp, int *LineLength)
{
  char c;
  int Available;
  int CharsRead;
  char *Line;
  int BytesRead;

  Available = INITLENGTH;
  CharsRead=0;
  BytesRead=0;  
  Line = (char *) malloc((size_t)Available);
  if(Line == (char *) 0) return (Line);

  while(1){
    c=getc(fp);
    if(c == '\n'){
      Line[CharsRead]='\0';
      *LineLength=CharsRead;
      return(Line);
    }
    if(c == EOF){
      Line[CharsRead]='\0';
      if(BytesRead == 0) *LineLength = (-1); /* Signal EOF */
      else *LineLength=CharsRead;
      return(Line);
    }
    BytesRead++;
    if(CharsRead == (Available-1)){ /* -1 because of null */
      Available += INITLENGTH/2;
      Line = (char *) realloc( (void *) Line, (size_t) (Available * sizeof (char)));
      if(Line == (char *) 0) return(Line);
    }
    Line[CharsRead++]=c;
  }
}
