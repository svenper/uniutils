#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unicode.h"
#include "unirange.h"

struct cr Range_Table []={
{0x0000,0x007F,"Basic Latin"},
{0x0080,0x00FF,"Latin-1 Supplement"},
{0x0100,0x017F,"Latin Extended-A"},
{0x0180,0x024F,"Latin Extended-B"},
{0x0250,0x02AF,"IPA Extensions"},
{0x02B0,0x02FF,"Spacing Modifier Letters"},
{0x0300,0x036F,"Combining Diacritical Marks"},
{0x0370,0x03FF,"Greek and Coptic"},
{0x0400,0x04FF,"Cyrillic"},
{0x0500,0x052F,"Cyrillic Supplement"},
{0x0530,0x058F,"Armenian"},
{0x0590,0x05FF,"Hebrew"},
{0x0600,0x06FF,"Arabic"},
{0x0700,0x074F,"Syriac"},
{0x0750,0x077F,"Arabic Supplement"},
{0x0780,0x07BF,"Thaana"},
{0x07C0,0x07FF,"NKo"},
{0x08C0,0x08FF,"Undefined"},
{0x0900,0x097F,"Devanagari"},
{0x0980,0x09FF,"Bengali"},
{0x0A00,0x0A7F,"Gurmukhi"},
{0x0A80,0x0AFF,"Gujarati"},
{0x0B00,0x0B7F,"Oriya"},
{0x0B80,0x0BFF,"Tamil"},
{0x0C00,0x0C7F,"Telugu"},
{0x0C80,0x0CFF,"Kannada"},
{0x0D00,0x0D7F,"Malayalam"},
{0x0D80,0x0DFF,"Sinhala"},
{0x0E00,0x0E7F,"Thai"},
{0x0E80,0x0EFF,"Lao"},
{0x0F00,0x0FFF,"Tibetan"},
{0x1000,0x109F,"Myanmar"},
{0x10A0,0x10FF,"Georgian"},
{0x1100,0x11FF,"Hangul Jamo"},
{0x1200,0x137F,"Ethiopic"},
{0x1380,0x139F,"Ethiopic Supplement"},
{0x13A0,0x13FF,"Cherokee"},
{0x1400,0x167F,"Unified Canadian Aboriginal Syllabics"},
{0x1680,0x169F,"Ogham"},
{0x16A0,0x16FF,"Runic"},
{0x1700,0x171F,"Tagalog"},
{0x1720,0x173F,"Hanunoo"},
{0x1740,0x175F,"Buhid"},
{0x1760,0x177F,"Tagbanwa"},
{0x1780,0x17FF,"Khmer"},
{0x1800,0x18AF,"Mongolian"},
{0x18B0,0x18FF,"Undefined"},
{0x1900,0x194F,"Limbu"},
{0x1950,0x197F,"Tai Le"},
{0x1980,0x19DF,"New Tai Lue"},
{0x19E0,0x19FF,"Khmer Symbols"},
{0x1A00,0x1A1F,"Buginese"},
{0x1A20,0x1AFF,"Undefined"},
{0x1B00,0x1B7F,"Balinese"},
{0x1B80,0x1CFF,"Undefined"},
{0x1D00,0x1D7F,"Phonetic Extensions"},
{0x1D80,0x1DBF,"Phonetic Extensions Supplement"},
{0x1DC0,0x1DFF,"Combining Diacritical Marks Supplement"},
{0x1E00,0x1EFF,"Latin Extended Additional"},
{0x1F00,0x1FFF,"Greek Extended"},
{0x2000,0x206F,"General Punctuation"},
{0x2070,0x209F,"Superscripts and Subscripts"},
{0x20A0,0x20CF,"Currency Symbols"},
{0x20D0,0x20FF,"Combining Diacritical Marks for Symbols"},
{0x2100,0x214F,"Letterlike Symbols"},
{0x2150,0x218F,"Number Forms"},
{0x2190,0x21FF,"Arrows"},
{0x2200,0x22FF,"Mathematical Operators"},
{0x2300,0x23FF,"Miscellaneous Technical"},
{0x2400,0x243F,"Control Pictures"},
{0x2440,0x245F,"Optical Character Recognition"},
{0x2460,0x24FF,"Enclosed Alphanumerics"},
{0x2500,0x257F,"Box Drawing"},
{0x2580,0x259F,"Block Elements"},
{0x25A0,0x25FF,"Geometric Shapes"},
{0x2600,0x26FF,"Miscellaneous Symbols"},
{0x2700,0x27BF,"Dingbats"},
{0x27C0,0x27EF,"Miscellaneous Mathematical Symbols-A"},
{0x27F0,0x27FF,"Supplemental Arrows-A"},
{0x2800,0x28FF,"Braille Patterns"},
{0x2900,0x297F,"Supplemental Arrows-B"},
{0x2980,0x29FF,"Miscellaneous Mathematical Symbols-B"},
{0x2A00,0x2AFF,"Supplemental Mathematical Operators"},
{0x2B00,0x2BFF,"Miscellaneous Symbols and Arrows"},
{0x2C00,0x2C5F,"Glagolitic"},
{0x2C60,0x2C7F,"Latin Extended-C"},
{0x2C80,0x2CFF,"Coptic"},
{0x2D00,0x2D2F,"Georgian Supplement"},
{0x2D30,0x2D7F,"Tifinagh"},
{0x2D80,0x2DDF,"Ethiopic Extended"},
{0x2E00,0x2E7F,"Supplemental Punctuation"},
{0x2E80,0x2EFF,"CJK Radicals Supplement"},
{0x2F00,0x2FDF,"Kangxi Radicals"},
{0x2FF0,0x2FFF,"Ideographic Description Characters"},
{0x3000,0x303F,"CJK Symbols and Punctuation"},
{0x3040,0x309F,"Hiragana"},
{0x30A0,0x30FF,"Katakana"},
{0x3100,0x312F,"Bopomofo"},
{0x3130,0x318F,"Hangul Compatibility Jamo"},
{0x3190,0x319F,"Kanbun"},
{0x31A0,0x31BF,"Bopomofo Extended"},
{0x31C0,0x31EF,"CJK Strokes"},
{0x31F0,0x31FF,"Katakana Phonetic Extensions"},
{0x3200,0x32FF,"Enclosed CJK Letters and Months"},
{0x3300,0x33FF,"CJK Compatibility"},
{0x3400,0x4DBF,"CJK Unified Ideographs Extension A"},
{0x4DC0,0x4DFF,"Yijing Hexagram Symbols"},
{0x4E00,0x9FFF,"CJK Unified Ideographs"},
{0xA000,0xA48F,"Yi Syllables"},
{0xA490,0xA4CF,"Yi Radicals"},
{0xA4D0,0xA6FF,"Undefined"},
{0xA700,0xA71F,"Modifier Tone Letters"},
{0xA720,0xA7FF,"Latin Extended-D"},
{0xA800,0xA82F,"Syloti Nagri"},
{0xA830,0xA83F,"Undefined"},
{0xA840,0xA87F,"Phags-pa"},
{0xA880,0xABFF,"Undefined"},
{0xAC00,0xD7AF,"Hangul Syllables"},
{0xD7B0,0xD7FF,"Undefined"},
{0xD800,0xDB7F,"High Surrogates"},
{0xDB80,0xDBFF,"High Private Use Surrogates"},
{0xDC00,0xDFFF,"Low Surrogates"},
{0xE000,0xE07F,"Private Use Area - Conlang Tengwar"},
{0xE080,0xE0FF,"Private Use Area - Conlang brie.Cirth"},
{0xE2E0,0xE2FF,"Private Use Area - Conlang Xaini"},
{0xE300,0xE33F,"Private Use Area - Conlang Mizarian"},
{0xE340,0xE35F,"Private Use Area - Conlang Ziri:nka"},
{0xE360,0xE37F,"Private Use Area - Conlang Sarkai"},
{0xE380,0xE3AF,"Private Use Area - Conlang Thelwik"},
{0xE3B0,0xE3FF,"Private Use Area - Conlang Olaetyan"},
{0xE400,0xE42F,"Private Use Area - Conlang Niskloz"},
{0xE430,0xE44F,"Private Use Area - Conlang Kazat ?akkorou"},
{0xE450,0xE46F,"Private Use Area - Conlang Kazvarad"},
{0xE470,0xE48F,"Private Use Area - Conlang Zarkhand"},
{0xE490,0xE4BF,"Private Use Area - Conlang Rozhxh"},
{0xE4C0,0xE4EF,"Private Use Area - Conlang Serivelna"},
{0xE4F0,0xE4FF,"Private Use Area - Conlang Kelwathi"},
{0xE500,0xE51F,"Private Use Area - Conlang Saklor"},
{0xE520,0xE54F,"Private Use Area - Conlang Rynnan"},
{0xE550,0xE57F,"Private Use Area - Conlang Alzetjan"},
{0xE580,0xE59F,"Private Use Area - Conlang Telarasso"},
{0xE5A0,0xE5BF,"Private Use Area - Conlang Ssuraki"},
{0xE5C0,0xE5DF,"Private Use Area - Conlang Gargoyle"},
{0xE5E0,0xE5FF,"Private Use Area - Conlang Ophidian"},
{0xE600,0xE62F,"Private Use Area - Conlang Ferengi"},
{0xE630,0xE64F,"Private Use Area - Conlang Seussian Latin Extensions"},
{0xE650,0xE68F,"Private Use Area"},
{0xE690,0xE6CF,"Private Use Area - Conlang Ewellic"},
{0xE6D0,0xE6FF,"Private Use Area - Conlang Phaistos Disc"},
{0xE740,0xE76F,"Private Use Area - Conlang Unifon"},
{0xE770,0xE77F,"Private Use Area - Conlang Solresol"},
{0xE780,0xE7FF,"Private Use Area - Conlang Visible Speech"},
{0xE800,0xE82F,"Private Use Area - Conlang Monofon"},
{0xE830,0xEDFF,"Private Use Area"},
{0xEE00,0xEEAF,"Private Use Area - Hungarian Runes"},
{0xEEB0,0xF89F,"Private Use Area"},
{0xF8A0,0xF8CF,"Private Use Area - Conlang Aiha"},
{0xF8D0,0xF8FF,"Private Use Area - Conlang Klingon"},
{0xF900,0xFAFF,"CJK Compatibility Ideographs"},
{0xFB00,0xFB4F,"Alphabetic Presentation Forms"},
{0xFB50,0xFDFF,"Arabic Presentation Forms-A"},
{0xFE00,0xFE0F,"Variation Selectors"},
{0xFE10,0xFE1F,"Vertical Forms"},
{0xFE20,0xFE2F,"Combining Half Marks"},
{0xFE30,0xFE4F,"CJK Compatibility Forms"},
{0xFE50,0xFE6F,"Small Form Variants"},
{0xFE70,0xFEFF,"Arabic Presentation Forms-B"},
{0xFF00,0xFFEF,"Halfwidth and Fullwidth Forms"},
{0xFFF0,0xFFFF,"Specials"},
{0x10000,0x1007F,"Linear B Syllabary"},
{0x10080,0x100FF,"Linear B Ideograms"},
{0x10100,0x1013F,"Aegean Numbers"},
{0x10140,0x1018F,"Ancient Greek Numbers"},
{0x10190,0x102FF,"Undefined"},
{0x10300,0x1032F,"Old Italic"},
{0x10330,0x1034F,"Gothic"},
{0x10380,0x1039F,"Ugaritic"},
{0x103A0,0x103DF,"Old Persian"},
{0x10400,0x1044F,"Deseret"},
{0x10450,0x1047F,"Shavian"},
{0x10480,0x104AF,"Osmanya"},
{0x10800,0x1083F,"Cypriot Syllabary"},
{0x10840,0x108FF,"Undefined"},
{0x10900,0x1091F,"Phoenician"},
{0x10A00,0x10A5F,"Kharoshthi"},
{0x10A60,0x11FFF,"Undefined"},
{0x12000,0x123FF,"Cuneiform"},
{0x12400,0x1247F,"Cuneiform Numbers and Punctuation"},
{0x12480,0x1CFFF,"Undefined"},
{0x1D000,0x1D0FF,"Byzantine Musical Symbols"},
{0x1D100,0x1D1FF,"Musical Symbols"},
{0x1D200,0x1D24F,"Ancient Greek Musical Notation"},
{0x1D250,0x1D2FF,"Undefined"},
{0x1D300,0x1D35F,"Tai Xuan Jing Symbols"},
{0x1D360,0x1D37F,"Counting Rod Numerals"},
{0x1D380,0x1D3FF,"Undefined"},
{0x1D400,0x1D7FF,"Mathematical Alphanumeric Symbols"},
{0x1D800,0x1FFFF,"Undefined"},
{0x20000,0x2A6DF,"CJK Unified Ideographs Extension B"},
{0x2A6E0,0x2F7FF,"Undefined"},
{0x2F800,0x2FA1F,"CJK Compatibility Ideographs Supplement"},
{0x2FAB0,0xDFFFF,"Unused"},
{0xE0000,0xE007F,"Tags"},
{0xE0080,0xE00FF,"Unused"},
{0xE0100,0xE01EF,"Variation Selectors Supplement"},
{0xE01F0,0xEFFFF,"Unused"},
{0xF0000,0xF0E69,"Supplementary Private Use Area-A Conlang Kinya Syllables"},
{0xF0E70,0xF16AF,"Supplementary Private Use Area-A Conlang Pikto"},
{0xF16B0,0xFFFFF,"Supplementary Private Use Area-A"},
{0x100000,0x10FFFF,"Supplementary Private Use Area-B"},
};

