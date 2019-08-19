#include <bits/stdc++.h>
using namespace std;
int a[30],n;
set<set<int> > S;
//pos下标
void dfs(int sum,int pos, set<int> s) {
    if(pos == n + 1) return;
    if(sum == 24) {
        S.insert(s);
        return;
    }
    dfs(sum, pos + 1, s);    //不选当前的数字
    s.insert(a[pos]); //将选择的数字加入小set中
    cout << a[pos] << endl;
    dfs(sum + a[pos],pos + 1, s);    //选择这个数字加入sum
}
int main()
{
    scanf("%d",&n);
    for(int i = 0; i < n; i++){
        scanf("%d",&a[i]);
    }
    set<int> s;
    dfs(0,0,s);
    printf("%d\n",(int)S.size());
    return 0;
}
