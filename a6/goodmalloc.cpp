#include "goodmalloc.hpp"

void* mem;
queue<ptr> freePages;   // stores the starting location of free pages
unordered_map<string, list> page_table;

void createMem()
{
    mem = malloc(MEM_SIZE);
    for (int i = 0; i < MAXPAGES; i++)
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

list::~list()
{
    // for(int i=0; i<occupiedPages.size(); i++)
    // {
    //     freePages.push(occupiedPages[i]);
    // }
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
    // element *elem_ptr = (element *)temp;
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

int createList(string lname, int num_elements)
{
    int num_pages = (num_elements + PAGESIZE - 1) / PAGESIZE;
    // cout << num_pages << ' ' << freePages.size() << '\n';
    if(num_pages > (int)freePages.size())
    {
        return ERROR;
    }
    // vector<ptr> occ_pages;
    // ptr prev_page;
    int temp_num_elems = num_elements;
    element *elem_ptr;//, *prev_elem_ptr;
    ptr next, prev=-1;
    for(int i=0; i<num_pages; i++)
    {
        page_table[lname].occupiedPages.push_back(freePages.front());
        next = freePages.front();
        cout << "\npage change starts from " << next << endl; 
        elem_ptr = (element*)mem + freePages.front();
        for(int j=0; j<PAGESIZE; j++){
            // cout << "pgno. "  <<temp_num_elems << endl;
            if(temp_num_elems<=0){
                elem_ptr--;
                elem_ptr->_next=-1;
                cout << elem_ptr->_next << ' ';
                break;
            }
            temp_num_elems--;
            
            next += 1;//sizeof(element);
            elem_ptr->_next = next;
            cout << elem_ptr->_next<< ' ';
            
            if(j==0){
                elem_ptr->_prev = prev;
                cout << elem_ptr->_prev<< endl;
                if(prev!=-1){
                    elem_ptr = (element*)mem + prev;
                    elem_ptr->_next = freePages.front();
                    cout << elem_ptr->_next<< ' ';
                    elem_ptr = (element*)mem + freePages.front();
                }
                prev = freePages.front();
            }
            else{
                elem_ptr->_prev = prev;
                cout << elem_ptr->_prev << endl;
                prev += 1;//sizeof(element);
            }
            ++elem_ptr;
        }
        // prev += 1;//sizeof(element);
        freePages.pop();
        // cout << freePages.size() << '\n';
    }
    page_table[lname].size = num_elements;
    page_table[lname].head = page_table[lname].occupiedPages[0];
    // page_table[lname] = list(occ_pages[0], num_elements, occ_pages);
    return SUCCESS;
}

int assignVal(string lname, int index, int val)
{
    if(page_table.find(lname) == page_table.end() || index > page_table[lname].getSize()){
        return ERROR;
    }
    page_table[lname].setElem(index, val);
    return SUCCESS;
}

int getVal(string lname, int index, int &val)
{
    if(page_table.find(lname) == page_table.end() || index > page_table[lname].getSize())
    {
        return ERROR;
    }
    val = page_table[lname].getElem(index);
    return SUCCESS;
}

int freeElem(string lname)
{
    auto it = page_table.find(lname);
    if(it == page_table.end())
    {
        return ERROR;
    }
    for(int i=0; i<it->second.occupiedPages.size(); i++)
    {
        freePages.push(it->second.occupiedPages[i]);
    }
    page_table.erase(it);
    return SUCCESS;
}

int printPages(string lname)
{
    auto it = page_table.find(lname);
    if(it == page_table.end())
    {
        return ERROR;
    }
    for(int i=0; i<it->second.occupiedPages.size(); i++)
    {
        cout << it->second.occupiedPages[i] << ' ';
    }
    cout << '\n';
    return SUCCESS;
}