# Investigation on data segment

## Sample program

```c

#dataVSbss.c

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
```

## compile and run

```
[root@DA22053700:LinuxKernel-1-4-MM]# vi dataVSbss.c
[root@DA22053700:LinuxKernel-1-4-MM]# gcc -g dataVSbss.c -o dataVSbss
[root@DA22053700:LinuxKernel-1-4-MM]# ./dataVSbss
Process ID: 54582

initialized_global : address=0x5b668b046010, value=1234
zero_global        : address=0x5b668b04601c, value=0
explicitly_zero    : address=0x5b668b046020, value=0
initialized_static : address=0x5b668b046014, value=5678
zero_static        : address=0x5b668b046024, value=0
read_only_global   : address=0x5b668b044008, value=999
local_variable     : address=0x7ffe1e500814, value=42

Press Enter to exit...
```

## Memory map

```
[root@DA22053700:LinuxKernel-1-4-MM]# cd /proc/54582

[root@DA22053700:54582]# cat maps
5b668b042000-5b668b043000 r--p 00000000 00:47 1407374883663653           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/dataVSbss
5b668b043000-5b668b044000 r-xp 00001000 00:47 1407374883663653           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/dataVSbss
5b668b044000-5b668b045000 r--p 00002000 00:47 1407374883663653           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/dataVSbss
5b668b045000-5b668b046000 r--p 00002000 00:47 1407374883663653           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/dataVSbss
5b668b046000-5b668b047000 rw-p 00003000 00:47 1407374883663653           /mnt/d/Codes/ProgrammerLearning/testrepo/LinuxKernel-1-4-MM/dataVSbss
5b66ad02b000-5b66ad04c000 rw-p 00000000 00:00 0                          [heap]
76e6da000000-76e6da028000 r--p 00000000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
76e6da028000-76e6da1bd000 r-xp 00028000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
76e6da1bd000-76e6da215000 r--p 001bd000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
76e6da215000-76e6da216000 ---p 00215000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
76e6da216000-76e6da21a000 r--p 00215000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
76e6da21a000-76e6da21c000 rw-p 00219000 08:30 38227                      /usr/lib/x86_64-linux-gnu/libc.so.6
76e6da21c000-76e6da229000 rw-p 00000000 00:00 0
76e6da397000-76e6da39a000 rw-p 00000000 00:00 0
76e6da3a4000-76e6da3a6000 rw-p 00000000 00:00 0
76e6da3a6000-76e6da3aa000 r--p 00000000 00:00 0                          [vvar]
76e6da3aa000-76e6da3ac000 r--p 00000000 00:00 0                          [vvar_vclock]
76e6da3ac000-76e6da3ae000 r-xp 00000000 00:00 0                          [vdso]
76e6da3ae000-76e6da3b0000 r--p 00000000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
76e6da3b0000-76e6da3da000 r-xp 00002000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
76e6da3da000-76e6da3e5000 r--p 0002c000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
76e6da3e6000-76e6da3e8000 r--p 00037000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
76e6da3e8000-76e6da3ea000 rw-p 00039000 08:30 38214                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffe1e4e1000-7ffe1e503000 rw-p 00000000 00:00 0                          [stack]
```

## Review ELF file

