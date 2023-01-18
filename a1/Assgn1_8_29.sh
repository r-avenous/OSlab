#!/bin/bash
touch main.csv
echo "Date (dd-mm-yy), Category, Amount, Name">main.csv
default_args=($(echo "${@: -4}" | awk '{print $1,$2,$3,$4}'))

date=${default_args[0]}
category=${default_args[1]}
amount=${default_args[2]}
name=${default_args[3]}

echo "$date, $category, $amount, $name"

while getopts "c:n:s:h" opt
do
    case $opt in 
        c) echo "Option c: $opt, argument: $OPTARG";;
        n) echo "Option n: $opt, argument: $OPTARG";;
        s) echo "Option s: $opt, argument: $OPTARG";;
        h) echo "Option h: $opt, argument: $OPTARG";;
    esac
done