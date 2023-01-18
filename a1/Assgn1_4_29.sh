#!/bin/bash

file=$1
word=$2

while read line; do
  if echo "$line" | grep -q "$word"; then
    echo "$line" | awk '{gsub(/[[:alpha:]]/,"&\n")}1' | awk '{if (NR % 2 == 0) print tolower($0); else print toupper($0);}' | tr -d '\n'
  else
    echo "$line"
  fi
done < "$file"
