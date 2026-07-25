# 0 "testProgram.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "testProgram.c"
extern int printf (const char *__restrict __format, ...);



int main() {

    int i;
    for (i = 0; i < 1; i++) {
        printf("Hello, World!\n");
    }
    return 0;
}
