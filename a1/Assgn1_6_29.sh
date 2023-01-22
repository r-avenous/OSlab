#!/bin/bash
prime=()
for (( i=2; i<=1000000; i++ ))
do
    isPrime[$i]=true
done
for (( p=2; p*p<=1000000; p++ ))
do
    if ${isPrime[$p]}
    then
        for (( j=p*p; j<=1000000; j+=p ))
        do
            isPrime[$j]=false
        done
    fi
done
for (( p=2; p<=1000000; p++ ))
do
    if ${isPrime[$p]}
    then
        prime+=( $p )
    fi
done
> output.txt
while read -r n
do
    pr=()
    for p in ${prime[@]}
    do
        if [ $p -gt $n ]
        then
            break
        fi
        if [[ $((n%p)) -eq 0 ]]
        then
            pr+=( $p )
            while [[ $((n%p)) -eq 0 ]]
            do
                n=$((n/p))
            done
        fi
    done
    echo ${pr[@]} >> output.txt
done < input.txt