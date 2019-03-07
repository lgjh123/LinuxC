#include "threadpool.h"
#include <iostream>
//static  int number = 0;
class test
{
public:
test();
~test();
void process();
};
test::test()
{
    int a = 0;
}
void test::process()
{
    pthread_t thid = pthread_self();
        sleep(3);
        //每三秒有六个线程同时运行，与线程池的六个等待线程对应
        std::cout << "this thid is = "<< thid << std::endl;
}
int main (int argc,char**argv)
{
    test * te = new test;
    threadpool<test> * pool = NULL;
    pool = new threadpool< test >;
    for(int i = 0; i < 99 ; i++)
    {
        pool->append(te);
            //sleep(1);
    //1.一开始执行程序时，线程池创建后只执行一个任务，或是只执行随机的几个，然后就结束了。是因为主函数已经返回了
    //线程执行不完的原因是：这个循环完了之后程序结束新的任务不会再被执行,把sleep 放在最外面
    }
    sleep(100);  //让主函数一直不结束
}