class Solution {
public:
    /**
     * 获得两个整形二进制表达位数不同的数量
     *
     * @param m 整数m
     * @param n 整数n
     * @return 整型
     */
    int countBitDiff(int m, int n) {
           int count = 0;
           int num = m ^ n;
           while(num)
           {
               num = num&(num-1);
              ++ count ;
          }
          return count;

    }
};
