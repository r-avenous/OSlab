#!/bin/bash
# find $1 -name *.py | grep -n -e \# -e \"\"\"
# grep -n -e \# -e \"\"\" nmt-master/nmt/attention_model.py
# awk '/#|""".*"""/ {print NR, $0}' nmt-master/nmt/attention_model.py
grep -nR --include \*.py -e \# -e \"\"\" $1