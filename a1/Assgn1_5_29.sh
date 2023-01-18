#!/bin/bash
# find $1 -name *.py | grep -n -e \# -e \"\"\"
# grep -n -e \# -e \"\"\" nmt-master/nmt/attention_model.py
# awk '/#|""".*"""/ {print NR, $0}' nmt-master/nmt/attention_model.py
grep -nRE --include \*.py -e \# -e '""".' $1 | grep -vE '".*#.*"'
# grep -nR --include \*.py -vE -e '".*#.*"' -e '^(\s)*"""(\s)*$' $1 | grep -e \# -e '"""'