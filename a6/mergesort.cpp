#include <stdio.h>
#include "goodmalloc.hpp"
#include <time.h>

// int lcount = 12;



void mergeSort(string lname, int start, int size)
{
    // cout << start << ' ' << start + size - 1 << '\n';
    if(size == 1) return;

    push_frame();
    int mid = size/2;
    // string lname1 = "lname1";
    // string lname2 = "lname2";
    // string lname3 = lname + to_string(lcount++);

    // createList(lname1, mid);
    // createList(lname2, size-mid);

    // cout << "Keys\n";
    // printKeys();
    // cout << "\n\n";

    // int temp;

    // copy values to smaller lists
    // for (int i=0; i<mid; i++)
    // {
    //     getVal(lname, i, temp);
    //     assignVal(lname1, i, temp);
    // }

    // for (int i=mid; i<size; i++)
    // {
    //     getVal(lname, i, temp);
    //     assignVal(lname2, i-mid, temp);

    // }

    // sort smaller lists
    // cout << "list1 before" ;
    // print_list(lname1);
    // cout << '\n';
    mergeSort(lname, start, mid);
    // cout << "list1 after" ;
    // print_list(lname1);
    // cout << '\n';
    // cout << "list2 before";
    // print_list(lname2);
    // cout << '\n';
    mergeSort(lname, (start+mid), size-mid);
    // cout << "list2 after";
    // print_list(lname2);
    // cout << '\n';
    // createList(lname3, size);
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
    // freeElem(lname1);
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
    
    int size = 50000;
    // lcount = 12;
    string lname = "list12";
    createList(lname, size);

    for(int i=0; i<size; i++)
    {
        srand(time(0)+i);
        x = rand()%(2 * size) + 1;
        assignVal(lname, i, x);
    }

    // print_list(lname);

    // printKeys();

    // mergesort
    // start timer
    clock_t start = clock();
    mergeSort(lname, 0, size);
    // end timer
    clock_t end = clock();
    // cout << '\n';
    // for(int i=0; i<size; i++)
    // {
    //     getVal(lname, i, x);
    //     cout << x << '\n';
    // }
    cout << "Time taken: " << (double)(end-start)/CLOCKS_PER_SEC << '\n';
    // cout << '\n';
    // printKeys();
    return 0;
}


