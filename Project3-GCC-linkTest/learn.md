# Static vs Dynamic learning

In this project, a library file is created. I'll then build a static library and a dynamic library using the same source code.

The test program, the main program, calls a function in the library.

During the link phase, I'll create 2 programs, one is linked to static library and another one is linked to the dynamic library

Here're the codes

```c
#testLib.h
#ifndef TESTLIB_H
#define TESTLIB_H

extern int libFuncPrint(void);

#endif
```

```c
#testLib.c

extern int printf (const char *__restrict __format, ...);

int libFuncPrint() {
        printf("This is printed by libFuncPrint\n");
        return 0;
}
```

```c
#testProgram.c

#include "testLib.h"

#define LOOP 1

int main() {
    /*below code was generated automatically by VScode*/
    int i;
    for (i = 0; i < LOOP; i++) {
        libFuncPrint();
    }
    return 0;
}

```

# Compile the library

```bash
[root@DA22053700:Project3-GCC-linkTest]# gcc -c testLib.c -o testLib.o
[root@DA22053700:Project3-GCC-linkTest]# objdump -t testLib.o | grep libFuncPrint
0000000000000000 g     F .text  000000000000001e libFuncPrint
[root@DA22053700:Project3-GCC-linkTest]# objdump -t testLib.o

testLib.o:     file format elf64-x86-64

SYMBOL TABLE:
0000000000000000 l    df *ABS*  0000000000000000 testLib.c
0000000000000000 l    d  .text  0000000000000000 .text
0000000000000000 l    d  .rodata        0000000000000000 .rodata
0000000000000000 g     F .text  000000000000001e libFuncPrint
0000000000000000         *UND*  0000000000000000 puts


[root@DA22053700:Project3-GCC-linkTest]# objdump -d testLib.o

testLib.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <libFuncPrint>:
   0:   f3 0f 1e fa             endbr64
   4:   55                      push   %rbp
   5:   48 89 e5                mov    %rsp,%rbp
   8:   48 8d 05 00 00 00 00    lea    0x0(%rip),%rax        # f <libFuncPrint+0xf>
   f:   48 89 c7                mov    %rax,%rdi
  12:   e8 00 00 00 00          call   17 <libFuncPrint+0x17>
  17:   b8 00 00 00 00          mov    $0x0,%eax
  1c:   5d                      pop    %rbp
  1d:   c3                      ret
```

# Create Static library and link the program

## Create static library

```bash
[root@DA22053700:Project3-GCC-linkTest]# ar rcs staticlibTestLib.a testLib.o
[root@DA22053700:Project3-GCC-linkTest]# ls -l
total 888
-rwxrwxrwx 1 root root   1678 Jul 26 10:37 staticlibTestLib.a
-rwxrwxrwx 1 root root    139 Jul 25 21:31 testLib.c
-rwxrwxrwx 1 root root     76 Jul 25 21:46 testLib.h
-rwxrwxrwx 1 root root   1528 Jul 26 10:21 testLib.o
-rwxrwxrwx 1 root root    210 Jul 25 21:48 testProgram.c
```

What this does:

- ar = archiver tool
- r = insert/replace
- c = create if doesn't exist
- s = add symbol table

Verify
```bash
[root@DA22053700:Project3-GCC-linkTest]# ar t staticlibTestLib.a
testLib.o
[root@DA22053700:Project3-GCC-linkTest]# nm staticlibTestLib.a

testLib.o:
0000000000000000 T libFuncPrint
                 U puts
```

for more information about nm command refer to [nm in details](nm.md)

## link static library

```bash
[root@DA22053700:Project3-GCC-linkTest]# gcc testProgram.c -o testProgram_static staticlibTestLib.a
[root@DA22053700:Project3-GCC-linkTest]# ls -l
total 36
-rwxrwxrwx 1 root root  1678 Jul 26 10:37 staticlibTestLib.a
-rwxrwxrwx 1 root root   139 Jul 25 21:31 testLib.c
-rwxrwxrwx 1 root root    76 Jul 25 21:46 testLib.h
-rwxrwxrwx 1 root root  1528 Jul 26 10:21 testLib.o
-rwxrwxrwx 1 root root   210 Jul 25 21:48 testProgram.c
-rwxrwxrwx 1 root root 16040 Jul 26 10:57 testProgram_static
```

Verify content of the ELF