```
[root@DA22053700:LinuxKernel-1-4-MM]# readelf -SW dataVSbss
There are 37 section headers, starting at offset 0x3d70:

Section Headers:
  [Nr] Name              Type            Address          Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            0000000000000000 000000 000000 00      0   0  0
  [ 1] .interp           PROGBITS        0000000000000318 000318 00001c 00   A  0   0  1
  [ 2] .note.gnu.property NOTE            0000000000000338 000338 000030 00   A  0   0  8
  [ 3] .note.gnu.build-id NOTE            0000000000000368 000368 000024 00   A  0   0  4
  [ 4] .note.ABI-tag     NOTE            000000000000038c 00038c 000020 00   A  0   0  4
  [ 5] .gnu.hash         GNU_HASH        00000000000003b0 0003b0 000024 00   A  6   0  8
  [ 6] .dynsym           DYNSYM          00000000000003d8 0003d8 000108 18   A  7   1  8
  [ 7] .dynstr           STRTAB          00000000000004e0 0004e0 0000be 00   A  0   0  1
  [ 8] .gnu.version      VERSYM          000000000000059e 00059e 000016 02   A  6   0  2
  [ 9] .gnu.version_r    VERNEED         00000000000005b8 0005b8 000040 00   A  7   1  8
  [10] .rela.dyn         RELA            00000000000005f8 0005f8 0000c0 18   A  6   0  8
  [11] .rela.plt         RELA            00000000000006b8 0006b8 000078 18  AI  6  24  8
  [12] .init             PROGBITS        0000000000001000 001000 00001b 00  AX  0   0  4
  [13] .plt              PROGBITS        0000000000001020 001020 000060 10  AX  0   0 16
  [14] .plt.got          PROGBITS        0000000000001080 001080 000010 10  AX  0   0 16
  [15] .plt.sec          PROGBITS        0000000000001090 001090 000050 10  AX  0   0 16
  [16] .text             PROGBITS        00000000000010e0 0010e0 000256 00  AX  0   0 16
  [17] .fini             PROGBITS        0000000000001338 001338 00000d 00  AX  0   0  4
  [18] .rodata           PROGBITS        0000000000002000 002000 000183 00   A  0   0  8
  [19] .eh_frame_hdr     PROGBITS        0000000000002184 002184 000034 00   A  0   0  4
  [20] .eh_frame         PROGBITS        00000000000021b8 0021b8 0000ac 00   A  0   0  8
  [21] .init_array       INIT_ARRAY      0000000000003d98 002d98 000008 08  WA  0   0  8
  [22] .fini_array       FINI_ARRAY      0000000000003da0 002da0 000008 08  WA  0   0  8
  [23] .dynamic          DYNAMIC         0000000000003da8 002da8 0001f0 10  WA  7   0  8
  [24] .got              PROGBITS        0000000000003f98 002f98 000068 08  WA  0   0  8
  [25] .data             PROGBITS        0000000000004000 003000 000018 00  WA  0   0  8
  [26] .bss              NOBITS          0000000000004018 003018 000010 00  WA  0   0  4
  [27] .comment          PROGBITS        0000000000000000 003018 00002d 01  MS  0   0  1
  [28] .debug_aranges    PROGBITS        0000000000000000 003045 000030 00      0   0  1
  [29] .debug_info       PROGBITS        0000000000000000 003075 000167 00      0   0  1
  [30] .debug_abbrev     PROGBITS        0000000000000000 0031dc 0000c2 00      0   0  1
  [31] .debug_line       PROGBITS        0000000000000000 00329e 000086 00      0   0  1
  [32] .debug_str        PROGBITS        0000000000000000 003324 000165 01  MS  0   0  1
  [33] .debug_line_str   PROGBITS        0000000000000000 003489 000091 01  MS  0   0  1
  [34] .symtab           SYMTAB          0000000000000000 003520 000450 18     35  20  8
  [35] .strtab           STRTAB          0000000000000000 003970 000293 00      0   0  1
  [36] .shstrtab         STRTAB          0000000000000000 003c03 00016a 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), l (large), p (processor specific)
```

