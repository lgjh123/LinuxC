#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <fcntl.h>
/*
hook的目标是strcmp，所以typedef了一个STRCMP函数指针
hook的目的是要控制函数行为，从原库libc.so.6中拿到strcmp指针，保存成old_strcmp以备调用
*/
typedef int(*OPEN)(const char*, const int,...);

int open(const char *s1, const int a,...)
{
    static void *handle = NULL;
    static OPEN old_open = NULL;

    if( !handle )
    {
        handle = dlopen("libc.so.6", RTLD_LAZY);
        old_open = (OPEN)dlsym(handle, "open");
    }
    printf("oops!!! hack function invoked. s1=<%s> s2=<%d>\n", s1, a);
    return old_open(s1, a);
}