```
[root@DA22053700:Project3-GCC-linkTest]# objdump -d testProgram_static

testProgram_static:     file format elf64-x86-64

Disassembly of section .plt:

0000000000001020 <.plt>:
    1020:       ff 35 9a 2f 00 00       push   0x2f9a(%rip)        # 3fc0 <_GLOBAL_OFFSET_TABLE_+0x8>
    1026:       f2 ff 25 9b 2f 00 00    bnd jmp *0x2f9b(%rip)        # 3fc8 <_GLOBAL_OFFSET_TABLE_+0x10>
    102d:       0f 1f 00                nopl   (%rax)
    1030:       f3 0f 1e fa             endbr64
    1034:       68 00 00 00 00          push   $0x0
    1039:       f2 e9 e1 ff ff ff       bnd jmp 1020 <_init+0x20>
    103f:       90                      nop

Disassembly of section .plt.got:

0000000000001040 <__cxa_finalize@plt>:
    1040:       f3 0f 1e fa             endbr64
    1044:       f2 ff 25 ad 2f 00 00    bnd jmp *0x2fad(%rip)        # 3ff8 <__cxa_finalize@GLIBC_2.2.5>
    104b:       0f 1f 44 00 00          nopl   0x0(%rax,%rax,1)

Disassembly of section .plt.sec:

0000000000001050 <puts@plt>:
    1050:       f3 0f 1e fa             endbr64
    1054:       f2 ff 25 75 2f 00 00    bnd jmp *0x2f75(%rip)        # 3fd0 <puts@GLIBC_2.2.5>
    105b:       0f 1f 44 00 00          nopl   0x0(%rax,%rax,1)

Disassembly of section .text:

0000000000001060 <_start>:
    1060:       f3 0f 1e fa             endbr64
    1064:       31 ed                   xor    %ebp,%ebp
    1066:       49 89 d1                mov    %rdx,%r9
    1069:       5e                      pop    %rsi
    106a:       48 89 e2                mov    %rsp,%rdx
    106d:       48 83 e4 f0             and    $0xfffffffffffffff0,%rsp
    1071:       50                      push   %rax
    1072:       54                      push   %rsp
    1073:       45 31 c0                xor    %r8d,%r8d
    1076:       31 c9                   xor    %ecx,%ecx
    1078:       48 8d 3d ca 00 00 00    lea    0xca(%rip),%rdi        # 1149 <main>
    107f:       ff 15 53 2f 00 00       call   *0x2f53(%rip)        # 3fd8 <__libc_start_main@GLIBC_2.34>
    1085:       f4                      hlt
    1086:       66 2e 0f 1f 84 00 00    cs nopw 0x0(%rax,%rax,1)


### some contents are omitted here

0000000000001149 <main>:
    1149:       f3 0f 1e fa             endbr64
    114d:       55                      push   %rbp
    114e:       48 89 e5                mov    %rsp,%rbp
    1151:       48 83 ec 10             sub    $0x10,%rsp
    1155:       c7 45 fc 00 00 00 00    movl   $0x0,-0x4(%rbp)
    115c:       eb 09                   jmp    1167 <main+0x1e>
    115e:       e8 11 00 00 00          call   1174 <libFuncPrint>
    1163:       83 45 fc 01             addl   $0x1,-0x4(%rbp)
    1167:       83 7d fc 00             cmpl   $0x0,-0x4(%rbp)
    116b:       7e f1                   jle    115e <main+0x15>
    116d:       b8 00 00 00 00          mov    $0x0,%eax
    1172:       c9                      leave
    1173:       c3                      ret

0000000000001174 <libFuncPrint>:
    1174:       f3 0f 1e fa             endbr64
    1178:       55                      push   %rbp
    1179:       48 89 e5                mov    %rsp,%rbp
    117c:       48 8d 05 85 0e 00 00    lea    0xe85(%rip),%rax        # 2008 <_IO_stdin_used+0x8>
    1183:       48 89 c7                mov    %rax,%rdi
    1186:       e8 c5 fe ff ff          call   1050 <puts@plt>
    118b:       b8 00 00 00 00          mov    $0x0,%eax
    1190:       5d                      pop    %rbp
    1191:       c3                      ret

```

**Conclusion**: We can see that the function, libFuncPrint, is also part of the linked ELF. That means the code is loaded together with the program.
However, the puts function, called within the libFuncPrint, is still dynamically loaded, as previously.

