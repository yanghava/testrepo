# 0 "testProgramWithAttr.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "testProgramWithAttr.c"
extern int printf (const char *__restrict __format, ...)
 __attribute__ ((__format__ (__printf__, 1, 2)))
 __attribute__ ((__nothrow__));;



int main() {

    int i;
    for (i = 0; i < 1; i++) {
        printf("Hello, World!\n");
    }
    return 0;
}
