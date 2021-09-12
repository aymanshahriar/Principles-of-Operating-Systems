#include <iostream>
#include <cstdio>
#include <unistd.h>

using namespace std;

int main()
{
    cout<<"Process ID is :"<<getpid()<<endl;
    cout<<"Parent Process id is "<<getppid()<<endl;

    return 0;
}
