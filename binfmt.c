/* Time-stamp: <2003-12-30 11:08:23 poser> */
/*
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
 * These functions format unsigned integers into base-2 text.
 * They return a pointer to static storage containing the resulting null terminated string.
 * There are four otherwise identical functions for char,short,int, and long operands.
 * They are written in such a way as to adapt at compile time to the operand sizes used
 * on a particular machine.
 */

#ifndef CHAR_BITS
#define CHAR_BITS 8
#endif

#define CBITS (sizeof(unsigned char) * CHAR_BITS)
#define SBITS (sizeof(unsigned short) * CHAR_BITS)
#define IBITS (sizeof(unsigned int) * CHAR_BITS)
#define LBITS (sizeof(unsigned long) * CHAR_BITS)

#define MASK 0x01

char *
binfmtc(unsigned char n)
{
  int i;
  static char str [CBITS+1];

  for (i = CBITS-1; i >= 0;i--){
    str[i] = (n & MASK ? '1' : '0');
    n >>= 1;
  }

  str[CBITS] = '\0';
  return(str);
}

char *
binfmts(unsigned short n)
{
  int i;
  static char str [SBITS+1];

  for (i = SBITS-1; i >= 0;i--){
    str[i] = (n & MASK ? '1' : '0');
    n >>= 1;
  }

  str[SBITS] = '\0';
  return(str);
}

char *
binfmti(unsigned int n)
{
  int i;
  static char str [IBITS+1];

  for (i = IBITS-1; i >= 0;i--){
    str[i] = (n & MASK ? '1' : '0');
    n >>= 1;
  }

  str[IBITS] = '\0';
  return(str);
}

char *
binfmtl(unsigned long n)
{
  int i;
  static char str [LBITS+1];

  for (i = LBITS-1; i >= 0;i--){
    str[i] = (n & MASK ? '1' : '0');
    n >>= 1;
  }

  str[LBITS] = '\0';
  return(str);
}
