#include <iostream>
using namespace std;
#include <set>
#include <string>

void help()
{
	string A, B;
	cin >> A >> B;
	int A_size = A.length();
	int B_size = B.length();

	set<string> s;
	int ret = 0;
/*从A中切出和B串等长的子串，对每个切出的子串分别和B进行比较，如果相等就加1，同时z注意的是从A串中可能切出相同的子串，为防止重复计算，可在比较之前判断当前子串是否经在set集合中，若存在则进行下一轮循环*/
	for (int i=0; i<=A_size-B_size; i++)
	{
		string cur = A.substr(i, B_size);
//如果当前切出的子串已经在set中出现过，则说明已经统计过
		if (s.count(cur)) continue;
		s.insert(cur);
		bool flag = true;
		for (int k=0; k<B_size; k++)
		{
			if (cur[k] != B[k])
			{
				if (B[k] == '?')
				continue;
				else
				{
				flag = false;
				break;
				}
			}
		}

		if (flag)
			ret++;
	}

	cout << ret << endl;
}

int main()
{
		help();
}
