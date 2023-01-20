BEGIN{
	FS = " "
	i=0
	printf("Valid usernames are denoted by 'YES' and unvalid usernames are denoted by 'NO' in respective lines\n")> "validation.txt"
}

NR == FNR { invalid[i] = tolower($0); i++; next}
{ if(length($0)<5 || length($0)>20) {out = 1;printf("hi1 %s\n",$0);}}
!/[a-zA-Z0-9]*/ { out = 1;printf("hi2 %s\n",$0);}
!/[a-zA-Z].*[0-9].*/ {out =1;printf("hi3 %s\n",$0);}
# !/[a-zA-Z].*/ {out =1;printf("hi4\n");}
{for (j=0;j<i;j++){ if(tolower($0) ~ invalid[j]) {out =1;printf("hi5 %s\n",$0)}}}
{if((out+0)==1) {printf("NO\n")>> "validation.txt"; out=0} else{ printf("YES\n")>> "validation.txt"}}