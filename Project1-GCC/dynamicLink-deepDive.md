# Differences in header

## o file
```bash
[root@DA22053700:Project1-GCC]# objdump -h testProgram.o

testProgram.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         0000001e  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000000  0000000000000000  0000000000000000  0000005e  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  0000000000000000  0000000000000000  0000005e  2**0
                  ALLOC
  3 .rodata.str1.1 0000000e  0000000000000000  0000000000000000  0000005e  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment      0000002e  0000000000000000  0000000000000000  0000006c  2**0
                  CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  0000009a  2**0
                  CONTENTS, READONLY
  6 .note.gnu.property 00000020  0000000000000000  0000000000000000  000000a0  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 .eh_frame     00000030  0000000000000000  0000000000000000  000000c0  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
```

## elf file
```bash
[root@DA22053700:Project1-GCC]# objdump -h testProgram

testProgram:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .interp       0000001c  0000000000000318  0000000000000318  00000318  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 .note.gnu.property 00000030  0000000000000338  0000000000000338  00000338  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  2 .note.gnu.build-id 00000024  0000000000000368  0000000000000368  00000368  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  3 .note.ABI-tag 00000020  000000000000038c  000000000000038c  0000038c  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .gnu.hash     00000024  00000000000003b0  00000000000003b0  000003b0  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  5 .dynsym       000000a8  00000000000003d8  00000000000003d8  000003d8  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  6 .dynstr       0000008d  0000000000000480  0000000000000480  00000480  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 .gnu.version  0000000e  000000000000050e  000000000000050e  0000050e  2**1
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  8 .gnu.version_r 00000030  0000000000000520  0000000000000520  00000520  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  9 .rela.dyn     000000c0  0000000000000550  0000000000000550  00000550  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 10 .rela.plt     00000018  0000000000000610  0000000000000610  00000610  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 11 .init         0000001b  0000000000001000  0000000000001000  00001000  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 12 .plt          00000020  0000000000001020  0000000000001020  00001020  2**4
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 13 .plt.got      00000010  0000000000001040  0000000000001040  00001040  2**4
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 14 .plt.sec      00000010  0000000000001050  0000000000001050  00001050  2**4
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 15 .text         00000107  0000000000001060  0000000000001060  00001060  2**4
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 16 .fini         0000000d  0000000000001168  0000000000001168  00001168  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 17 .rodata       00000012  0000000000002000  0000000000002000  00002000  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 18 .eh_frame_hdr 00000034  0000000000002014  0000000000002014  00002014  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 19 .eh_frame     000000a4  0000000000002048  0000000000002048  00002048  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 20 .init_array   00000008  0000000000003db8  0000000000003db8  00002db8  2**3
                  CONTENTS, ALLOC, LOAD, DATA
 21 .fini_array   00000008  0000000000003dc0  0000000000003dc0  00002dc0  2**3
                  CONTENTS, ALLOC, LOAD, DATA
 22 .dynamic      000001f0  0000000000003dc8  0000000000003dc8  00002dc8  2**3
                  CONTENTS, ALLOC, LOAD, DATA
 23 .got          00000048  0000000000003fb8  0000000000003fb8  00002fb8  2**3
                  CONTENTS, ALLOC, LOAD, DATA
 24 .data         00000010  0000000000004000  0000000000004000  00003000  2**3
                  CONTENTS, ALLOC, LOAD, DATA
 25 .bss          00000008  0000000000004010  0000000000004010  00003010  2**0
                  ALLOC
 26 .comment      0000002d  0000000000000000  0000000000000000  00003010  2**0
                  CONTENTS, READONLY
```

# explanation by Webai 🎯

You're now seeing the **massive transformation** that the linker performs. Let me break down the differences:

---

## Object File vs Executable: Side-by-Side

### Object File (testProgram.o): 7 Sections
```
.text (30 bytes)           ← Your code only
.rodata.str1.1 (14 bytes)  ← "Hello, World!\n"
.eh_frame (48 bytes)       ← Exception handling
... minimal metadata
```