## Verification
```bash
[root@DA22053700:Project3-GCC-linkTest]# ./testProgram_static
This is printed by libFuncPrint
```

# -static parameter

When link using **-static** parameter, it seems the elf is a file containing all the libraries

```bash
[root@DA22053700:Project3-GCC-linkTest]# gcc testProgram.c -o testProgram_static_full staticlibTestLib.a -static
[root@DA22053700:Project3-GCC-linkTest]# ls -l

-rwxrwxrwx 1 root root   1678 Jul 26 10:37 staticlibTestLib.a
-rwxrwxrwx 1 root root    139 Jul 25 21:31 testLib.c
-rwxrwxrwx 1 root root     76 Jul 25 21:46 testLib.h
-rwxrwxrwx 1 root root   1528 Jul 26 10:21 testLib.o
-rwxrwxrwx 1 root root    210 Jul 25 21:48 testProgram.c
-rwxrwxrwx 1 root root  16040 Jul 26 10:57 testProgram_static
-rwxrwxrwx 1 root root 900424 Jul 26 14:34 testProgram_static_full
```

verify 
```
[root@DA22053700:Project3-GCC-linkTest]# ldd testProgram_static
        linux-vdso.so.1 (0x0000707aa35d6000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x0000707aa3200000)
        /lib64/ld-linux-x86-64.so.2 (0x0000707aa35d8000)
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]# ldd testProgram_static_full
        not a dynamic executable
```

the "testProgram_static_full" is a program that can run indepently, while the testProgram_static still loads dynamic "so" files when it runs

Examin in the in details

```bash
[root@DA22053700:Project3-GCC-linkTest]# objdump -d testProgram_static_full | grep -A30 "<main>"
0000000000401775 <main>:
  401775:       f3 0f 1e fa             endbr64
  401779:       55                      push   %rbp
  40177a:       48 89 e5                mov    %rsp,%rbp
  40177d:       48 83 ec 10             sub    $0x10,%rsp
  401781:       c7 45 fc 00 00 00 00    movl   $0x0,-0x4(%rbp)
  401788:       eb 09                   jmp    401793 <main+0x1e>
  40178a:       e8 11 00 00 00          call   4017a0 <libFuncPrint>
  40178f:       83 45 fc 01             addl   $0x1,-0x4(%rbp)
  401793:       83 7d fc 00             cmpl   $0x0,-0x4(%rbp)
  401797:       7e f1                   jle    40178a <main+0x15>
  401799:       b8 00 00 00 00          mov    $0x0,%eax
  40179e:       c9                      leave
  40179f:       c3                      ret

00000000004017a0 <libFuncPrint>:
  4017a0:       f3 0f 1e fa             endbr64
  4017a4:       55                      push   %rbp
  4017a5:       48 89 e5                mov    %rsp,%rbp
  4017a8:       48 8d 05 59 68 09 00    lea    0x96859(%rip),%rax        # 498008 <_IO_stdin_used+0x8>
  4017af:       48 89 c7                mov    %rax,%rdi
  4017b2:       e8 e9 a9 00 00          call   40c1a0 <_IO_puts>
  4017b7:       b8 00 00 00 00          mov    $0x0,%eax
  4017bc:       5d                      pop    %rbp
  4017bd:       c3                      ret
  4017be:       66 90                   xchg   %ax,%ax
```

The call to "puts" in libFuncPrint is no longer pointing to plt table, but an address - 40c1a0

