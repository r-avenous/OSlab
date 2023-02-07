#!/bin/bash
if !(test -f "main.csv");
then
    echo "date,category,amount,name">main.csv
fi
args=($(echo "${@: -4}" | awk '{print $1,$2,$3,$4}'))

date=${args[0]}
category=${args[1]}
amount=${args[2]}
name=${args[3]}

echo "$date,$category,$amount,$name">>main.csv

tail -n +2 main.csv | sort -n -t ',' -k1.7 -k1.4,1.5 -k1.1,1.2 | sed '1 i\date,category,amount,name' | tee main.csv > /dev/null

while getopts "c:n:s:h" opt
do
    case $opt in 
        c) awk -v var="$OPTARG" 'BEGIN{ FS = "," ; sum=0 } NR!=1 && $2 == var {sum += $3} END{print sum}' main.csv;;
        n) awk -v var="$OPTARG" 'BEGIN{ FS = "," ; sum=0 } NR!=1 && $4 == var {sum += $3} END{print sum}' main.csv;;
        s) case $OPTARG in
                date) tail -n +2 main.csv | sort -n -t ',' -k1.7 -k1.4,1.5 -k1.1,1.2 | sed '1 i\date,category,amount,name' | tee main.csv > /dev/null;;
                name) tail -n +2 main.csv | sort -t ',' -k 4 | sed '1 i\date,category,amount,name' | tee main.csv > /dev/null;;
                category) tail -n +2 main.csv | sort -t ',' -k 2 | sed '1 i\date,category,amount,name' | tee main.csv > /dev/null;;
                amount) tail -n +2 main.csv | sort -t ',' -k 3 -n | sed '1 i\date,category,amount,name' | tee main.csv > /dev/null;;
            esac;;
        h) printf "\nName : Splitwise \n\nUsage : Pass -c for category and -n for name based calculations, -s for sorting by column and -h for displaying help prompt.\n\n";;
    esac
done