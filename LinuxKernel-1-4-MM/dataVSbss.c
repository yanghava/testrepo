#include <stdio.h>
#include <unistd.h>

int initialized_global = 1234;     // .data
int zero_global;                   // .bss
int explicitly_zero = 0;           // usually .bss

static int initialized_static = 5678; // .data
static int zero_static;                // .bss

const int read_only_global = 999;  // .rodata, not .data

int main(void)
{
    int local_variable = 42;       // stack

    printf("Process ID: %d\n\n", getpid());

    printf("initialized_global : address=%p, value=%d\n",
           (void *)&initialized_global, initialized_global);

    printf("zero_global        : address=%p, value=%d\n",
           (void *)&zero_global, zero_global);

    printf("explicitly_zero    : address=%p, value=%d\n",
           (void *)&explicitly_zero, explicitly_zero);

    printf("initialized_static : address=%p, value=%d\n",
           (void *)&initialized_static, initialized_static);

    printf("zero_static        : address=%p, value=%d\n",
           (void *)&zero_static, zero_static);

    printf("read_only_global   : address=%p, value=%d\n",
           (void *)&read_only_global, read_only_global);

    printf("local_variable     : address=%p, value=%d\n",
           (void *)&local_variable, local_variable);

    printf("\nPress Enter to exit...\n");
    getchar();

    return 0;
}
