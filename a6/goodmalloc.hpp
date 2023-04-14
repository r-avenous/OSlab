#ifndef __GOODMALLOC_HPP__
#define __GOODMALLOC_HPP__

#include <vector>
#include <cstdlib>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <stack>
#include <unordered_set>

using namespace std;

#define ptr unsigned int
#define PAGESIZE 128                       // 128 elements per page
#define MEM_SIZE 250 * (1 << 20)        // 250 MB memory size
#define MAXPAGES ((MEM_SIZE/sizeof(element))/PAGESIZE) // 1953125 pages
#define ERROR -1
#define SUCCESS 0

void createMem();

class element       // structure of each element in the list
{
    public:
        ptr _next, _prev;    // pointers to next and previous elements
        int _data;         // data stored in the element
        element(int data);  // constructor
        int get_data();     // getter for data
        void set_data(int data);    // setter for data
};

class list      // structure of the linked list
{
    public:
        ptr head;       // pointer to the head of the list
        int size;       // size of the list
        vector<ptr> occupiedPages;      // vector of pages occupied by the list

        list()      // default constructor
        {
            this->head = 0;
            this->size = 0;
        }
        list(ptr head, int size, vector<ptr> occ_pages);        // constructor
        list& operator=(const list &l);     // assignment operator
        int getSize();      // getter for size
        int getElem(int index);     // getter for element at index
        void setElem(int index, int val);       // setter for element at index
};

int createList(string lname, int size);     // creates a list of size 'size'
int assignVal(string lname, int index, int val);    // assigns value 'val' to element at index 'index' in list 'lname'
int freeElem(string lname);    // frees the list 'lname'
void freeElem();

int getVal(string lname, int index, int &val);   // gets the value of element at index 'index' in list 'lname' and stores it in 'val'
int printPages(string lname);   // prints the pages occupied by list 'lname'
void push_frame();    // pushes a new frame on the stack
void pop_frame();       // pops the top frame from the stack

void printKeys();       // prints the names of the lists in the page table
void printStackKeys();    // prints the names of the lists in the stack
void print_list(string lname);    // prints the list 'lname'

class frame     // structure of a frame
{
    public:
        int id;     // id of the frame
        unordered_set<string> localListNames;       // set of names of the lists declared in the frame
        frame();    // default constructor
};

#endif












// //-----------------WITHOUT PAGE STORAGE-----------------
// #ifndef __GOODMALLOC_HPP__
// #define __GOODMALLOC_HPP__

// #include <vector>
// #include <cstdlib>
// #include <queue>
// #include <unordered_map>
// #include <iostream>
// #include <stack>
// #include <unordered_set>

// using namespace std;

// #define ptr unsigned int
// #define PAGESIZE 128
// #define MEM_SIZE 250 * (1 << 20)
// #define MAXPAGES ((MEM_SIZE/sizeof(element))/PAGESIZE)
// #define ERROR -1
// #define SUCCESS 0

// void createMem();

// class element
// {
//     public:
//         ptr _next, _prev;
//         int _data;
//         element(int data);
//         int get_data();
//         void set_data(int data);
// };

// class list
// {
//     public:
//         ptr head;
//         int size;
//         // vector<ptr> occupiedPages;

//         list()
//         {
//             this->head = 0;
//             this->size = 0;
//         }
//         list(ptr head, int size);
//         list& operator=(const list &l);
//         int getSize();
//         int getElem(int index);
//         void setElem(int index, int val);
// };

// int createList(string lname, int size);
// int assignVal(string lname, int index, int val);
// int freeElem(string lname);

// int getVal(string lname, int index, int &val);
// int printPages(string lname);
// void push_frame();
// void pop_frame();

// void printKeys();
// void printStackKeys();
// void print_list(string lname);

// class frame
// {
//     public:
//         int id;
//         unordered_set<string> localListNames;
//         frame();
// };

// #endif