int Ranges_Defined = sizeof(Range_Table)/sizeof(struct cr);
static int Maximum_Range_Name_Length;


int Get_Maximum_Range_Name_Length (int *Flags) {
  int i;
  int len;
  int Maximum_Range_Name_Length = 1;

  for (i = 0; i < Ranges_Defined; i++) {
    if (Flags && Flags[i]) {
      len = strlen(Range_Table[i].name);
      if (len > Maximum_Range_Name_Length) Maximum_Range_Name_Length = len;
    }
  }
  return Maximum_Range_Name_Length;
}

int GetRange(UTF32 ch)
{
  int l;			/* Lower bound of region */
  int u;			/* Upper bound of region */
  int m;			/* Midpoint of region */
  
  l = 0;
  u = Ranges_Defined -1;

  /* Standard binary search */
  while(l <= u){
    m = (l + u) / 2;
    if(ch < Range_Table[m].b) u = m - 1;
    else if(ch > Range_Table[m].e)  l = m + 1;
    else return(m);
  }
  /* NOT REACHED */
}

void ListSelectedRanges(FILE *fp, int *Flags) {
  int i;

  Maximum_Range_Name_Length = Get_Maximum_Range_Name_Length(Flags);
  for (i = 0; i < Ranges_Defined; i++) {
    if(Flags[i]) fprintf(fp,"\t%-*s [0x%04X,0x%04X]\n",
			 Maximum_Range_Name_Length,
			 Range_Table[i].name,
			 Range_Table[i].b,
			 Range_Table[i].e);
  }
}