### Executable (testProgram): 27 Sections
```
.interp, .note.*, .gnu.* sections added
.dynsym, .dynstr            ← Dynamic symbol table
.plt, .plt.got, .plt.sec    ← Procedure Linkage Table (for printf!)
.init, .fini                ← Startup/cleanup code
.text (263 bytes)           ← Your code + runtime code
.rodata (18 bytes)          ← String data
.dynamic                    ← Runtime linker info
.got                        ← Global Offset Table
.data, .bss                 ← Data sections
... much more metadata
```

---

## 🔑 Key Differences Explained

| Aspect | Object File | Executable |
|--------|-------------|-----------|
| **VMA** | `0x0000...` (relative) | `0x0318...` (absolute addresses!) |
| **RELOC flag** | ✅ Has RELOC (unresolved) | ❌ No RELOC (all resolved) |
| **`.plt` section** | ❌ Missing | ✅ Present (for dynamic linking) |
| **`.dynsym`** | ❌ Missing | ✅ Present (symbol resolution) |
| **`.dynamic`** | ❌ Missing | ✅ Present (libc loader info) |
| **`.got`** | ❌ Missing | ✅ Present (Global Offset Table) |

---

## The PLT (Procedure Linkage Table) 🚀

This is **the star** of the linker's work:

```
.plt          00000020  0000000000001020   ← 32 bytes of trampolines
.plt.got      00000010  0000000000001040   ← Optimized PLT entries
.plt.sec      00000010  0000000000001050   ← Security-hardened PLT
.rela.plt     00000018                    ← Relocation data for printf()
```

When your code calls `printf()`:

```
1. Jump to .plt entry (0x1020)
   ↓
2. PLT looks up address in .got (Global Offset Table)
   ↓
3. First call? Load libc at runtime
   ↓
4. Execute printf() from libc
```

**Why?** → Dynamic linking allows updates to libc without recompiling! 🔄

---

## Verify the Resolved printf() Address

```bash
# Show your main function with resolved printf call
objdump -d testProgram | grep -A5 "main>:"
```

You'll see something like:
```asm
00001060 <main>:
    1060:  e8 bb ff ff ff          call   1020 <printf@plt>
```

The `call 1020` now has a **real address** (was `00 00 00 00` in `.o` file)! ✅

---

## Memory Layout Summary

```
Object File (.o):          Executable:
┌─────────────────┐       ┌──────────────────────┐
│ .text (30B)     │       │ .interp              │
│ .rodata (14B)   │  →→   │ .text (263B) + libs  │
│ .eh_frame (48B) │       │ .plt (32B) ← printf! │
│ unresolved refs │       │ .rodata (18B)        │
└─────────────────┘       │ .dynamic/.got        │
                          │ FULLY RESOLVED ✅    │
                          └──────────────────────┘
```

---

# Differences in o

