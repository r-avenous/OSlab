#include "goodmalloc.hpp"

void* mem;      // stores the starting location of the memory
queue<ptr> freePages;   // stores the starting location of free pages
unordered_map<string, list> page_table;     // stores the list name and the list object
vector<frame> stack_frame;       // stores the frames
int f_counter = 0;      // counter for the frame id

void printKeys()    // prints the names of the lists in the page table
{
    for (auto it = page_table.begin(); it != page_table.end(); it++)
    {
        cout << it->first << endl;
    }
}

void createMem()        // creates the memory
{
    mem = malloc(MEM_SIZE);
    for (long unsigned int i = 0; i < MAXPAGES; i++)
    {
        freePages.push(i *  PAGESIZE);
    }
}


// // alternative createMem() function for speedup
// void createMem()        // creates the memory
// {
//     mem = malloc(MEM_SIZE);
//     for (long unsigned int i = 0; i < MAXPAGES; i++)
//     {
//         freePages.push(i *  PAGESIZE);
//     }
//     int num_elems = MEM_SIZE / sizeof(element);
//     element *elem_ptr = (element*)mem;
//     elem_ptr->_prev = -1;
//     elem_ptr->_next = 1;
//     elem_ptr++;
//     num_elems--;
//     ptr next = 2;
//     while(num_elems--)
//     {
//         elem_ptr->_prev = (next - 2);
//         elem_ptr->_next = next;
//         next++;
//         elem_ptr++;
//     }
//     (--elem_ptr)->_next = -1;
// }


element::element(int data = 0)      // constructor for the element class
{
    this->_data = data;
}

int element::get_data()     // returns the data of the element
{
    return this->_data;
}

void element::set_data(int data)    // sets the data of the element
{
    this->_data = data;
}

list::list(ptr head, int size, vector<ptr> occ_pages)   // constructor for the list class
{
    this->head = head;
    this->size = size;
    this->occupiedPages = occ_pages;
}

list& list::operator=(const list &l)    // assignment operator for the list class
{
    this->head = l.head;
    this->size = l.size;
    this->occupiedPages = l.occupiedPages;
    return *this;
}

int list::getSize()    // returns the size of the list
{
    return this->size;
}

int list::getElem(int index)    // returns the data of the element at index 'index'
{
    int page_num = index / PAGESIZE;    // finds the page number which contains the element
    int offset_within_page = index % PAGESIZE;   // finds the offset of the element within the page
    element *temp;
    temp = (element*)mem + occupiedPages[page_num];
    temp += offset_within_page;
    return temp->get_data();
}

void list::setElem(int index, int val)   // sets the data of the element at index 'index' to 'val'
{
    int page_num = index / PAGESIZE;    // finds the page number which contains the element
    int offset_within_page = index % PAGESIZE;  // finds the offset of the element within the page
    element *temp;
    temp = (element*)mem + occupiedPages[page_num];
    temp += offset_within_page;
    temp->set_data(val);
}

string generateLName(string lname)  // generates the name of the list according to the current frame (scope)
{
    if(stack_frame.empty())     // if the stack is empty, then the list is global
    {
        lname += "__GLOBAL";
    }
    else        // if the stack is not empty, then append the frame id to the list name
    {
        lname += "__";
        lname += to_string(stack_frame.back().id);
    }
    return lname;
}

string findLName(string lname)  // finds the name of the list according to the current frame (scope)
{
    string temp = generateLName(lname);     
    if(page_table.find(temp) != page_table.end())       // if the list is found in the current frame, return the name
    {
        return temp;
    }
    else        // if the list is not found in the current frame, then search in the parent frame
    {
        // auto top = stack_frame.back();
        // stack_frame.pop_back();
        // temp = generateLName(lname);
        // stack_frame.push_back(top);
        // if(page_table.find(temp) != page_table.end())
        // {
        //     return temp;
        // }
        for(int i=stack_frame.size()-2; i>=0; i--)
        {
            temp = lname + "__" + to_string(stack_frame[i].id);
            if(page_table.find(temp) != page_table.end())
            {
                return temp;
            }
        }
    }
    temp = lname + "__GLOBAL";    // if the list is not found in the current frame or the parent frame, then search in the global scope
    if(page_table.find(temp) != page_table.end())
    {
        return temp;
    }
    return "__ERROR";   // if the list is not found in the global scope, then return an error
}

