#!/bin/bash

find $1 -name '*.py' -type f -print0 | xargs -0 -I {} sh -c 'echo {}; 
awk '\''BEGIN{m=0} 
/""".*"""/ {print NR, $0; next}
{if(m==1) {print $0; if($0 ~ /"""/) m=0; next}} 
{if (match($0, /^([^#]*)#/, arr)) {
    count = gsub(/"/, "", arr[1]);
    {if (count % 2 == 0) print NR, $0; next}
  }}
/"""/ {m=1; print NR, $0}'\'' {}'