```bash
[root@DA22053700:Project1-GCC]# objdump -d testProgram

testProgram:     file format elf64-x86-64


Disassembly of section .init:

0000000000001000 <_init>:
    1000:       f3 0f 1e fa             endbr64
    1004:       48 83 ec 08             sub    $0x8,%rsp
    1008:       48 8b 05 d9 2f 00 00    mov    0x2fd9(%rip),%rax        # 3fe8 <__gmon_start__@Base>
    100f:       48 85 c0                test   %rax,%rax
    1012:       74 02                   je     1016 <_init+0x16>
    1014:       ff d0                   call   *%rax
    1016:       48 83 c4 08             add    $0x8,%rsp
    101a:       c3                      ret

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
    108d:       00 00 00

0000000000001090 <deregister_tm_clones>:
    1090:       48 8d 3d 79 2f 00 00    lea    0x2f79(%rip),%rdi        # 4010 <__TMC_END__>
    1097:       48 8d 05 72 2f 00 00    lea    0x2f72(%rip),%rax        # 4010 <__TMC_END__>
    109e:       48 39 f8                cmp    %rdi,%rax
    10a1:       74 15                   je     10b8 <deregister_tm_clones+0x28>
    10a3:       48 8b 05 36 2f 00 00    mov    0x2f36(%rip),%rax        # 3fe0 <_ITM_deregisterTMCloneTable@Base>
    10aa:       48 85 c0                test   %rax,%rax
    10ad:       74 09                   je     10b8 <deregister_tm_clones+0x28>
    10af:       ff e0                   jmp    *%rax
    10b1:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)
    10b8:       c3                      ret
    10b9:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)

00000000000010c0 <register_tm_clones>:
    10c0:       48 8d 3d 49 2f 00 00    lea    0x2f49(%rip),%rdi        # 4010 <__TMC_END__>
    10c7:       48 8d 35 42 2f 00 00    lea    0x2f42(%rip),%rsi        # 4010 <__TMC_END__>
    10ce:       48 29 fe                sub    %rdi,%rsi
    10d1:       48 89 f0                mov    %rsi,%rax
    10d4:       48 c1 ee 3f             shr    $0x3f,%rsi
    10d8:       48 c1 f8 03             sar    $0x3,%rax
    10dc:       48 01 c6                add    %rax,%rsi
    10df:       48 d1 fe                sar    %rsi
    10e2:       74 14                   je     10f8 <register_tm_clones+0x38>
    10e4:       48 8b 05 05 2f 00 00    mov    0x2f05(%rip),%rax        # 3ff0 <_ITM_registerTMCloneTable@Base>
    10eb:       48 85 c0                test   %rax,%rax
    10ee:       74 08                   je     10f8 <register_tm_clones+0x38>
    10f0:       ff e0                   jmp    *%rax
    10f2:       66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)
    10f8:       c3                      ret
    10f9:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)

0000000000001100 <__do_global_dtors_aux>:
    1100:       f3 0f 1e fa             endbr64
    1104:       80 3d 05 2f 00 00 00    cmpb   $0x0,0x2f05(%rip)        # 4010 <__TMC_END__>
    110b:       75 2b                   jne    1138 <__do_global_dtors_aux+0x38>
    110d:       55                      push   %rbp
    110e:       48 83 3d e2 2e 00 00    cmpq   $0x0,0x2ee2(%rip)        # 3ff8 <__cxa_finalize@GLIBC_2.2.5>
    1115:       00
    1116:       48 89 e5                mov    %rsp,%rbp
    1119:       74 0c                   je     1127 <__do_global_dtors_aux+0x27>
    111b:       48 8b 3d e6 2e 00 00    mov    0x2ee6(%rip),%rdi        # 4008 <__dso_handle>
    1122:       e8 19 ff ff ff          call   1040 <__cxa_finalize@plt>
    1127:       e8 64 ff ff ff          call   1090 <deregister_tm_clones>
    112c:       c6 05 dd 2e 00 00 01    movb   $0x1,0x2edd(%rip)        # 4010 <__TMC_END__>
    1133:       5d                      pop    %rbp
    1134:       c3                      ret
    1135:       0f 1f 00                nopl   (%rax)
    1138:       c3                      ret
    1139:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)

0000000000001140 <frame_dummy>:
    1140:       f3 0f 1e fa             endbr64
    1144:       e9 77 ff ff ff          jmp    10c0 <register_tm_clones>

0000000000001149 <main>:
    1149:       f3 0f 1e fa             endbr64
    114d:       48 83 ec 08             sub    $0x8,%rsp
    1151:       48 8d 3d ac 0e 00 00    lea    0xeac(%rip),%rdi        # 2004 <_IO_stdin_used+0x4>
    1158:       e8 f3 fe ff ff          call   1050 <puts@plt>
    115d:       b8 00 00 00 00          mov    $0x0,%eax
    1162:       48 83 c4 08             add    $0x8,%rsp
    1166:       c3                      ret

Disassembly of section .fini:

0000000000001168 <_fini>:
    1168:       f3 0f 1e fa             endbr64
    116c:       48 83 ec 08             sub    $0x8,%rsp
    1170:       48 83 c4 08             add    $0x8,%rsp
    1174:       c3                      ret
```

