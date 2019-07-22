/*牛牛和羊羊正在玩一个纸牌游戏。这个游戏一共有 n 张纸牌, 第
i 张纸牌上写着数字 ai。
牛牛和羊羊轮流抽牌, 牛牛先抽, 每次抽牌他们可以从纸牌堆中任
意选择一张抽出, 直到纸牌被抽完。
他们的得分等于他们抽到的纸牌数字总和。
现在假设牛牛和羊羊都采用最优策略, 请你计算出游戏结束后牛牛
得分减去羊羊得分等于多少。*/
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;
int main()
{
    int n;
    cin >> n;
    vector<int> cards;
    int temp;
    for(int i = 0; i < n; ++i)
    {
        cin >> temp;
        cards.push_back(temp);
    }
    sort(cards.begin(),cards.begin());
    int x1 = 0,x2 = 0;

    for(int i = n-1;i>= 0; i-= 2)
    {
        x1 += cards[i];
    }
    for(int i = n=2;i>= 0; i-= 2)
    {
        x2 += cards[i];
    }
    cout << x1 - x2 <<endl;
    return 0;
}
