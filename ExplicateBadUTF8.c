/* Time-stamp: <2005-10-25 17:40:48 poser>
 *
 * Copyright (C) 2004 William J. Poser (billposer@alum.mit.edu)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * or go to the web page:  http://www.gnu.org/licenses/gpl.txt.
 *
 * Given a bad UTF-8 sequence, explain why it is bad.
 *  
 * Author: Bill Poser (wjposer@unagi.cis.upenn.edu)
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "unicode.h" 

#define FALSE 0
#define TRUE 1

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
ExplicateBadUTF8(FILE *fp, unsigned char *c){
  int BytesNeeded;
  int i;
  unsigned char *cptr;

  extern char * binfmtc(unsigned char);

  cptr = c;

  if( (c[0] & 0xC0) == 0x80){
    fprintf(fp,"The first byte, value 0x%02X, with bit pattern %s,\nis not a valid first byte of a UTF-8 sequence because its high bits are 10.\nA valid first byte must be of the form 0nnnnnnn or 11nnnnnn.\n",c[0],binfmtc(c[0]));
    return;
  }

  BytesNeeded = (int) TrailingBytesForUTF8[c[0]];

  for (i = 1; i <= BytesNeeded; i++){
    if (( c[i] & 0xC0) != 0x80){
      fprintf(fp,"The sequence is not a valid UTF-8 character because\nthe first byte, value 0x%02X, bit pattern %s,\nrequires %d continuation bytes, but of the immediately\nfollowing bytes, byte %d, value 0x%02X, bit pattern\n%s is not a valid continuation byte, since\nits high bits are not 10.\n",c[0],binfmtc(c[0]),BytesNeeded,i,c[i],binfmtc(c[i]));
      return;
    }
  }
}
