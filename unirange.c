#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unicode.h"
#include "unirange.h"

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

