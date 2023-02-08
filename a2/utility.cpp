#include "utility.hpp"

const char *history_file = ".cmd_history";
FILE* fphist;
string last_cmd;
int counter = 0;
history_state cmd_history;

vector<pair<string,int>> split(string s1, int* background)
{
    vector<pair<string,int>> v;
    string s = "";
    int wildcard = 0;
    pair<string,int> p;
    // cout << s1;
    for(int i = 0; i < s1.size(); i++)
    {
        if(s1[i]==' ') continue;
        while(s1[i] != '|' && s1[i] != '<' && s1[i] != '>' && s1[i] != '&' && s1[i] != ' ' && (i<s1.size())) 
        {
            if(s1[i] == '\"'){
                i++;
                while(s1[i] != '\"' && (i<s1.size())){
                    // if(((s1[i] == '*') || (s1[i] == '?')) && (s1[i-1] != '\\')){
                    //     wildcard = 1;
                    // }
                    s += s1[i];
                    i++;
                }
                i++;
                if(s1[i] == ' ' || s1[i] == '|' || s1[i] == '<' || s1[i] == '>' || s1[i] == '&'){
                    p = make_pair(s,wildcard);
                    v.push_back(p);
                    // wildcard = 0;
                    s="";
                    i--;
                    break;
                }
                // else{
                //     s += s1[i];
                //     i++;
                //     continue;
                // }
            }
            if(s1[i] == '\''){
                i++;
                while(s1[i] != '\'' && (i<s1.size())){
                    // if(((s1[i] == '*') || (s1[i] == '?')) && (s1[i-1] != '\\')){
                    //     wildcard = 1;
                    // }
                    s += s1[i];
                    i++;
                }
                i++;
                if(s1[i] == ' ' || s1[i] == '|' || s1[i] == '<' || s1[i] == '>' || s1[i] == '&'){
                    p = make_pair(s,wildcard);
                    v.push_back(p);
                    // wildcard = 0;
                    s="";
                    i--;
                    break;
                }
                // v.push_back(s);
                // s="";
                // continue;
            }
            if(((s1[i] == '*') || (s1[i] == '?')) && (s1[i-1] != '\\')){
                wildcard = 1;
            }
            s += s1[i];
            i++;
        }
        if(s!=""){
            p = make_pair(s,wildcard);
            v.push_back(p);
            wildcard = 0;
            s="";
        }
        if((s1[i] == '|' || s1[i] == '<' || s1[i] == '>' || s1[i] == '&') && (i<s1.size())){
            if(s1[i] == '&'){
                *background = 1;
            }
            s+=s1[i];
            p = make_pair(s,0);
            v.push_back(p);
        }
        s="";
        wildcard = 0;
    }

    // stringstream ss(s);
    // string item;
    // while(getline(ss, item, ' '))
    // {
    //     if(item[0]!=' '){
    //         v.push_back(item);
    //     }
    // }
    return v;
}


vector<pair<string,int>> wildcard_split(string s)
{
    int count = 0;
    for(int i=0; i<s.size(); i++){
        if(s[i] == '/'){
            count++;
        }
    }
    vector<pair<string,int>> v;
    pair<string,int> p;
    string str="";
    int wildcard = 0;
    p = make_pair(str,wildcard);
    // v.push_back(p);
    // int i=0;
    // if(s[0]=='/'){
    //     i++;
    // }
    for(int i=0; i<s.size(); i++){
        if(s[i]=='/'){
            count--;
            p.first = str;
            p.second = wildcard;
            v.push_back(p);
            str="";
            wildcard = 0;
        }
        else{
            if(s[i] == '*'){
                // str+=".*";
                wildcard = 1;
            }
            else if(s[i] == '?'){
                // str+=".";
                wildcard = 1;
            }
            // else if((s[i] == '.') && (count == 0)){
            //     str+="[\\.]";
            // }
            // else{
            //     str+=s[i];
            // }
            str+=s[i];
        }
    }
    p.first = str;
    p.second = wildcard;
    v.push_back(p);
    for(int i=0; i<v.size(); i++){
        cout << v[i].first << " " << v[i].second << endl;
    }
    return v;
}