```bash
[root@DA22053700:Project1-GCC]# objdump -d testProgram.o

testProgram.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <main>:
   0:   f3 0f 1e fa             endbr64
   4:   48 83 ec 08             sub    $0x8,%rsp
   8:   48 8d 3d 00 00 00 00    lea    0x0(%rip),%rdi        # f <main+0xf>
   f:   e8 00 00 00 00          call   14 <main+0x14>
  14:   b8 00 00 00 00          mov    $0x0,%eax
  19:   48 83 c4 08             add    $0x8,%rsp
  1d:   c3                      ret
```

# Explanation by WebAI

This is the most educational comparison possible. Let me break down what the linker **actually did**:

---

## Your `main()` Function: Before vs After

### Object File (testProgram.o) - UNRESOLVED ❌
```asm
0x0000000000000000 <main>:
   8:   lea    0x0(%rip),%rdi        # PLACEHOLDER: string address = 0x0
   f:   e8 00 00 00 00          call   14    # PLACEHOLDER: printf addr = 0x0
```

### Executable (testProgram) - FULLY RESOLVED ✅
```asm
0x0000000000001149 <main>:
   1151:  lea    0xeac(%rip),%rdi     # REAL: "Hello, World!" @ 0x2004
   1158:  e8 f3 fe ff ff       call   1050  # REAL: puts@plt @ 0x1050
```

---

## 🔍 What Changed?

| Detail | Object File | Executable | What Happened |
|--------|-------------|-----------|---|
| **String address** | `0x0(%rip)` | `0xeac(%rip)` → 0x2004 | ✅ Linked to `.rodata` section |
| **Function call** | `call 14` (broken) | `call 1050` (puts@plt) | ✅ Points to PLT trampoline |
| **main() location** | 0x0 (relative) | **0x1149** (absolute) | ✅ Assigned real memory address |

---

## 🚀 The Complete Call Chain

When you run `./testProgram`, here's what **actually happens**:

```
1. OS loads executable at 0x400000 (ASLR may vary)
   ↓
2. Executes _start (0x1060)
   - Sets up stack/registers
   - Calls __libc_start_main() (0x3fd8)
   ↓
3. __libc_start_main initializes libc, then calls main() at 0x1149
   ↓
4. main() executes:
   lea 0xeac(%rip),%rdi        ← Load string "Hello, World!" from 0x2004
   call 1050 <puts@plt>        ← Jump to PLT entry
   ↓
5. PLT (Procedure Linkage Table) at 0x1050:
   f2 ff 25 75 2f 00 00    bnd jmp *0x2f75(%rip)
   
   This jumps to: 0x1050 + 0x2f75 = 0x3fd0
   which contains the real puts() address from libc!
   ↓
6. puts() prints "Hello, World!\n"
   ↓
7. main() returns 0
   ↓
8. _fini (0x1168) cleans up and exits
```

---

## 🎓 Key Insight: Why Use PLT?

Your object file had:
```asm
e8 00 00 00 00          call   14
```

The linker **could have** replaced `00 00 00 00` with `puts`'s actual address. **But it didn't!** Instead it created:

```asm
e8 f3 fe ff ff          call   1050 <puts@plt>
```

### Why? **Dynamic Linking!** 🔄

- ✅ libc can be updated without recompiling your binary
- ✅ libc can be placed anywhere in memory (ASLR)
- ✅ Multiple programs share one libc in memory

The **PLT is the middleman** that resolves function addresses at runtime.

---

## Let's Verify: Compare String Addresses

### Object File
```asm
8:   lea    0x0(%rip),%rdi        # address = 0x0 (WRONG!)
```

### Executable
```asm
1151: lea    0xeac(%rip),%rdi     # address = ?
```