int createList(string _lname, int num_elements)     // creates a list with name 'lname' and size 'num_elements'
{
    string lname = generateLName(_lname);
    int num_pages = (num_elements + PAGESIZE - 1) / PAGESIZE;   // calculates the number of pages required for the list
    if(num_pages > (int)freePages.size())   // if the number of pages required is greater than the number of free pages, then return an error
    {
        return ERROR;
    }
    int temp_num_elems = num_elements;
    element *elem_ptr;
    ptr next, prev=-1;
    for(int i=0; i<num_pages; i++)  // sets the pointers of the elements of the list
    {
        page_table[lname].occupiedPages.push_back(freePages.front());   // adds a free page to the list's occupied pages
        next = freePages.front();
        elem_ptr = (element*)mem + freePages.front();
        for(int j=0; j<PAGESIZE; j++){      // sets the pointers of the elements of the page
            if(temp_num_elems<=0){      // set the next pointer of the last element to -1
                elem_ptr--;
                elem_ptr->_next=-1;
                break;
            }
            temp_num_elems--;
            
            next += 1;
            elem_ptr->_next = next;
            
            if(j==0){   
                elem_ptr->_prev = prev;     // sets the prev pointer of the first element of the page to the last element of the previous page
                if(prev!=(unsigned int)-1){     // if the previous page is not the first page, then set the next pointer of the last element of the previous page to the first element of the current page
                    elem_ptr = (element*)mem + prev;
                    elem_ptr->_next = freePages.front();
                    elem_ptr = (element*)mem + freePages.front();
                }
                prev = freePages.front();   // sets the prev pointer to the first element of the current page
            }
            else{
                elem_ptr->_prev = prev;    // sets the prev pointer of the element to the previous element
                prev += 1;
            }
            ++elem_ptr;
        }
        freePages.pop();    // removes the page from the free pages
    }
    page_table[lname].size = num_elements;      // sets the size of the list
    page_table[lname].head = page_table[lname].occupiedPages[0];        // sets the head of the list
    if(!stack_frame.empty())        // if the stack is not empty, then add the list name to the local list names of the current frame
    {
        auto top = stack_frame.back();
        stack_frame.pop_back();
        top.localListNames.insert(_lname);
        stack_frame.push_back(top);
    }
    return SUCCESS;
}





// // alternative implementation of createList function for SPEEDUP

// int createList(string _lname, int num_elements)     // creates a list with name 'lname' and size 'num_elements'
// {
//     string lname = generateLName(_lname);
//     int num_pages = (num_elements + PAGESIZE - 1) / PAGESIZE;   // calculates the number of pages required for the list
//     if(num_pages > (int)freePages.size())   // if the number of pages required is greater than the number of free pages, then return an error
//     {
//         return ERROR;
//     }
//     element *elem_ptr = (element*)mem + freePages.front();
//     ptr next, prev = freePages.front();
//     for(int i=0; i<num_pages; i++)  // sets the pointers of the elements of the list
//     {
//         page_table[lname].occupiedPages.push_back(freePages.front());
//         if(i!=0){
//             next = freePages.front();
//             prev += (PAGESIZE-1);
//             elem_ptr += (PAGESIZE-1);
//             elem_ptr->_next = next;
//             elem_ptr = (element*)mem + next;
//             elem_ptr->_prev = prev;
//             prev = next;
//         }
//         freePages.pop();
//     }
//     page_table[lname].size = num_elements;
//     page_table[lname].head = page_table[lname].occupiedPages[0];
//     if(!stack_frame.empty())
//     {
//         auto top = stack_frame.top();
//         stack_frame.pop();
//         top.localListNames.insert(_lname);
//         stack_frame.push(top);
//     }
//     return SUCCESS;
// }




int assignVal(string lname, int index, int val)     // assigns the value 'val' to the element at index 'index' of the list 'lname'
{
    lname = findLName(lname);    // finds the list name in the current frame and the parent frame and the global scope
    if(page_table.find(lname) == page_table.end() || index > page_table[lname].getSize())
    {
        return ERROR;
    }
    page_table[lname].setElem(index, val);
    return SUCCESS;
}

int getVal(string lname, int index, int &val)       // gets the value of the element at index 'index' of the list 'lname' and stores it in 'val'
{
    lname = findLName(lname);       // finds the list name in the current frame and the parent frame and the global scope
    if(page_table.find(lname) == page_table.end() || index > page_table[lname].getSize())
    {
        return ERROR;
    }
    val = page_table[lname].getElem(index);
    return SUCCESS;
}

int freeElem(string lname)      // frees the list 'lname'
{
    lname = generateLName(lname);
    auto it = page_table.find(lname);
    if(it == page_table.end())      // if the list is not found, then return an error
    {
        exit(1);        // design choice
        return ERROR;
    }
    for(int i=0; i<(int)it->second.occupiedPages.size(); i++)       // adds the pages of the list to the free pages
    {
        freePages.push(it->second.occupiedPages[i]);
    }
    page_table.erase(it);
    return SUCCESS;
}

void freeElem()         // design choice to free elements of the current frame ONLY
{
    auto it = stack_frame.back();
    // stack_frame.pop_back();
    for(auto itr = it.localListNames.begin(); itr != it.localListNames.end(); itr++){
        freeElem(*itr);
    }
    it.localListNames.clear();
    // stack_frame.push_back(it);
}

