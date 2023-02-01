#!/bin/bash
array=( $@ )
counter=1
for n in ${array[@]}; 
do
    if [ "$counter" -ne 1 -a "$counter" -ne 2 ]
    then
        var=.$n
        if [ "$counter" -ne $# ]
        then
            var=$var,
        fi
        k+=$var
    fi
    counter=$((counter+1))
done

for file in "$1"/*;
do
    basename=${file##*/}
    filename=${basename%.*}
    rm -f "$2/${filename}.csv"
    jq -r "[$k] | @csv" "$file" >>"$2/$filename"".csv"
done

