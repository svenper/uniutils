/* Time-stamp: <2008-04-03 19:46:42 poser>
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
 *
 * Given a sequence of bytes, this program determines whether that sequence
 * constitutes a valid UTF-8 code. If not, it explains why not.
 * If so, it explains why and shows how the UTF32 value is assembled.
 *  
 * Author: Bill Poser (billposer@alum.mit.edu)
 *
 */

#include "config.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "unicode.h" 

#define FALSE 0
#define TRUE 1

char *pgname ="ExplicateUTF8";
char compdate[]="Compiled " __DATE__ " " __TIME__ ;

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of bytes that should follow.
 */

static const char TrailingBytesForUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */

static const UTF32 OffsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
					 0x03C82080UL, 0xFA082080UL, 0x82082080UL };

void
Usage(void){
  fprintf(stderr,"Determine and explain the validity of a potential UTF-8 byte sequence\n");
  fprintf(stderr,"%s: ((-o Offset) <file name>)\n",pgname);
}


void
ShowVersion(void){
  fprintf(stderr,"\n%s  %s\n",pgname,PACKAGE_VERSION);
  fprintf(stderr,"%s\n",compdate);
  fprintf(stderr,"Copyright (C) 2003-2008 William J. Poser\n");
  fprintf(stderr,"Released under the terms of the GNU General Public License, version 3.\n\n");
}

int
main(int ac, char **av){

  int BytesNeeded;
  int BytesRead;
  int UsefulBits;
  unsigned char c[6];
  int i;
  UTF32 ch;
  unsigned char *cptr;
  unsigned char ShiftedByte;
  char tempstr[33];
  int GotBits;
  int oc;
  unsigned long Offset = 0L; 
  int infd;
  int spaces;

  extern char * binfmtc(unsigned char);
  extern char * binfmtl(unsigned long);
  extern int opterr;
  extern int optind;
  extern char *optarg;
  extern int optopt;

  opterr=0;
  cptr = &(c[0]);

  while( (oc = getopt(ac,av,"ho:v")) != EOF){
    switch(oc){
    case 'h':
      Usage();
      exit(2);
      break;			/* NOTREACHED */
    case 'o':
      Offset = atol(optarg);
      break;
    case 'v':
      ShowVersion();
      exit(2);
      break;			/* NOTREACHED */
    case '?':
    default:
      fprintf(stderr,"Unrecognized option %c\n",(char) optopt);
      exit(1);
    }
  }

  if(optind < ac) {
    infd = open(av[optind], O_RDONLY);
    if(infd < 0){
      perror(NULL);
      fprintf(stderr,"%s: unable to open file %s\n",pgname,av[optind]);
      exit(2);
    }
    if(lseek(infd, (off_t)Offset,SEEK_SET) < 0){
      perror(NULL);
      exit(1);
    }
  }
  else infd = fileno(stdin);


  /* Get the first byte */
  BytesRead = read(infd,(void *) c,1);
  if (BytesRead == 0){
    fprintf(stderr,"%s: could not read first byte from input.\n",pgname);
    exit(2);
  }
  if (BytesRead < 0){
    perror(NULL);
    exit(2);
  }

  if( (c[0] & 0xC0) == 0x80){
    printf("The first byte, value 0x%02X, with bit pattern %s,\nis not a valid first byte of a UTF-8\nsequence because its high bits are 10.\nA valid first byte must be of the form 0nnnnnnn or 11nnnnnn.\n",c[0],binfmtc(c[0]));
    exit(0);
  }

  if(c[0] <= 0x7F){
    printf("The first byte, value 0x%02X, bit pattern %s, is a valid UTF-8 code by itself\nsince its high bit is 0.\n",c[0],binfmtc(c[0]));    
    exit(0);
  }

  BytesNeeded = (int) TrailingBytesForUTF8[c[0]];
  BytesRead = read(infd,(void *) &c[1],(size_t) BytesNeeded);
  if(BytesRead != BytesNeeded){
    printf("The sequence is not a valid UTF-8 character\nbecause the first byte, value 0x%02X, bit pattern %s,\ntells us that a total of %d bytes are needed but\n only %d bytes are present.\n",
	   c[0],binfmtc(c[0]),BytesNeeded+1,BytesRead+1);
    exit(1);
  } 

  for (i = 1; i <= BytesRead; i++){
    if (( c[i] & 0xC0) != 0x80){
      printf("The sequence is not a valid UTF-8 character\nbecause byte %d, value 0x%02X, bit pattern %s\nis not a valid continuation byte, whose high bits must be 10.\n",i+1,c[i],binfmtc(c[i]));
      exit(1);      
    }
  }

  /* If we get here, everything is okay, so assemble the UTF32 value */

  ch = 0;
  switch (BytesNeeded) {
    case 5:	ch += *cptr++; ch <<= 6;
    case 4:	ch += *cptr++; ch <<= 6;
    case 3:	ch += *cptr++; ch <<= 6;
    case 2:	ch += *cptr++; ch <<= 6;
    case 1:	ch += *cptr++; ch <<= 6;
    case 0:	ch += *cptr++;
  }
  ch -= OffsetsFromUTF8[BytesNeeded];

  printf("The sequence ");
  for (i = 0; i <= BytesRead; i++) printf("0x%02X     ",c[i]);
  printf("\n             ");
  for (i = 0; i <= BytesRead; i++) printf("%s ",binfmtc(c[i]));
  printf("\n");
  printf("is a valid UTF-8 character encoding equivalent to UTF32 0x%08lX.\n",ch);
  printf("The first byte tells us that there should be %d\ncontinuation bytes since it begins with %d contiguous 1s.\nThere are %d following bytes and all are valid\ncontinuation bytes since they all have high bits 10.\n",BytesNeeded,BytesNeeded+1,BytesNeeded);
  UsefulBits = 6- BytesNeeded;
  printf("The first byte contributes its low %d bits.\n",UsefulBits);
  GotBits = UsefulBits + (6 * BytesNeeded);
  printf("The remaining bytes each contribute their low 6 bits,\nfor a total of %d bits: ",GotBits);

  ShiftedByte = c[0] << (BytesNeeded +2);
  sprintf(tempstr,"%s",binfmtc(ShiftedByte));
  tempstr[6-BytesNeeded] = '\0';
  printf("%s ",tempstr); 
  for(i = 1; i <= BytesNeeded; i++){
    ShiftedByte = c[i] << 2;
    sprintf(tempstr,"%s",binfmtc(ShiftedByte));
    tempstr[6] = '\0';
    printf("%s ",tempstr); 
  }
  printf("\n");
  printf("This is padded to 32 places with %d zeros: %n%s\n",(32-GotBits),&spaces,binfmtl(ch));
  sprintf(tempstr,"                                ");
  sprintf(tempstr,"%08lX",ch);
  tempstr[28] = tempstr[7];
  tempstr[24] = tempstr[6];
  tempstr[20] = tempstr[5];
  tempstr[16] = tempstr[4];
  tempstr[12] = tempstr[3];
  tempstr[8]  = tempstr[2];
  tempstr[4]  = tempstr[1];
  tempstr[1] = 0x20;
  tempstr[2] = 0x20;
  tempstr[3] = 0x20;
  tempstr[5] = 0x20;
  tempstr[6] = 0x20;
  tempstr[7] = (unsigned char) 0x20;
  tempstr[29] =(unsigned char)  0x00;
  printf("%*s%s\n",spaces,"",tempstr);

  exit(0);
}
