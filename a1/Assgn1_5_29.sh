#!/bin/bash
# find $1 -name *.py | grep -n \#
# grep -n -e \# -e \"\"\" nmt-master/nmt/attention_model.py
# awk '/#|""".*"""/ {print NR, $0}' nmt-master/nmt/attention_model.py
grep -nR -e \# -e \"\"\" $1