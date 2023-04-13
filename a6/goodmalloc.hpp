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
#define PAGESIZE 128
#define MEM_SIZE 250 * (1 << 20)
#define MAXPAGES ((MEM_SIZE/sizeof(element))/PAGESIZE)
#define ERROR -1
#define SUCCESS 0

void createMem();

class element
{
    public:
        ptr _next, _prev;
        int _data;
        element(int data);
        int get_data();
        void set_data(int data);
};

class list
{
    public:
        ptr head;
        int size;
        vector<ptr> occupiedPages;

        list()
        {
            this->head = 0;
            this->size = 0;
        }
        list(ptr head, int size, vector<ptr> occ_pages);
        list& operator=(const list &l);
        int getSize();
        int getElem(int index);
        void setElem(int index, int val);
};

int createList(string lname, int size);
int assignVal(string lname, int index, int val);
int freeElem(string lname);

int getVal(string lname, int index, int &val);
int printPages(string lname);
void push_frame();
void pop_frame();

void printKeys();
void printStackKeys();
void print_list(string lname);

class frame
{
    public:
        int id;
        unordered_set<string> localListNames;
        frame();
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
