#!/bin/bash
mkdir -p $2
for ch in {a..z} ; do
 for fname in $1/*.txt ; do
  echo $fname
 done
done


