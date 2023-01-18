#!/bin/bash
mkdir -p $2
for ch in {a..z};do
 touch $2/${ch}.txt
 grep -r -h -i "^$ch" $1>$2/${ch}.txt
 sort -o $2/${ch}.txt $2/${ch}.txt
done


