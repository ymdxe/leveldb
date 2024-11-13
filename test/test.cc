#include <iostream>

// #include "leveldb/include/leveldb/db.h"
// #include "dbformat.h"

using namespace std;

void test1()
{
    union {
        short s;
        char c[sizeof(short)];
    } un;
    un.s = 0x0102;
    if (sizeof(short) == 2) {
        if (un.c[0] == 1 && un.c[1] == 2) {
            cout << "大端" << endl;
        } else if (un.c[1] == 1 && un.c[0] == 2) {
            cout << "小端" << endl;
        } else {
            cout << "unknown" << endl;
        }
    } else {
        cout << "sizeof(short)=" << sizeof(short) << endl;
    }
}

void test2()
{   
    uint32_t size;
    const char *state = "11110abddak";
    // cout << sizeof(size);
    memcpy(&size, state, sizeof(size));//将state中内存取四个字节给size
    cout << size;
    // char* result = new char[size + 5];
    // memcpy(result, state, size + 5);
    // cout << result << endl;
}

void test3()
{   
    //分配的内存空间是连续的
    // char* result = new char[4096];//0x55555556aeb0 ""
    // char* res2 = new char[4096];//0x55555556bec0 ""
    // char* res3 = new char[4096];//0x55555556ced0 ""
    char* result = new char[2];//0x55555556aeb0 ""
    char* res2 = new char[2];//0x55555556aed0 ""
    char* res3 = new char[2];//0x55555556aef0 ""
    // char* res = res2 - result;
    cout << result << endl;
}

void test4()
{
    // 指针有一个地址，在该地址中存放目标的首地址
    cout << sizeof(void*) << sizeof(char*) << endl;
    char* result = new char[sizeof(void*)];
    int cnt = sizeof(result);
    char* res2 = (result + 1);
    char* res3 = res2 + 1;
    char* res4 = res3 + 1;
    result;
    cout << (static_cast<void *> (result)) << ' ' << &res2 << ' ' << &res3 << ' ' << &res4 << endl;
}

void test5()
{

}

int main() {
    test4();
    return 0;
}