int printPages(string lname)        // prints the pages of the list 'lname'
{
    lname = findLName(lname);
    auto it = page_table.find(lname);
    if(it == page_table.end())
    {
        return ERROR;
    }
    for(int i=0; i<(int)it->second.occupiedPages.size(); i++)
    {
        cout << it->second.occupiedPages[i] << ' ';
    }
    cout << '\n';
    return SUCCESS;
}

frame::frame()      // constructor for the frame class
{
    id = f_counter++;
    localListNames.clear();
}

void push_frame()       // pushes a new frame to the stack
{
    frame new_frame;
    stack_frame.push_back(new_frame);
}

void pop_frame()        // pops the top frame from the stack and frees the lists in the local list names of the frame
{
    auto top = stack_frame.back();
    for(auto s: top.localListNames)
    {
        string lname = generateLName(s);
        auto it = page_table.find(lname);
        if(it == page_table.end())
        {
            continue;
        }
        for(int i=0; i<(int)it->second.occupiedPages.size(); i++)
        {
            freePages.push(it->second.occupiedPages[i]);
        }
        page_table.erase(it);
    }
    stack_frame.pop_back();
}

void print_list(string lname)       // prints the list 'lname'
{
    string temp = findLName(lname);
    int s = page_table[temp].size;
    int val;
    for(int i=0; i<s; i++)
    {
        getVal(lname, i, val);
        cout << val << ' ';
    }
}

void printStackKeys()       // prints the local list names of the frames in the stack
{
    auto top = stack_frame.back();
    for(auto s: top.localListNames)
    {
        cout << s << ' ';
    }
}


















// //-----------------WITHOUT PAGE STORAGE-----------------
// #include "goodmalloc.hpp"

// void* mem;
// queue<ptr> freePages;   // stores the starting location of free pages
// unordered_map<string, list> page_table;
// stack<frame> stack_frame;
// int f_counter = 0;

// void printKeys()
// {
//     for (auto it = page_table.begin(); it != page_table.end(); it++)
//     {
//         cout << it->first << endl;
//     }
// }

// void createMem()
// {
//     mem = malloc(MEM_SIZE);
//     for (long unsigned int i = 0; i < MAXPAGES; i++)
//     {
//         freePages.push(i *  PAGESIZE);
//     }
// }

// element::element(int data = 0)
// {
//     this->_data = data;
// }

// int element::get_data()
// {
//     return this->_data;
// }

// void element::set_data(int data)
// {
//     this->_data = data;
// }

// list::list(ptr head, int size)
// {
//     this->head = head;
//     this->size = size;
// }

// list& list::operator=(const list &l)
// {
//     this->head = l.head;
//     this->size = l.size;
//     return *this;
// }

// int list::getSize()
// {
//     return this->size;
// }

// int list::getElem(int index)
// {
//     int page_num = index / PAGESIZE;
//     int offset_within_page = index % PAGESIZE;
//     ptr next_ptr = this->head;
//     element *temp;
//     temp = (element*)mem + next_ptr;
//     while(page_num--){
//         temp += (PAGESIZE-1);
//         next_ptr = temp->_next;
//         temp = (element*)mem + next_ptr;
//     }
//     temp += offset_within_page;
//     return temp->get_data();
// }

// void list::setElem(int index, int val)
// {
//     int page_num = index / PAGESIZE;
//     int offset_within_page = index % PAGESIZE;
//     ptr next_ptr = this->head;
//     element *temp;
//     temp = (element*)mem + next_ptr;
//     while(page_num--){
//         temp += (PAGESIZE-1);
//         next_ptr = temp->_next;
//         temp = (element*)mem + next_ptr;
//     }
//     temp += offset_within_page;
//     temp->set_data(val);
// }

// string generateLName(string lname)
// {
//     if(stack_frame.empty())
//     {
//         lname += "__GLOBAL";
//     }
//     else 
//     {
//         lname += "__";
//         lname += to_string(stack_frame.top().id);
//     }
//     return lname;
// }

// string findLName(string lname)
// {
//     string temp = generateLName(lname);
//     if(page_table.find(temp) != page_table.end())
//     {
//         return temp;
//     }
//     else
//     {
//         auto top = stack_frame.top();
//         stack_frame.pop();
//         temp = generateLName(lname);
//         stack_frame.push(top);
//         if(page_table.find(temp) != page_table.end())
//         {
//             return temp;
//         }
//     }
//     temp = lname + "__GLOBAL";
//     if(page_table.find(temp) != page_table.end())
//     {
//         return temp;
//     }
//     return "__ERROR";
// }

