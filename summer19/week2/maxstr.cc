#include <iostream>
#include <string>
#include <algorithm>
//贪心思想
using namespace std;
int main()
{
    string a;
    cin >> a;
    string b;
    while(!a.empty())
    {
        auto iter = max_element(a.begin(),b.end());
        b.append(1,*iter); 
        a.erase(a.begin(),iter+1);
    }
    return 0;
}

