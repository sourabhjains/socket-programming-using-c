#include <unistd.h>
#include <stdio.h>
int main()
{
    printf("text1\n");
    fork();
    fork();
    printf("text2\n");
    return 0;
}