void ListRanges (FILE *fp, short AlphabeticalP) {
  int Index;
  int *Indices;
  int gap;
  int i;
  int j;
  int len;
  int temp;
  short AUnusedP;
  short BUnusedP;
  short AUndefinedP;
  short BUndefinedP;
  char *aptr;
  char *bptr;

  Indices = (int *) malloc ((size_t) Ranges_Defined * sizeof(int));
  if(Indices == NULL) {
    fprintf(stderr,"ListRanges: storage allocation error.\n");
    return;
  }

  for (i = 0; i < Ranges_Defined; i++) {
    Indices[i] = i;
  }

  /* Shell sort with special comparisons to handle unused and undefined ranges */
  if(AlphabeticalP) {
    for(gap = Ranges_Defined/2; gap > 0; gap /= 2){
      for(i = gap; i < Ranges_Defined; i++){
	for(j = i-gap; j>= 0; j -= gap){
	  aptr = Range_Table[Indices[j]].name;
	  bptr = Range_Table[Indices[j+gap]].name;
	  AUnusedP = BUnusedP = AUndefinedP = BUndefinedP = 0;

	  if(strcmp(aptr,"Unused") == 0) AUnusedP = 1;
	  if(strcmp(bptr,"Unused") == 0) BUnusedP = 1;
	  if(strcmp(aptr,"Undefined") == 0) AUndefinedP = 1;
	  if(strcmp(bptr,"Undefined") == 0) BUndefinedP = 1;

	  if(AUndefinedP) {
	    if(BUndefinedP) {
	      if (Range_Table[Indices[j]].b > Range_Table[Indices[j+gap]].b) {
		temp = Indices[j];Indices[j] = Indices[j+gap];Indices[j+gap] = temp;
	      }
	    }
	    else {temp = Indices[j];Indices[j] = Indices[j+gap];Indices[j+gap] = temp;}
	  }
	  else if(AUnusedP) {
	    if(!BUndefinedP) {
	      if(BUnusedP) {
		if (Range_Table[Indices[j]].b > Range_Table[Indices[j+gap]].b) {
		  temp = Indices[j];Indices[j] = Indices[j+gap];Indices[j+gap] = temp;
		}
	      }
	      else {temp = Indices[j];Indices[j] = Indices[j+gap];Indices[j+gap] = temp;}
	    }
	  }
	  else if(BUnusedP||BUndefinedP) {
	      continue;
	  }
	  else if(strcmp(aptr,bptr) <= 0) break;
	  else {
	    temp = Indices[j];Indices[j] = Indices[j+gap];Indices[j+gap] = temp;
	  }
	}
      }
    }
  }

  if (AlphabeticalP) {		/* Get maximum length of range name for use in formatting */
    Maximum_Range_Name_Length = Get_Maximum_Range_Name_Length(NULL);
  }

  for (i = 0; i < Ranges_Defined; i++) {
    Index = Indices[i];
    if (AlphabeticalP) {
      fprintf(fp,"%-*s  [%06lX-%06lX]\n",
	      Maximum_Range_Name_Length,
	      Range_Table[Index].name,
	      Range_Table[Index].b,
	      Range_Table[Index].e);
    }
    else {
      fprintf(fp,"[%06lX-%06lX]  %s\n",
	      Range_Table[Index].b,
	      Range_Table[Index].e,
	      Range_Table[Index].name);
    }
  }
}

