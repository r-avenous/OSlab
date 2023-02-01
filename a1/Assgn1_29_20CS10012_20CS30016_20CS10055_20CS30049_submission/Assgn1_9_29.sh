#!/bin/bash

awk '
{ depfreq[$2]+=1 }
END {
    for (dep in depfreq) {
        print dep, depfreq[dep]
    }
}' $1 | sort -t$' ' -k2rn -k1
awk '
{ studentfreq[$1]+=1 }
END {
    for (student in studentfreq) {
        if (studentfreq[student]>=2) {
            print student
        }
        else {
            count+=1
        }
    }
    print count
}' $1

