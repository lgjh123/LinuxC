#include <iostream>
using namespace std;
const long  mod=987654321;
const int maxn=1e5+5;
int main() 
{
    int n;
    cin>>n;
    int tmp[maxn];
    for(int i=1;i<=n;i++)
    {
        int k=i;
        for(int j=2;j*j<=n;j++)
        {
            int s=0;
            while(k%j==0)
            {//求需要乘因子j的个
                s++;
                k/=j;
            }
            tmp[j]=max(tmp[j],s);            
        }
        if(k>1) tmp[k]=max(tmp[k],1);//产生新的因子   ???????---
    }
    long long res=1;
    for(int i=1;i<=n;i++)
    {//将每个素因子的最大个数相乘
        for(int j=0;j<tmp[i];j++)
        {
            res=res*i%mod;
        }
    }
	cout  << res;
	return 0;
}

