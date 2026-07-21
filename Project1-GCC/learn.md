# I forgot almost everything about C!
I tried to create a new file in VSCode. and it helped me to make up everything!
```C
#define LOOP 1

int main() {
    /*below code was generated automatically by VScode*/
    int i;
    for (i = 0; i < LOOP; i++) {
        printf("Hello, World!\n");
    }
    return 0;
}
```
When I finished, I managed to configure my WSL to connect to the same repository.

At this time I tried to download, using 'git pull' command. THen when I tried to compile, I just failed

```bash
[root@DA22053700:Project1-GCC]# gcc testProgram.c
testProgram.c: In function ‘main’:
testProgram.c:7:9: warning: implicit declaration of function ‘printf’ [-Wimplicit-function-declaration]
    7 |         printf("Hello, World!\n");
      |         ^~~~~~
testProgram.c:1:1: note: include ‘<stdio.h>’ or provide a declaration of ‘printf’
  +++ |+#include <stdio.h>
    1 | #define LOOP 1
testProgram.c:7:9: warning: incompatible implicit declaration of built-in function ‘printf’ [-Wbuiltin-declaration-mismatch]
    7 |         printf("Hello, World!\n");
      |         ^~~~~~
testProgram.c:7:9: note: include ‘<stdio.h>’ or provide a declaration of ‘printf’
```
what a mess!

