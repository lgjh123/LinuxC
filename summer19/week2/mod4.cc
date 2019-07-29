//寻找可以被4和不可以被2整除的数的个数
//一个不可以被4和2整除的数周围必须有2个可以被4整除的数，除了第一个
//那么对于每一个n1都必须有一个属于它的n4，如果没有n2，那第一个n1可以和第2个共有一个
//所以就是 n4>=n1||(n2==0&&n4>=n1-1)
#include<iostream>
using namespace std;
int main(){
    int n;
    int l;
    int num;
    int n1,n4;
    while(cin>>n){
        for(int i = 0;i<n;i++){
            cin>>l;
            n1 = 0;
            n4 = 0;
            for(int j = 0;j<l;j++){
                cin>>num;
                if(num%4==0)
                    n4++;
                else if(num%2!=0)
                    n1++;
            }
            if(n4>=n1||(n4>=n1-1&&(n4+n1)==l))
                cout<<"Yes"<<endl;
            else
                cout<<"No"<<endl;
              
        }
    }
    return 0;
}