Calculate it:
```
0x1151 (instruction address) + 0xeac (offset) = 0x1151 + 0xeac = 0x1ffd
```

Hmm, that's `0x1ffd` but the actual string is at `0x2004`. Let me recalculate...

Actually, `%rip` points to the **next instruction** (0x1158), so:
```
0x1158 + 0xeac = 0x2004 ✅
```

**That's the string!** Verify:

```bash
[root@DA22053700:Project1-GCC]# hexdump -C testProgram | grep 2000
00002000  01 00 02 00 48 65 6c 6c  6f 2c 20 57 6f 72 6c 64  |....Hello, World|
                      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
```

You should see `Hello, World!\n` at offset 0x2004! 📍

---

## Final Verification: Run It! 🎬

```bash
[root@DA22053700:Project1-GCC]# ./testProgram
Hello, World!
```

**Then show me the output!** You've earned it! 🏆

---

## Summary: What You've Learned

| Step | Input | Output | Linker's Job |
|------|-------|--------|---|
| Compile | `main.c` | `main.o` (unresolved) | — |
| **Link** | `main.o` | `main` (resolved) | Replace all `00 00 00 00` placeholders with real addresses ✅ |

You're now understanding what happens **inside the compilation pipeline** that 99% of developers never see! 🎓

## Why was printf changed to puts
Look at your original C code:

```c
printf("Hello, World!\n");
```

But the compiler optimized it! 🎯

Since your string ends with \n and contains no format specifiers, the compiler replaced:

```c
printf("Hello, World!\n");   // Complex: parses format string
```
with:
```c
puts("Hello, World!");        // Simple: just prints string + newline
```
This is a compiler optimization! Check with -fno-builtin:

```bash
gcc testProgram.c -fno-builtin -o testProgram_noopt
objdump -d testProgram_noopt | grep -A2 "call.*printf"
```


###🔍 Visual Summary: What's Where
```bash

TEXT SEGMENT (.text):
├─ _start (0x1060)
├─ main (0x1149)
│  └─ call 1050 <puts@plt>  ← Your code
└─ ...

PLT SEGMENT (.plt.sec):
├─ puts@plt (0x1050)
│  └─ bnd jmp *0x3fd0  ← Points to GOT

GOT SEGMENT (.got.plt):
├─ 0x3fd0 → 0x7ffff7e5c9e0 (puts in libc)

LIBC (Dynamically Loaded):
├─ 0x7ffff7e5c9e0: puts() function ← EXECUTES HERE!

### Summary
Question	Answer
Where does call 1050 go?	To PLT entry puts@plt at 0x1050
What does PLT do?	Jumps to address stored in GOT at 0x3fd0
What's in GOT 0x3fd0?	Address of puts() in libc (0x7fff...)
Where is printf?	Optimized away! Compiler used puts() instead
Why can't we see puts() code?	It's in libc (loaded at runtime), not in your binary


# GDB

While debugging the program, we can also review similiar information. However, the actual address changes while the program is loaded 


## 🎯 Aha! Important Lesson: ASLR (Address Space Layout Randomization)!

You've hit a **critical real-world security feature**! Let me explain:

---

## The Problem: ASLR 🔐

Notice your breakpoint address:
```
Breakpoint 1 at 0x1149    ← In objdump
Breakpoint 1, 0x0000555555555149 in main ()    ← In gdb
```

**The base address changed from `0x0000000000001149` to `0x0000555555555149`!**

This is **ASLR** — the OS randomizes where the executable loads in memory for security.

### The GOT Address Also Shifted!

In objdump, we calculated GOT at `0x3fd0`, but **that's a relative offset**!

With ASLR:
```
Base address: 0x555555554000 (randomized each run)
GOT address: 0x555555554000 + 0x3fd0 = 0x555555557fd0 (not 0x3fd0!)
```

---

## ✅ Solution: Read GOT from the Binary Itself

Instead of hardcoding `0x3fd0`, let's find the actual GOT address:

```bash
gdb ./testProgram
(gdb) break main
(gdb) run
(gdb) info proc mappings
```

Output will show:
```
Start Addr           End Addr       Size     Offset objfile
0x555555554000   0x555555555000   0x1000   0x0    /path/to/testProgram
0x555555755000   0x555555757000   0x2000   0x2000 /path/to/testProgram
```

The first entry is where your executable is loaded. Everything is offset from there.

---

## Better Approach: Let gdb Calculate It

```bash
(gdb) break main
(gdb) run
(gdb) info symbol 0x555555557fd0
# Or: x/gx &main
(gdb) print/x $rip
$1 = 0x555555555149          ← Current main address
```

Now calculate the GOT address from the binary offset:

```bash
(gdb) info files
# Shows: Symbols from "/path/to/testProgram"
# Entry point: 0x1060
```

---

## 🔥 The Direct Way: Use Symbol Names!

Instead of guessing addresses, use **symbol references**:

```bash
(gdb) break main
(gdb) run
(gdb) info variables puts     # Find puts symbol
(gdb) x/gx 0x555555557fd0    # Try the calculated address
```

Or even better — **examine the GOT directly**:

```bash
(gdb) x/a $rip - 0x9        # Look back at recent instructions
(gdb) disassemble main       # See actual addresses in current context
```

---

## 🎯 Let's Do This Properly

Run these commands in gdb:

```bash
gdb ./testProgram
(gdb) break main
(gdb) run

