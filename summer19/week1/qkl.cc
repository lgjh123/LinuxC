/*小 Q 的父母要出差 N 天,走之前给小 Q 留下了 M 块巧克力。小 Q
决定每天吃的巧克力数量不少于前一天吃的一半,但是他又不想在父
母回来之前的某一天没有巧克力吃,请问他第一天最多能吃多少块巧
克力?*/
#include <iostream>
using namespace std;

int main()
{
    int N,M,i,j,sum_eat,current_eat,last_eat;

    cin >> N >> M;
    for(i = M-N+1;i >= 1;i--)
    {
        sum_eat = i;
        j = 1;
        last_eat = i;
        while(j<N)
        {
            if(last_eat <= 2)
            {
                current_eat = 1;
            }
            else
            {
                current_eat = last_eat / 2 + (last_eat % 2);
            }

            sum_eat += current_eat;
            if(sum_eat > M)
            {
                break;
            }

            last_eat = current_eat;
            j++;
        }
        if(j == N)
        {
            break;
        }
    }
    cout << i << endl;

    return 0;
}
