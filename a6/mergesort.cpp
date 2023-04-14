#include <stdio.h>
#include "goodmalloc.hpp"
#include <time.h>

#define LIST_SIZE 50000
#define MAX_VAL 100000

void mergeSort(string lname, int start, int size)
{
    if(size == 1) return;

    push_frame();
    int mid = size/2;

    mergeSort(lname, start, mid);

    mergeSort(lname, (start+mid), size-mid);
    
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
    pop_frame();
}

int main()
{
    createMem();
    int x;

    
    string lname = "list1";
    createList(lname, LIST_SIZE);

    for(int i=0; i<LIST_SIZE; i++)
    {
        srand(time(0)+i);
        x = rand()%(MAX_VAL) + 1;
        assignVal(lname, i, x);
    }

    cout << "Unsorted list: " << '\n';
    print_list(lname);

    // start timer
    clock_t start = clock();
    mergeSort(lname, 0, LIST_SIZE);
    // end timer
    clock_t end = clock();

    cout << "\n\n\n\n\nSorted list: " << '\n';
    print_list(lname);

    cout << "\n\n\n\n\n\n\nTime taken: " << (double)(end-start)/CLOCKS_PER_SEC << "s" << '\n';


    // {
    //     push_frame();
    //     createList("list1", 100);
    //     {
    //         push_frame();
    //         cout << assignVal("list1", 0, 10) << '\n';
    //         createList("list2", 100);
    //         {
    //             push_frame();
    //             cout << getVal("list1", 0, x) << '\n';
    //             createList("list3", 100);
    //             createList("list4", 100);
    //             printKeys();
    //             cout << '\n';
    //             cout << x << '\n';
    //             {
    //                 push_frame();
    //                 createList("list5", 100);
    //                 cout << assignVal("list2", 0, 20) << '\n';
    //                 cout << getVal("list2", 0, x) << '\n';
    //                 pop_frame();
    //             }
    //             freeElem("list1");
    //             printKeys();
    //             cout << '\n';
    //             pop_frame();
    //             printKeys();
    //             cout << '\n';
    //         }
    //         pop_frame();
    //     }
    //     pop_frame();
    // }
    return 0;
}