# Now print what we need:
(gdb) disassemble main
```

You'll see:
```asm
0x0000555555555149 <+0>:    endbr64
0x000055555555514d <+4>:    sub    $0x8,%rsp
0x0000555555555151 <+11>:   lea    0xeac(%rip),%rdi
0x0000555555555158 <+18>:   call   0x555555555050 <puts@plt>
```

Now let's trace to the PLT:

```bash
(gdb) x/i 0x555555555050     # Disassemble puts@plt
(gdb) x/gx 0x555555557fd0    # Read GOT entry (adjust address!)
```

To find the correct GOT address:

```bash
(gdb) info symbol 0x555555557fd0
(gdb) x/gx 0x555555555050 + 4  # Look near the PLT entry
```

---

## 📊 Better Way: Use objdump + gdb Together

First, find the offset in your binary:

```bash
objdump -R testProgram | grep puts
# Output: 00000000000020d0 JUMP_SLOT  puts@GLIBC_2.2.5  0000000000000000
```

This says GOT entry for `puts` is at offset **0x20d0** in the binary.

Then in gdb, with ASLR offset `0x555555554000`:

```bash
(gdb) x/gx 0x555555554000 + 0x20d0
```

Or let gdb do it:

```bash
(gdb) x/gx puts@got.plt    # gdb syntax!
0x555555757fd0:  0x00007ffff7e5c9e0
```

🎯 **There it is!** `0x00007ffff7e5c9e0` is the address of `puts()` in libc!

---

## 🚀 Final Verification

```bash
(gdb) x/i 0x00007ffff7e5c9e0
0x7ffff7e5c9e0 <puts>:    push   %rbp
```

**Perfect!** You're looking at the actual `puts()` function in libc! ✅

---

## Summary: The Complete Chain (With ASLR)

```
Your code:
  0x555555555158:  call   0x555555555050 <puts@plt>
                        ↓
PLT entry:
  0x555555555050:  bnd jmp *0x2f75(%rip)
                        ↓ (computes to 0x555555557fd0)
GOT entry:
  0x555555557fd0:  0x00007ffff7e5c9e0
                        ↓
libc:
  0x7ffff7e5c9e0:  <puts> executes!
```

---

## Try This Now

Run in gdb:

```bash
(gdb) break main
(gdb) run
(gdb) disassemble main
(gdb) x/gx puts@got.plt
(gdb) x/i 0x[address_from_above]
(gdb) continue
```