```
000000000040c1a0 <_IO_puts>:
  40c1a0:       f3 0f 1e fa             endbr64
  40c1a4:       41 55                   push   %r13
  40c1a6:       41 54                   push   %r12
  40c1a8:       49 89 fc                mov    %rdi,%r12
  40c1ab:       55                      push   %rbp
  40c1ac:       53                      push   %rbx
  40c1ad:       48 83 ec 18             sub    $0x18,%rsp
  40c1b1:       e8 ca 4f ff ff          call   401180 <_init+0x180>
  40c1b6:       48 8b 2d 33 95 0b 00    mov    0xb9533(%rip),%rbp        # 4c56f0 <stdout>
  40c1bd:       48 89 c3                mov    %rax,%rbx
  40c1c0:       8b 45 00                mov    0x0(%rbp),%eax
  40c1c3:       25 00 80 00 00          and    $0x8000,%eax
  40c1c8:       75 5e                   jne    40c228 <_IO_puts+0x88>
  40c1ca:       64 4c 8b 2c 25 10 00    mov    %fs:0x10,%r13
  40c1d1:       00 00
  40c1d3:       4c 8b 85 88 00 00 00    mov    0x88(%rbp),%r8
  40c1da:       4d 39 68 08             cmp    %r13,0x8(%r8)
  40c1de:       0f 84 f4 00 00 00       je     40c2d8 <_IO_puts+0x138>
  40c1e4:       ba 01 00 00 00          mov    $0x1,%edx
  40c1e9:       f0 41 0f b1 10          lock cmpxchg %edx,(%r8)
  40c1ee:       0f 85 2c 01 00 00       jne    40c320 <_IO_puts+0x180>
  40c1f4:       4c 8b 85 88 00 00 00    mov    0x88(%rbp),%r8
  40c1fb:       48 8b 3d ee 94 0b 00    mov    0xb94ee(%rip),%rdi        # 4c56f0 <stdout>
  40c202:       4d 89 68 08             mov    %r13,0x8(%r8)
  40c206:       8b 87 c0 00 00 00       mov    0xc0(%rdi),%eax
  40c20c:       41 83 40 04 01          addl   $0x1,0x4(%r8)
  40c211:       85 c0                   test   %eax,%eax
  40c213:       74 20                   je     40c235 <_IO_puts+0x95>
  40c215:       83 f8 ff                cmp    $0xffffffff,%eax
  40c218:       74 25                   je     40c23f <_IO_puts+0x9f>
  40c21a:       b8 ff ff ff ff          mov    $0xffffffff,%eax
```

# Create a dymanic library and link the program

## create a dynamic library
gcc -shared -fPIC testLib.c -o libtestLib.so

```bash
[root@DA22053700:Project3-GCC-linkTest]# gcc -shared -fPIC testLib.c -o libtestLib.so
[root@DA22053700:Project3-GCC-linkTest]# ls -l
total 940
-rwxrwxrwx 1 root root  15576 Jul 26 15:11 libtestLib.so
-rwxrwxrwx 1 root root   1678 Jul 26 10:37 staticlibTestLib.a
-rwxrwxrwx 1 root root    139 Jul 25 21:31 testLib.c
-rwxrwxrwx 1 root root     76 Jul 25 21:46 testLib.h
-rwxrwxrwx 1 root root   1528 Jul 26 10:21 testLib.o
-rwxrwxrwx 1 root root    210 Jul 25 21:48 testProgram.c
-rwxrwxrwx 1 root root  16040 Jul 26 10:57 testProgram_static
-rwxrwxrwx 1 root root 900424 Jul 26 14:34 testProgram_static_full

```
What this does:

- shared = create shared library
- fPIC = Position Independent Code (required for .so)

Verification

```
[root@DA22053700:Project3-GCC-linkTest]# file libtestLib.so
libtestLib.so: ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, BuildID[sha1]=a89f89153df212b869c084519aedf7473d61d161, not stripped
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]# nm libtestLib.so
0000000000003e20 d _DYNAMIC
0000000000004000 d _GLOBAL_OFFSET_TABLE_
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
00000000000020e0 r __FRAME_END__
0000000000002020 r __GNU_EH_FRAME_HDR
0000000000004028 d __TMC_END__
                 w __cxa_finalize@GLIBC_2.2.5
00000000000010d0 t __do_global_dtors_aux
0000000000003e18 d __do_global_dtors_aux_fini_array_entry
0000000000004020 d __dso_handle
0000000000003e10 d __frame_dummy_init_array_entry
                 w __gmon_start__
0000000000001138 t _fini
0000000000001000 t _init
0000000000004028 b completed.0
0000000000001060 t deregister_tm_clones
0000000000001110 t frame_dummy
0000000000001119 T libFuncPrint
                 U puts@GLIBC_2.2.5
0000000000001090 t register_tm_clones
```

## Dynamic linking

```
[root@DA22053700:Project3-GCC-linkTest]# gcc testProgram.c -o testProgram_dynamic -L. -ltestLib
[root@DA22053700:Project3-GCC-linkTest]# ls -l
total 960
-rwxrwxrwx 1 root root  15576 Jul 26 15:11 libtestLib.so
-rwxrwxrwx 1 root root   1678 Jul 26 10:37 staticlibTestLib.a
-rwxrwxrwx 1 root root    139 Jul 25 21:31 testLib.c
-rwxrwxrwx 1 root root     76 Jul 25 21:46 testLib.h
-rwxrwxrwx 1 root root   1528 Jul 26 10:21 testLib.o
-rwxrwxrwx 1 root root    210 Jul 25 21:48 testProgram.c
-rwxrwxrwx 1 root root  15960 Jul 26 15:15 testProgram_dynamic
-rwxrwxrwx 1 root root  16040 Jul 26 10:57 testProgram_static
-rwxrwxrwx 1 root root 900424 Jul 26 14:34 testProgram_static_full
```

