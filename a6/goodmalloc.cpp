#include "goodmalloc.hpp"

void* mem;      // stores the starting location of the memory
queue<ptr> freePages;   // stores the starting location of free pages
unordered_map<string, list> page_table;     // stores the list name and the list object
vector<frame> stack_frame;       // stores the frames
int f_counter = 0;      // counter for the frame id

int getNumUsedPages()      
{
    return (MAXPAGES - freePages.size());
}

void printKeys()    // prints the names of the lists in the page table
{
    for (auto it = page_table.begin(); it != page_table.end(); it++)
    {
        cout << it->first << endl;
    }
}

void createMem()        // creates the memory
{
    // cout << "Creating memory segment of size " << MEM_SIZE << " bytes ..." << "\n\n";
    mem = malloc(MEM_SIZE);
    for (long unsigned int i = 0; i < MAXPAGES; i++)
    {
        freePages.push(i *  PAGESIZE);
    }
}

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
    temp = lname + "__GLOBAL";    // if the list is not found in the current frame or the parent frame, then search in the global scope
    if(page_table.find(temp) != page_table.end())
    {
        return temp;
    }
    else        // if the list is not found in the current frame, then search in the parent frame
    {
        for(int i=stack_frame.size()-2; i>=0; i--)
        {
            temp = lname + "__" + to_string(stack_frame[i].id);
            if(page_table.find(temp) != page_table.end())
            {
                return temp;
            }
        }
    }
    return "__ERROR";   // if the list is not found in the global scope, then return an error
}

int createList(string _lname, int num_elements)     // creates a list with name 'lname' and size 'num_elements'
{
    if(!stack_frame.empty())        // if the stack is not empty, then add the list name to the local list names of the current frame
    {
        auto top = stack_frame.back();
        cout << "Creating list " << _lname << " in scope " << top.id << " with " << num_elements << " elements ..." << "\n";
    }
    
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

void clearAll()
{
    while(!stack_frame.empty())     // pops all the frames from the stack
    {
        while(!stack_frame.back().localListNames.empty())     // pops all the lists from the current frame
        {
            string lname = *stack_frame.back().localListNames.begin();
            freeElem(lname);
            stack_frame.back().localListNames.erase(lname);
        }
        stack_frame.pop_back();
    }
    while(!page_table.empty())
    {
        string x = page_table.begin()->first;
        x = x.substr(0, x.find('_'));
        freeElem(x);
    }
}

int assignVal(string lname, int index, int val)     // assigns the value 'val' to the element at index 'index' of the list 'lname'
{
    // cout << "assignVal: " << lname << "[" << index << "] = " << val << "\n";
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
    // cout << "getVal: " << lname << "[" << index << "]\n";
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
    cout << "Freeing list " << lname << "\n\n";
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

void freeElem()         // design choice to free lists of the current frame ONLY or all global lists if the stack is empty
{
    cout << "Freeing unused lists" << "\n\n";
    if(stack_frame.empty())
    {
        while(!page_table.empty())
        {
            string x = page_table.begin()->first;
            x = x.substr(0, x.find('_'));
            freeElem(x);
        }
        return;
    }
    auto it = stack_frame.back();
    for(auto itr = it.localListNames.begin(); itr != it.localListNames.end(); itr++){
        freeElem(*itr);
    }
    it.localListNames.clear();
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
    cout << "Creating new scope " << new_frame.id << " and pushing frame to global stack ..." << "\n";
    stack_frame.push_back(new_frame);
}

void pop_frame()        // pops the top frame from the stack and frees the lists in the local list names of the frame
{
    auto top = stack_frame.back();
    cout << "Popping scope " << top.id << " from global stack and freeing all local lists ..." << endl;
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
