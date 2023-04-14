#include <stdio.h>
#include "goodmalloc.hpp"
#include <time.h>

#define LIST_SIZE 50000
#define MAX_VAL 2 * LIST_SIZE
int RUNS = 100;
int maxUsage = -1;

void mergeSort(string lname, int start, int size)
{
    maxUsage = max(maxUsage, getNumUsedPages());
    if(size == 1) return;

    push_frame();
    int mid = size/2;

    mergeSort(lname, start, mid);

    mergeSort(lname, (start+mid), size-mid);
    
    int k=(start+mid),j=start,i;
    int t1, t2;
    string lname1 = "lname1";
    createList(lname1, size);
    for(i=0;i<size;i++)
    {
        getVal(lname,j,t1);
        getVal(lname,k,t2);
        if(t1>t2){
            assignVal(lname1, i, t2);
            k++;
            if(k>=(start + size)) break;
            continue;
        }
        assignVal(lname1, i, t1);
        j++;
        if(j>=(start + mid)) break;
    }
    while(k<(start + size)){
        getVal(lname,k,t2);
        i++;
        assignVal(lname1, i, t2);
        k++;
    }
    while(j<start + mid){
        getVal(lname,j,t1);
        i++;
        assignVal(lname1, i, t1);
        j++;
    }
    int v;
    for(i=start;i<(start+size);i++){
        getVal(lname1, (i-start), v);
        assignVal(lname, i, v);
    }
    pop_frame();
}

int main()
{
    createMem();
    int x;

    double avg_time = 0;
    int rr = RUNS;
    while(rr--)
    {
        string lname = "list1";
        createList(lname, LIST_SIZE);

        for(int i=0; i<LIST_SIZE; i++)
        {
            srand(time(0)+i);
            x = rand()%(MAX_VAL) + 1;
            assignVal(lname, i, x);
        }

        cout << "\nUnsorted list: " << '\n';
        print_list(lname);
        cout << "\n\n";

        // start timer
        clock_t start = clock();
        mergeSort(lname, 0, LIST_SIZE);
        // end timer
        clock_t end = clock();

        cout << "\n\n\n\n\nSorted list: " << '\n';
        print_list(lname);
        cout << "\n\n";

        avg_time += (double)(end-start)/CLOCKS_PER_SEC;
        clearAll();
    }

    cout << "\n\n\n\n\n\n\nAvg Time taken: " << avg_time/RUNS << "s" << '\n';
    cout << "Max page usage: " << maxUsage << " (" << (maxUsage * 100.0) / MAXPAGES << "%)\n";

    return 0;
}


