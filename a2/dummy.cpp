#include <iostream>
#include <string>

using namespace std;
int main()
{
    // sleep(3);
    string s;
    cin >> s;
    cout << s;

    for (int i = 0; i< s.length() - 2; i++) printf("\b");
    return 0;
}