#This script extracts a table of Unicode character names from the
#UnicodeData file. It also generates the ancillary offset table.

BEGIN{
  FS=";";
  printf("char * unames[]={\n");
  printf("#include \"unicode.h\"\n#include \"offsettbl.h\"\nstruct oft Offset_Table[]={\n") > "offsettbl.c";
  prev=-1;
  cur=-1;
  Offset = 0;
  PreviousLineType = 0;
}
{
  hone = "0x" $1; cur = strtonum(hone);

#  if(cur > 0x2FA1D) nextfile; # That is, stop (after executing the END action).

#Deal with undefined ranges.

  if(cur > prev+1){
    if(PreviousLineType == 1){ #We just ended a defined range.
      printf("0x%X,0x%X,%d,\n",DefinedRangeStart,prev,Offset) > "offsettbl.c";
    }
    printf("0x%X,0x%X,OFT_UNDEF,\n",prev+1,cur-1) > "offsettbl.c";
    Offset += (cur - prev -1);
    PreviousLineType = 0;
  }
 
#Deal with specified ranges  

  if(match($2,/First>$/)){
    RangeStart=cur;
    getline;
    if(!match($2,/Last>$/)){
      printf(stderr,"Error: first and last range specs not adjacent at line %d.\n",NR) > "/dev/stderr";
      exit;
    }
    hone = "0x" $1; cur = strtonum(hone);
    RangeEnd = cur;
    if(PreviousLineType == 1){ #We just ended a defined range.
      printf("0x%X,0x%X,%d,\n",DefinedRangeStart,RangeStart-1,Offset) > "offsettbl.c";
    }
    RangeName = substr($2,2,RSTART-4);
    Offset += (RangeEnd - RangeStart+1);
    printf("0x%X,0x%X,OFT_NOINF,\n",RangeStart,RangeEnd) > "offsettbl.c";
    PreviousLineType = 0;
    prev=cur;
    next;
  }

#The Latin-1 control characters all have the official Unicode name "control", so we
#use the real name in the comment field for them when it exists.
  if(PreviousLineType == 0) DefinedRangeStart = cur;
  if(match($2,"<control>")){
    if(length($11) > 0) Name = $11;
    else Name = "unspecified control character";
  }
  else Name = $2;
  printf("\"%s\",\t/* 0x%04X */\n",Name,cur);
  PreviousLineType = 1;
  prev=cur;
}
END{
  if(PreviousLineType == 1){ #We just ended a defined range.
    printf("0x%X,0x%X,%d,\n",DefinedRangeStart,prev,Offset) > "offsettbl.c";
  }
  printf("};\n");
  printf("};\nint Offset_Entries = sizeof(Offset_Table)/sizeof(struct oft);\n") > "offsettbl.c";
}