```
[root@DA22053700:LinuxKernel-1-4-MM]# nm -S -a ./dataVSbss
0000000000000000 a
0000000000000000 a Scrt1.o
0000000000003da8 d _DYNAMIC
0000000000003f98 d _GLOBAL_OFFSET_TABLE_
0000000000002000 0000000000000004 R _IO_stdin_used
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
0000000000002260 r __FRAME_END__
0000000000002184 r __GNU_EH_FRAME_HDR
0000000000004018 D __TMC_END__
000000000000038c 0000000000000020 r __abi_tag
0000000000004018 B __bss_start
                 w __cxa_finalize@GLIBC_2.2.5
0000000000004000 D __data_start
0000000000001180 t __do_global_dtors_aux
0000000000003da0 d __do_global_dtors_aux_fini_array_entry
0000000000004008 D __dso_handle
0000000000003d98 d __frame_dummy_init_array_entry
                 w __gmon_start__
                 U __libc_start_main@GLIBC_2.34
                 U __stack_chk_fail@GLIBC_2.4
0000000000004018 D _edata
0000000000004028 B _end
0000000000001338 T _fini
0000000000001000 T _init
00000000000010e0 0000000000000026 T _start
0000000000004018 0000000000000001 b completed.0
0000000000000000 a crtstuff.c
0000000000000000 a crtstuff.c
0000000000000000 a dataVSbss.c
0000000000004000 W data_start
0000000000001110 t deregister_tm_clones
0000000000004020 0000000000000004 B explicitly_zero
00000000000011c0 t frame_dummy
                 U getchar@GLIBC_2.2.5
                 U getpid@GLIBC_2.2.5
0000000000004010 0000000000000004 D initialized_global
0000000000004014 0000000000000004 d initialized_static
00000000000011c9 000000000000016d T main
                 U printf@GLIBC_2.2.5
                 U puts@GLIBC_2.2.5
0000000000002008 0000000000000004 R read_only_global
0000000000001140 t register_tm_clones
000000000000401c 0000000000000004 B zero_global
0000000000004024 0000000000000004 b zero_static
```

## Conclusion
- data segment starts from ***0000000000004000***, ends at ***0000000000004018*** according to ELF. During runtime, the start address was mapped to ***5b668b042000 + 4000 = 5b668b046000*** and ended at ***5b668b046018***
- bss segment starts from ***0000000000004018*** ends at ***0000000000004028***. During runtime, the start address was mapped to ***5b668b042000 + 4018 = 5b668b046018*** and ended at ***5b668b046028***
- Global variales with initial values were mapped in the data segment
	- initialized_global : address=***0x5b668b046010***, value=1234
	- initialized_static : address=***0x5b668b046014***, value=5678
- Variables without initial values, or default 0 values, were mapped to bss segment
	- zero_global        : address=***0x5b668b04601c***, value=0
	- explicitly_zero    : address=***0x5b668b046020***, value=0
	- zero_static        : address=***0x5b668b046024***, value=0
- Readonly variable was mapped to read only data segment
	- [18] .rodata           PROGBITS        ***0000000000002000*** 002000 000183 00   A  0   0  8
	- ***0000000000002008*** 0000000000000004 R read_only_global
	- read_only_global   : address=***0x5b668b044008***, value=999
- Stack variable was mapped to stack segment
	- 7ffe1e4e1000-***7ffe1e503000*** rw-p 00000000 00:00 0                          [stack]
	- local_variable     : address=***0x7ffe1e500814***, value=42


## Why does /proc/\<pid\>/maps show only one rw-p region in the program 

Linux maps memory in pages, usually 4096 bytes:

```
5b668b046000-5b668b047000 rw-p
```

The .data section occupies only 24 bytes:
```
[25] .data             PROGBITS        0000000000004000 003000 ***000018*** 00  WA  0   0  8
```

The .bss section occupies only 16 bytes:
```
[26] .bss              NOBITS          0000000000004018 003018 000010 00  WA  0   0  4
```

Together, they fit inside the same 4096-byte page. Linux therefore maps the entire page as one writable region.

The map does not show this:

```
.data
.bss
```

Instead, it shows the permissions of the page containing them:

```
rw-p
```
That page contains both sections:
```
                    one 4 KiB page
┌─────────────────────────────────────────┐
│ .data │ padding │ .bss │ unused space   │
└─────────────────────────────────────────┘
          rw-p
```
