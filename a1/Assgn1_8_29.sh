#!/bin/bash
if !(test -f "main.csv");
then
    echo "Date (dd-mm-yy), Category, Amount, Name">main.csv
fi
# default_args=($(echo "${@: -4}" | awk '{printf("%s,%s,%s,%s\n",$1,$2,$3,$4)}' >> main.csv))
default_args=($(echo "${@: -4}" | awk '{print $1,$2,$3,$4}'))

date=${default_args[0]}
category=${default_args[1]}
amount=${default_args[2]}
name=${default_args[3]}

echo "$date,$category,$amount,$name">>main.csv

while getopts "c:n:s:h" opt
do
    case $opt in 
        c) echo "Option c: $opt, argument: $OPTARG";;
        n) awk 'BEGIN{ FS = "," ; sum=0} $4 == $OPTARG {sum += $3} END{print sum}' main.csv;;
        s) sort -t ',' -k 2 main.csv;;
        h) echo "Option h: $opt, argument: $OPTARG";;
    esac
done