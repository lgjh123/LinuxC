#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

int main()
{
    string a;
    cin >> a;
    string b;
    cin >> b;
    sort (b.begin(),b.end(),greater<char>());
    auto iter2 = b.begin();
    for(auto iter = a.begin();iter!=a.end();iter++)
    {
        if(iter2!= b.end())
        {
            if(*iter < *iter2)
            {
                *iter = *iter2; 
                iter2++;
            }
        }
        else
            break;
    }
    cout << a << endl;
    return 0;
}

