#!/bin/bash
awk 'BEGIN{
	FS = " "
	i=0
}

NR == FNR { invalid[i] = tolower($0); i++; next}
{ if(length($0)<5 || length($0)>20) out = 1}
!/[a-zA-Z0-9]*/ { out = 1}
!/[a-zA-Z].*[0-9].*/ {out =1}
# !/[a-zA-Z].*/ {out =1}
{for (j=0;j<i;j++){ if(tolower($0) ~ invalid[j]) out =1}}
{if((out+0)==1) {printf("NO\n")> "validation.txt"; out=0} else{ printf("YES\n")> "validation.txt"}}' fruits.txt $1