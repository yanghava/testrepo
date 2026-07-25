extern int printf (const char *__restrict __format, ...)
	__attribute__ ((__format__ (__printf__, 1, 2)))
	__attribute__ ((__nothrow__));;

#define LOOP 1

int main() {
    /*below code was generated automatically by VScode*/
    int i;
    for (i = 0; i < LOOP; i++) {
        printf("Hello, World!\n");
    }
    return 0;
}
