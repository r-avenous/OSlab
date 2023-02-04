#include "utility.hpp"

vector<string> split(string s1)
{
    string s = "";
    for(int i = 0; i < s1.size(); i++)
    {
        if(s1[i] == '|' || s1[i] == '<' || s1[i] == '>' || s1[i] == '&') 
        {
            s += ' ';
            s += s1[i];
            s += ' ';
        }
        else s += s1[i];
    }

    vector<string> v;
    stringstream ss(s);
    string item;
    while(getline(ss, item, ' '))
    {
        v.push_back(item);
    }
    return v;
}

bool stringEmpty(string s)
{
    for(char c: s)
    {
        if(c != ' ' && c != '\t' && c != '\n') return false;
    }
    return true;
}