vector<string> wildcard_handler(string s)
{
    vector<pair<string,int>> v = wildcard_split(s);
    vector<string> result;
    string p = "";
    result.push_back(p);
    p="";
    DIR *dir;
    for(int i=0; i<v.size(); i++){
        int size = result.size();
        for(int j=0; j<size; j++){
            if(v[i].second == 1){
                // chdir(result[j].c_str());
                if(result[0]==""){
                    dir = opendir (".");
                }
                else{
                    dir = opendir (result[0].c_str());
                }
                if(dir != NULL) {
                    struct dirent *ent;
                    while ((ent = readdir (dir)) != NULL) {
                        if(fnmatch(v[i].first.c_str(), ent->d_name, FNM_CASEFOLD) == 0){
                            p = ent->d_name;
                            result.push_back(result[0] + p + "/");
                        }
                    }
                    result.erase(result.begin());
                    closedir (dir);
                } else {
                    perror ("");
                }
            }
            else{
                result.push_back(result[0] + v[i].first + "/");
                result.erase(result.begin());
            }
        }
    }
    for(int i=0; i<result.size(); i++){
        result[i] = result[i].substr(0,result[i].size()-1);
    }
    // struct dirent *ent;
    // if ((dir = opendir (".")) != NULL) {
    //     while ((ent = readdir (dir)) != NULL) {
    //         if(fnmatch(s.c_str(), ent->d_name, FNM_CASEFOLD) == 0){
    //             p = ent->d_name;
    //             v.push_back(p);
    //         }
    //     }
    //     closedir (dir);
    // } else {
    //     perror ("");
    // }
    return result;
}

bool stringEmpty(string s)
{
    for(char c: s)
    {
        if(c != ' ' && c != '\t' && c != '\n') return false;
    }
    return true;
}

// read history from file
void read_history() {

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fphist = fopen(history_file, "r");
 
    while ((read = getline(&line, &len, fphist)) != -1)
    {
        // added to remove newline character from getline input
        if (line[read-1] == '\n')
            line[read-1] = '\0';
        cmd_history.history.push_back(string(line));
    }

    cmd_history.size = cmd_history.history.size();
    cmd_history.index = cmd_history.size - 1;

    fclose(fphist);
    if (line)
        free(line);
}

// write deque history to file
void write_history(){

    fphist = fopen(history_file, "w");
    for (int i=0; i<cmd_history.size; i++){
        fputs(cmd_history.history[i].c_str(), fphist);
        putc('\n', fphist);
    }
    fclose(fphist);
}

// function to add terminal command to deque and file
void add_history(char* s)
{
    cmd_history.history.push_back(string(s));
    cmd_history.size++;
    cmd_history.index = cmd_history.size;

    fphist = fopen(history_file, "a");
    printf("%d\n", fputs(s, fphist));
    putc('\n', fphist);
    fclose(fphist);

    if (cmd_history.size > SIZE)
    {
        cmd_history.history.pop_front();
        cmd_history.size--;
        cmd_history.index--;
        // remove first line from history file
    }
}

/* 
    Functions :

    Removes any text on the current terminal line
    Displays the previous history line
    If the first line is reached, the cursor stays on the first line
*/
int backward_history(int count, int key)
{
    // cmd_history.history[cmd_history.index] = string(rl_line_buffer);
    counter--;
    // cout << counter << endl;
    if(counter==-1){
        last_cmd = string(rl_line_buffer);
    }
    if (cmd_history.index > 0)
        cmd_history.index--;
    
    if (cmd_history.index >= 0){

        string s;
        s = cmd_history.history[cmd_history.index];
        rl_replace_line(s.c_str(), 0);
        rl_redisplay();
        rl_point = rl_end;
    }
    return 0;
}

/* 
    Functions :

    Removes any text on the current terminal line
    Displays the next history line
    If the last line is reached, the cursor stays on the last line
*/
int forward_history(int count, int key)
{
    // cmd_history.history[cmd_history.index] = string(rl_line_buffer);

    if(counter<0){
        counter++;
    }
    // cout << counter << endl;
    if (cmd_history.index < (cmd_history.size - 1)){
        cmd_history.index++;
        string s;
        s = cmd_history.history[cmd_history.index];
        rl_replace_line(s.c_str(), 0);
        rl_redisplay();
        rl_point = rl_end;
    }
    else{
        if(cmd_history.index < cmd_history.size){
            cmd_history.index++;
        }
        rl_replace_line(last_cmd.c_str(), 0);
        rl_redisplay();
        rl_point = rl_end;
    }
    return 0;
}

void initialize_readline(){

    // map backward_history function to up arrow key
    rl_bind_keyseq("\e[A", backward_history);

    // map forward_history function to down arrow key
    rl_bind_keyseq("\e[B", forward_history);

}