#!/bin/bash
array=( $@ )
counter=1
input=${array[0]}
for n in ${array[@]}; 
do
    if [ "$counter" -ne 1 ]
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

echo $k
for file in "$input"/*;
do
    jq -n '[inputs]' <$file >$(basename "$file").json
    jq -r --arg k $k '.[] | [ $k,"," ] | @csv' $(basename "$file").json > $(basename "$file").csv
    rm $(basename "$file").json
done