// int createList(string _lname, int num_elements)
// {
//     string lname = generateLName(_lname);
//     int num_pages = (num_elements + PAGESIZE - 1) / PAGESIZE;
//     if(num_pages > (int)freePages.size())
//     {
//         return ERROR;
//     }
//     int temp_num_elems = num_elements;
//     element *elem_ptr;
//     ptr next, prev=-1;
//     page_table[lname].size = num_elements;
//     page_table[lname].head = freePages.front();
//     for(int i=0; i<num_pages; i++)
//     {
//         // page_table[lname].occupiedPages.push_back(freePages.front());
//         next = freePages.front();
//         elem_ptr = (element*)mem + freePages.front();
//         for(int j=0; j<PAGESIZE; j++){
//             if(temp_num_elems<=0){
//                 elem_ptr--;
//                 elem_ptr->_next=-1;
//                 break;
//             }
//             temp_num_elems--;
            
//             next += 1;
//             elem_ptr->_next = next;
            
//             if(j==0){
//                 elem_ptr->_prev = prev;
//                 if(prev!=(unsigned int)-1){
//                     elem_ptr = (element*)mem + prev;
//                     elem_ptr->_next = freePages.front();
//                     elem_ptr = (element*)mem + freePages.front();
//                 }
//                 prev = freePages.front();
//             }
//             else{
//                 elem_ptr->_prev = prev;
//                 prev += 1;
//             }
//             ++elem_ptr;
//         }
//         freePages.pop();
//     }
//     if(!stack_frame.empty())
//     {
//         auto top = stack_frame.top();
//         stack_frame.pop();
//         top.localListNames.insert(_lname);
//         stack_frame.push(top);
//     }
//     return SUCCESS;
// }

// int assignVal(string lname, int index, int val)
// {
//     lname = findLName(lname);
//     if(page_table.find(lname) == page_table.end() || index > page_table[lname].getSize())
//     {
//         return ERROR;
//     }
//     page_table[lname].setElem(index, val);
//     return SUCCESS;
// }

// int getVal(string lname, int index, int &val)
// {
//     lname = findLName(lname);
//     if(page_table.find(lname) == page_table.end() || index > page_table[lname].getSize())
//     {
//         return ERROR;
//     }
//     val = page_table[lname].getElem(index);
//     return SUCCESS;
// }

// int freeElem(string lname)
// {
//     lname = generateLName(lname);
//     auto it = page_table.find(lname);
//     if(it == page_table.end())
//     {
//         exit(1);
//         return ERROR;
//     }
//     ptr head = it->second.head;
//     int size = it->second.size;
//     int num_pages = (size + PAGESIZE - 1) / PAGESIZE;
//     element *elem_ptr;
//     ptr next;
//     next = head;
//     elem_ptr = (element*)mem + head;
//     for(int i=0; i<num_pages; i++)
//     {
//         freePages.push(next);
//         elem_ptr += (PAGESIZE-1);
//         next = elem_ptr->_next;
//         elem_ptr = (element*)mem + next;
//     }
//     // for(int i=0; i<(int)it->second.occupiedPages.size(); i++)
//     // {
//     //     freePages.push(it->second.occupiedPages[i]);
//     // }
//     page_table.erase(it);
//     return SUCCESS;
// }

// // int printPages(string lname)
// // {
// //     lname = findLName(lname);
// //     auto it = page_table.find(lname);
// //     if(it == page_table.end())
// //     {
// //         return ERROR;
// //     }
// //     for(int i=0; i<(int)it->second.occupiedPages.size(); i++)
// //     {
// //         cout << it->second.occupiedPages[i] << ' ';
// //     }
// //     cout << '\n';
// //     return SUCCESS;
// // }

// frame::frame()
// {
//     id = f_counter++;
//     localListNames.clear();
// }

// void push_frame()
// {
//     frame new_frame;
//     stack_frame.push(new_frame);
// }

// void pop_frame()
// {
//     auto top = stack_frame.top();
//     for(auto s: top.localListNames)
//     {
//         freeElem(s);
//         // string lname = generateLName(s);
//         // auto it = page_table.find(lname);
//         // if(it == page_table.end())
//         // {
//         //     continue;
//         // }
//         // for(int i=0; i<(int)it->second.occupiedPages.size(); i++)
//         // {
//         //     freePages.push(it->second.occupiedPages[i]);
//         // }
//         // page_table.erase(it);
//     }
//     stack_frame.pop();
// }

// void print_list(string lname)
// {
//     string temp = findLName(lname);
//     int s = page_table[temp].size;
//     int val;
//     for(int i=0; i<s; i++)
//     {
//         getVal(lname, i, val);
//         cout << val << ' ';
//     }
// }

// void printStackKeys()
// {
//     auto top = stack_frame.top();
//     for(auto s: top.localListNames)
//     {
//         cout << s << ' ';
//     }
// }