### verification

The dynamic linked file will load the libtestLib.so as it executes

```
[root@DA22053700:Project3-GCC-linkTest]# ldd testProgram_dynamic
        linux-vdso.so.1 (0x000072f5b2601000)
        libtestLib.so => not found
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x000072f5b2200000)
        /lib64/ld-linux-x86-64.so.2 (0x000072f5b2603000)
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]#
[root@DA22053700:Project3-GCC-linkTest]# LD_LIBRARY_PATH=. ldd testProgram_dynamic
        linux-vdso.so.1 (0x000073cb680d1000)
        libtestLib.so => ./libtestLib.so (0x000073cb680bf000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x000073cb67e00000)
        /lib64/ld-linux-x86-64.so.2 (0x000073cb680d3000)
```
The **LD_LIBRARY_PATH=.** indicates to search so file under given path, in addition to system path

The objdump has more details to tell the differences. What do the parameters do

  - -t, --syms               Display the contents of the symbol table(s)
  - -T, --dynamic-syms       Display the contents of the dynamic symbol table

```
[root@DA22053700:Project3-GCC-linkTest]# objdump -t testProgram_dynamic | wc -l
41
[root@DA22053700:Project3-GCC-linkTest]# objdump -t testProgram_static | wc -l
43
[root@DA22053700:Project3-GCC-linkTest]# objdump -t testProgram_static_full | wc -l
2100
```

**Conclusion** Dynamic linked ELF has the smallest symble table, while the fullly static linked ELF has the largest 

```
[root@DA22053700:Project3-GCC-linkTest]# objdump -T testProgram_dynamic

testProgram_dynamic:     file format elf64-x86-64

DYNAMIC SYMBOL TABLE:
0000000000000000      DF *UND*  0000000000000000 (GLIBC_2.34) __libc_start_main
0000000000000000  w   D  *UND*  0000000000000000  Base        _ITM_deregisterTMCloneTable
0000000000000000      DF *UND*  0000000000000000  Base        libFuncPrint
0000000000000000  w   D  *UND*  0000000000000000  Base        __gmon_start__
0000000000000000  w   D  *UND*  0000000000000000  Base        _ITM_registerTMCloneTable
0000000000000000  w   DF *UND*  0000000000000000 (GLIBC_2.2.5) __cxa_finalize


[root@DA22053700:Project3-GCC-linkTest]# objdump -T testProgram_static

testProgram_static:     file format elf64-x86-64

DYNAMIC SYMBOL TABLE:
0000000000000000      DF *UND*  0000000000000000 (GLIBC_2.34) __libc_start_main
0000000000000000  w   D  *UND*  0000000000000000  Base        _ITM_deregisterTMCloneTable
0000000000000000      DF *UND*  0000000000000000 (GLIBC_2.2.5) puts
0000000000000000  w   D  *UND*  0000000000000000  Base        __gmon_start__
0000000000000000  w   D  *UND*  0000000000000000  Base        _ITM_registerTMCloneTable
0000000000000000  w   DF *UND*  0000000000000000 (GLIBC_2.2.5) __cxa_finalize


[root@DA22053700:Project3-GCC-linkTest]# objdump -T testProgram_static_full

testProgram_static_full:     file format elf64-x86-64

objdump: testProgram_static_full: not a dynamic object
DYNAMIC SYMBOL TABLE:
no symbols
```

**Conclusion**: 
- dynamic linked elf loads the function as it runs. 
- static linked elf loads the function libFuncPrint together while the program loads. System functions are loaded while called
- Full static linked elf loads everything while the program loads

## Run the program

```
[root@DA22053700:Project3-GCC-linkTest]# ./testProgram_dynamic
./testProgram_dynamic: error while loading shared libraries: libtestLib.so: cannot open shared object file: No such file or directory
[root@DA22053700:Project3-GCC-linkTest]# LD_LIBRARY_PATH=. ./testProgram_dynamic
This is printed by libFuncPrint
```
