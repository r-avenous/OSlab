#!/bin/bash
# find $1 -name *.py | grep -n -e \# -e \"\"\"
# grep -n -e \# -e \"\"\" nmt-master/nmt/attention_model.py
# awk '/#|""".*"""/ {print NR, $0}' nmt-master/nmt/attention_model.py
# grep -nRE --include \*.py -e \# -e '""".' $1 | grep -vE '".*#.*"'1
# grep -nR --include \*.py -vE -e '".*#.*"' -e '^(\s)*"""(\s)*$' $1 | grep -e \# -e '"""'

# find $1 -name '*.py' -type f -exec awk 'BEGIN{m=0} 
# {if(m==1) {print $0; if($0 ~ /"""/) m=0; next}}
# /^[^"]*#[^"]*$|""".*"""/ {print NR, $0; next} 
# /"""/ {m=1; print NR, $0}' {} + > output.txt

find $1 -name '*.py' -type f -print0 | xargs -0 -I {} sh -c 'echo {}; 
awk '\''BEGIN{m=0} 
/""".*"""/ {print NR, $0; next}
{if(m==1) {print $0; if($0 ~ /"""/) m=0; next}} 
{if (match($0, /^([^#]*)#/, arr)) {
    count = gsub(/"/, "", arr[1]);
    {if (count % 2 == 0) print NR, $0; next}
  }}
/"""/ {m=1; print NR, $0}'\'' {}'