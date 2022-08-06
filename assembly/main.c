#include <stdio.h>

static int a = 0;

int set_a(int val)
{
    a = val;
    return 0;
}

int main()
{
    int temp = 10;
    set_a(temp);
    return 0;
}