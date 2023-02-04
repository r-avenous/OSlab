#include "utility.h"

void push_back(vectorstring *v, char *s)
{
    if(v->size == v->capacity)
    {
        v->capacity *= 2;
        v->data = (char**)realloc(v->data, sizeof(char*) * v->capacity);
    }
    v->data[v->size++] = s;
}

vectorstring split(char *s)
{
    vectorstring v;
    v.data = (char**)malloc(sizeof(char*) * 100);
    v.size = 0;
    v.capacity = 100;
    char *p = strtok(s, " ");
    while(p != NULL)
    {
        v.data[v.size] = (char*) malloc(1 + strlen(p));
        strcpy(v.data[v.size], p);
        v.size++;
        p = strtok(NULL, " ");
    }
    return v;
}

int stringEmpty(char *s)
{
    for(int i = 0; s[i]; i++)
    {
        if(s[i] != ' ' && s[i] != '\t' && s[i] != '\n') return 0;
    }
    return 1;
}