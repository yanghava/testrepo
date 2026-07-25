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

# update the file to include the stdio.h
```C
#include <stdio.h>

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

Then proceed with GCC and it succeeded

```bash
[root@DA22053700:Project1-GCC]# vi testProgram.c
[root@DA22053700:Project1-GCC]# gcc testProgram.c
[root@DA22053700:Project1-GCC]# ls -l
total 20
-rwxrwxrwx 1 root root 15968 Jul 22 08:17 a.out
-rwxrwxrwx 1 root root  1250 Jul 22 08:14 learn.md
-rwxrwxrwx 1 root root   219 Jul 22 08:16 testProgram.c
```
## Note
The a.out was also generated even when the warning message was printed. 


# observe results of different compile phases
- gcc -E loop.c -o loop.i (Preprocess: creates a huge text file).
- gcc -S loop.i -o loop.s (Compile: creates Assembly code).
- gcc -c loop.s -o loop.o (Assemble: creates the binary Object file).
- gcc loop.o -o loop(Link: creates the final ELF executable).

## gcc -E
```bash
[root@DA22053700:Project1-GCC]# rm a.out
[root@DA22053700:Project1-GCC]# gcc -E testProgram.c -o testProgram.i
[root@DA22053700:Project1-GCC]# ls -l
total 24
-rwxrwxrwx 1 root root  1250 Jul 22 08:14 learn.md
-rwxrwxrwx 1 root root   219 Jul 22 08:16 testProgram.c
-rwxrwxrwx 1 root root 18043 Jul 22 08:28 testProgram.i
[root@DA22053700:Project1-GCC]# file testProgram.i
testProgram.i: C source, ASCII text
[root@DA22053700:Project1-GCC]# cat testProgram.i
# 0 "testProgram.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "testProgram.c"
# 1 "/usr/include/stdio.h" 1 3 4
# 27 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 33 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 392 "/usr/include/features.h" 3 4

skip many many lines ;-)

at the bottom of the .i file, we have 

# 2 "testProgram.c" 2




# 5 "testProgram.c"
int main() {

    int i;
    for (i = 0; i < 1; i++) {
        printf("Hello, World!\n");
    }
    return 0;
}

```

so, what's in the .i file? Here's the explaination from WebAI

### 📋 Preprocessed C File Analysis

#### Summary

This `.i` file is the **preprocessed output** of your simple C program. Here's what happened:

##### 🔄 Generation Process
```bash
gcc -E testProgram.c -o testProgram.i
```
The `-E` flag tells GCC to **stop after preprocessing** — it expands all headers and macros but doesn't compile to object code.

---

#### 📊 What's in the File

##### 1️⃣ **Massive Header Expansion** (~900+ lines)
Your simple `#include <stdio.h>` expanded into:
- ✅ **stdio.h** and all its dependencies (features.h, types.h, etc.)
- ✅ **System types** (size_t, off_t, FILE struct, etc.)
- ✅ **Function declarations** (printf, fprintf, scanf, fopen, etc.)
- ✅ **Attributes** (compiler hints like `__nothrow__`, `__leaf__`)

**Why so large?** The `<stdio.h>` header itself includes many other headers to define the FILE structure and all I/O functions.

##### 2️⃣ **Macro Expansion**
```c
#define LOOP 1
```
↓ Expanded to:
```c
for (i = 0; i < 1; i++) {  // LOOP replaced with 1
```

##### 3️⃣ **Your Actual Code** (last ~10 lines)
```c
int main() {
    int i;
    for (i = 0; i < 1; i++) {
        printf("Hello, World!\n");
    }
    return 0;
}
```

---

#### 🏃 Runtime Usage

| Stage | File | Purpose |
|-------|------|---------|
| 1. Preprocess | `.i` | ✅ **Generated** (intermediate artifact) |
| 2. Compile | `.o` | ✅ **Generated** from `.i` |
| 3. Link | `a.out` | ✅ **Generated** from `.o` |
| 4. **Execute** | `a.out` | ✅ **Runtime** — `.i` is **NOT used** |

⚠️ The `.i` file is **discarded** after compilation. Only the final executable (`a.out`) is needed at runtime.

---

#### 💡 Key Observations

| Aspect | Details |
|--------|---------|
| **Loop iterations** | 1 (LOOP macro = 1) |
| **Output** | `"Hello, World!\n"` printed once |
| **Return value** | 0 (success) |
| **Dependencies** | printf (from stdio.h) |


## gcc -S 
```bash
[root@DA22053700:Project1-GCC]# ls -l
total 28
-rwxrwxrwx 1 root root  5505 Jul 24 12:42 learn.md
-rwxrwxrwx 1 root root   219 Jul 22 08:16 testProgram.c
-rwxrwxrwx 1 root root 18043 Jul 22 08:28 testProgram.i
[root@DA22053700:Project1-GCC]# gcc -S testProgram.i -O testProgram.s
[root@DA22053700:Project1-GCC]# ls -l
total 28
-rwxrwxrwx 1 root root  5505 Jul 24 12:42 learn.md
-rwxrwxrwx 1 root root   219 Jul 22 08:16 testProgram.c
-rwxrwxrwx 1 root root 18043 Jul 22 08:28 testProgram.i
-rwxrwxrwx 1 root root   631 Jul 24 12:44 testProgram.s
[root@DA22053700:Project1-GCC]# file testProgram.s
testProgram.s: assembler source, ASCII text
[root@DA22053700:Project1-GCC]# cat testProgram.s
        .file   "testProgram.c"
        .text
        .section        .rodata.str1.1,"aMS",@progbits,1
.LC0:
        .string "Hello, World!"
        .text
        .globl  main
        .type   main, @function
main:
.LFB0:
        .cfi_startproc
        endbr64
        subq    $8, %rsp
        .cfi_def_cfa_offset 16
        leaq    .LC0(%rip), %rdi
        call    puts@PLT
        movl    $0, %eax
        addq    $8, %rsp
        .cfi_def_cfa_offset 8
        ret
        .cfi_endproc
.LFE0:
        .size   main, .-main
        .ident  "GCC: (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0"
        .section        .note.GNU-stack,"",@progbits
        .section        .note.gnu.property,"a"
        .align 8
        .long   1f - 0f
        .long   4f - 1f
        .long   5
0:
        .string "GNU"
1:
        .align 8
        .long   0xc0000002
        .long   3f - 2f
2:
        .long   0x3
3:
        .align 8
4:
```
### [explaination to the gcc -S](gcc-S-explained.md)

## gcc -c
```bash
[root@DA22053700:Project1-GCC]# gcc -c testProgram.s -o testProgram.o
[root@DA22053700:Project1-GCC]# file testProgram.o
testProgram.o: ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), not stripped
```

### [explaination to the gcc -c](gcc-C-explained.md)

## gcc link
```bash
[root@DA22053700:Project1-GCC]# gcc testProgram.o -o testProgram

[root@DA22053700:Project1-GCC]# ls -l testProgram*
-rwxrwxrwx 1 root root 15968 Jul 24 16:22 testProgram
-rwxrwxrwx 1 root root   219 Jul 22 08:16 testProgram.c
-rwxrwxrwx 1 root root 18043 Jul 22 08:28 testProgram.i
-rwxrwxrwx 1 root root  1512 Jul 24 15:39 testProgram.o
-rwxrwxrwx 1 root root   631 Jul 24 12:44 testProgram.s

```

### [explaination to gcc link](gcc-link-explained.md)

```bash
[root@DA22053700:Project1-GCC]# ./testProgram
Hello, World!
```
