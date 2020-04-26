/* Time-stamp: <2005-10-25 21:04:57 poser>
 *
 * Copyright (C) 2003 William J. Poser (billposer@alum.mit.edu)
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
 */

/*
 * This function reads as little as possible in order to check the magic number.
 * With two exceptions, it is safe to assume that on return from this function
 * the file pointer points to the first byte of the Unicode data.
 *
 * The first exception is when it returns NOMAGICNUMBER. In order to return
 * this value, it is necessary to read the first four bytes of the input,
 * which in this case are actual data, not part of the magic number.
 * The second exception is when it returns FT_UTF16_BS. To decide that
 * the input is of this type rather than FT_UTF32_BS, it is necessary to
 * read four bytes, of which only the first two belong to the magic
 * number. The third and fourth bytes represent the first character.
 *
 * In either of these cases, if you wish to read the input from the first
 * character, it will be necessary to seek backward. Since it is not possible
 * to seek on pipes, this may be a problem. As a way around this problem,
 * in these two cases the external variable UDataBegin is set to the
 * address of static storage containing the two or four bytes of character
 * data that have already been read.
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "uniftypes.h"

#define HDRLEN 4
#define MSGLEN 128

unsigned char *UDataBegin; /* This variable is intentionally NOT static */

static char msg[MSGLEN];

static unsigned char utf16_magic[] = {0xFE,0xFE};
static unsigned char utf16bs_magic[] = {0xFF,0xFE};
static unsigned char scsu_magic[] = {0x0E,0xFE,0xFF};
static unsigned char utf8_magic[] = {0xEF,0xBB,0xBF};
static unsigned char utf32_magic[] = {0x00,0x00,0xFE,0xFF};
static unsigned char utfebcdic_magic[] = {0xDD,0x73,0x73,0x73};
static unsigned char utf32bs_magic[] = {0xFF,0xFE,0x00,0x00};


/* Return a Unicode file's type based on the magic number */

int Read_UMagic_Number(int infd)
{

  static unsigned char hdr[HDRLEN];
  ssize_t nread;

  nread=read(infd,hdr,2);
  if(nread < 2){
    snprintf(msg,MSGLEN-1,"Could not read 2 bytes to check magic number.\n");
    perror(msg);
  }

  if(memcmp(hdr,utf16_magic,2) == 0) return FT_UTF_16;
  if(memcmp(hdr,utf16bs_magic,2) == 0){
    nread=read(infd,&hdr[2],2);
    if(nread < 2){
      snprintf(msg,MSGLEN-1,"Could not read 4 bytes to check magic number.\n");
      perror(msg);
    }
    if(memcmp(hdr,utf32bs_magic,4) == 0) return FT_UTF_32BS;    
    else{
      UDataBegin = &hdr[2];
      return FT_UTF_16BS;
    }
  }
  nread=read(infd,&hdr[2],1);
  if(nread < 1){
    snprintf(msg,MSGLEN-1,"Could not read 3 bytes to check magic number.\n");
    perror(msg);
  }
  if(memcmp(hdr,scsu_magic,3) == 0) return FT_SCSU;
  if(memcmp(hdr,utf8_magic,3) == 0) return FT_UTF_8;

  nread=read(infd,&hdr[3],1);
  if(nread < 1){
    snprintf(msg,MSGLEN-1,"Could not read 4 bytes to check magic number.\n");
    perror(msg);
  }

  if(memcmp(hdr,utf32_magic,4) == 0) return FT_UTF_32;
  if(memcmp(hdr,utfebcdic_magic,4) == 0) return FT_UTF_EBCDIC;

  UDataBegin = hdr;
  return NOMAGICNUMBER;
}

