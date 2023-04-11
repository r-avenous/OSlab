#include <stdio.h>
#include "goodmalloc.hpp"

int lcount;
void mergeSort(string lname, int size){

    if(size == 1) return;

    int mid = size/2;
    string lname1 = lname + to_string(lcount++);
    string lname2 = lname + to_string(lcount++);

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
}

int main()
{
    createMem();
    int x;
    for(int i=0; i<10; i++)
    {
        createList("list" + to_string(i), 2);
    }
    for(int i=0; i<10; i+=2)
    {
        freeElem("list" + to_string(i));
    }
    createList("list11", 17);

    for(int i=0; i<17; i++) assignVal("list11", i, i);

    extern unordered_map<string, list> page_table;
    extern void* mem;

    ptr h = page_table["list11"].head;
    for(int i=0; i<17; i++)
    {
        cout << ((element*)mem + h)->_data << ' ';
        h = ((element*)mem + h)->_next;
    }

    int size = 5;
    lcount = 12;
    string lname = "list" + lcount;
    createList(lname, size);

    assignVal(lname, 0, 4);
    assignVal(lname, 1, 1);
    assignVal(lname, 2, 2);
    assignVal(lname, 3, 5);
    assignVal(lname, 4, 3);

    // mergesort
    mergeSort(lname, size);

    return 0;
}