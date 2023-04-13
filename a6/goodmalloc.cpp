#include "goodmalloc.hpp"

void* mem;
queue<ptr> freePages;   // stores the starting location of free pages
unordered_map<string, list> page_table;
stack<frame> stack_frame;
int f_counter = 0;

void printKeys()
{
    for (auto it = page_table.begin(); it != page_table.end(); it++)
    {
        cout << it->first << endl;
    }
}

void createMem()
{
    mem = malloc(MEM_SIZE);
    for (long unsigned int i = 0; i < MAXPAGES; i++)
    {
        freePages.push(i *  PAGESIZE);
    }
}

element::element(int data = 0)
{
    this->_data = data;
}

int element::get_data()
{
    return this->_data;
}

void element::set_data(int data)
{
    this->_data = data;
}

list::list(ptr head, int size, vector<ptr> occ_pages)
{
    this->head = head;
    this->size = size;
    this->occupiedPages = occ_pages;
}

list& list::operator=(const list &l)
{
    this->head = l.head;
    this->size = l.size;
    this->occupiedPages = l.occupiedPages;
    return *this;
}

int list::getSize()
{
    return this->size;
}

int list::getElem(int index)
{
    int page_num = index / PAGESIZE;
    int offset_within_page = index % PAGESIZE;
    element *temp;
    temp = (element*)mem + occupiedPages[page_num];
    temp += offset_within_page;
    return temp->get_data();
}

void list::setElem(int index, int val)
{
    int page_num = index / PAGESIZE;
    int offset_within_page = index % PAGESIZE;
    element *temp;
    temp = (element*)mem + occupiedPages[page_num];
    temp += offset_within_page;
    temp->set_data(val);
}

string generateLName(string lname)
{
    if(stack_frame.empty())
    {
        lname += "__GLOBAL";
    }
    else 
    {
        lname += "__";
        lname += to_string(stack_frame.top().id);
    }
    return lname;
}

string findLName(string lname)
{
    string temp = generateLName(lname);
    if(page_table.find(temp) != page_table.end())
    {
        return temp;
    }
    else
    {
        auto top = stack_frame.top();
        stack_frame.pop();
        temp = generateLName(lname);
        stack_frame.push(top);
        if(page_table.find(temp) != page_table.end())
        {
            return temp;
        }
    }
    temp = lname + "__GLOBAL";
    if(page_table.find(temp) != page_table.end())
    {
        return temp;
    }
    return "__ERROR";
}

int createList(string _lname, int num_elements)
{
    string lname = generateLName(_lname);
    int num_pages = (num_elements + PAGESIZE - 1) / PAGESIZE;
    if(num_pages > (int)freePages.size())
    {
        return ERROR;
    }
    int temp_num_elems = num_elements;
    element *elem_ptr;
    ptr next, prev=-1;
    for(int i=0; i<num_pages; i++)
    {
        page_table[lname].occupiedPages.push_back(freePages.front());
        next = freePages.front();
        elem_ptr = (element*)mem + freePages.front();
        for(int j=0; j<PAGESIZE; j++){
            if(temp_num_elems<=0){
                elem_ptr--;
                elem_ptr->_next=-1;
                break;
            }
            temp_num_elems--;
            
            next += 1;
            elem_ptr->_next = next;
            
            if(j==0){
                elem_ptr->_prev = prev;
                if(prev!=(unsigned int)-1){
                    elem_ptr = (element*)mem + prev;
                    elem_ptr->_next = freePages.front();
                    elem_ptr = (element*)mem + freePages.front();
                }
                prev = freePages.front();
            }
            else{
                elem_ptr->_prev = prev;
                prev += 1;
            }
            ++elem_ptr;
        }
        freePages.pop();
    }
    page_table[lname].size = num_elements;
    page_table[lname].head = page_table[lname].occupiedPages[0];
    if(!stack_frame.empty())
    {
        auto top = stack_frame.top();
        stack_frame.pop();
        top.localListNames.insert(_lname);
        stack_frame.push(top);
    }
    return SUCCESS;
}

int assignVal(string lname, int index, int val)
{
    lname = findLName(lname);
    if(page_table.find(lname) == page_table.end() || index > page_table[lname].getSize())
    {
        return ERROR;
    }
    page_table[lname].setElem(index, val);
    return SUCCESS;
}

int getVal(string lname, int index, int &val)
{
    lname = findLName(lname);
    if(page_table.find(lname) == page_table.end() || index > page_table[lname].getSize())
    {
        return ERROR;
    }
    val = page_table[lname].getElem(index);
    return SUCCESS;
}

int freeElem(string lname)
{
    lname = generateLName(lname);
    auto it = page_table.find(lname);
    if(it == page_table.end())
    {
        exit(1);
        return ERROR;
    }
    for(int i=0; i<(int)it->second.occupiedPages.size(); i++)
    {
        freePages.push(it->second.occupiedPages[i]);
    }
    page_table.erase(it);
    return SUCCESS;
}

int printPages(string lname)
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

frame::frame()
{
    id = f_counter++;
    localListNames.clear();
}

void push_frame()
{
    frame new_frame;
    stack_frame.push(new_frame);
}

void pop_frame()
{
    auto top = stack_frame.top();
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
    stack_frame.pop();
}

void print_list(string lname)
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

void printStackKeys()
{
    auto top = stack_frame.top();
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