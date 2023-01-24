#!/bin/bash
mkdir -p $2
for ch in {a..z};do
    grep -rhi "^$ch" $1>$2/${ch}.txt
    sort -o $2/${ch}.txt $2/${ch}.txt
done
