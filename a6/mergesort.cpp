#include <stdio.h>
#include "goodmalloc.hpp"

// int lcount = 12;



void mergeSort(string lname, int size)
{
    if(size == 1) return;

    push_frame();
    int mid = size/2;
    string lname1 = "lname1";
    string lname2 = "lname2";
    // string lname3 = lname + to_string(lcount++);

    createList(lname1, mid);
    createList(lname2, size-mid);

    int temp;

    // copy values to smaller lists
    for (int i=0; i<mid; i++)
    {
        getVal(lname, i, temp);
        assignVal(lname1, i, temp);
    }

    for (int i=mid; i<size; i++)
    {
        getVal(lname, i, temp);
        assignVal(lname2, i-mid, temp);

    }

    // sort smaller lists
    mergeSort(lname1, mid);

    mergeSort(lname2, size-mid);
    // createList(lname3, size);
    int k=0,j=0,i;
    int t1, t2;
    for(i=0;i<size;i++)
    {
        getVal(lname1,j,t1);
        getVal(lname2,k,t2);
        if(t1>t2){
            assignVal(lname, i, t2);
            k++;
            if(k>=(size-mid)) break;
            continue;
        }
        assignVal(lname, i, t1);
        j++;
        if(j>=(mid)) break;
    }
    while(k<(size-mid)){
        getVal(lname2,k,t2);
        i++;
        assignVal(lname, i, t2);
        k++;
    }
    while(j<mid){
        getVal(lname1,j,t1);
        i++;
        assignVal(lname, i, t1);
        j++;
    }
    pop_frame();
}

int main()
{
    createMem();
    int x;
    // for(int i=0; i<10; i++)
    // {
    //     createList("list" + to_string(i), 2);
    // }
    // for(int i=0; i<10; i+=2)
    // {
    //     freeElem("list" + to_string(i));
    // }
    // createList("list11", 17);

    // for(int i=0; i<17; i++) assignVal("list11", i, i);

    // extern unordered_map<string, list> page_table;
    // extern void* mem;

    // printKeys();

    // ptr h = page_table["list11__GLOBAL"].head;
    // for(int i=0; i<17; i++)
    // {
    //     cout << ((element*)mem + h)->_data << ' ';
    //     h = ((element*)mem + h)->_next;
    // }

    int size = 5;
    // lcount = 12;
    string lname = "list12";
    cout << "\nmerge wali list " << createList(lname, size) << '\n';

    assignVal(lname, 0, 4);
    assignVal(lname, 1, 1);
    assignVal(lname, 2, 2);
    assignVal(lname, 3, 5);
    assignVal(lname, 4, 3);

    print_list(lname);

    printKeys();

    // mergesort
    mergeSort(lname, size);
    cout << '\n';
    for(int i=0; i<size; i++)
    {
        getVal(lname, i, x);
        cout << x << ' ';
    }
    cout << '\n';

    return 0;
}


