# 🎯 nm Command: Symbol Table Inspector

The `nm` command **displays symbols from object files and libraries**.

---

## Basic Syntax

```bash
nm [options] file
```

---

## What is a Symbol?

A **symbol** is a name that represents:
- 🔵 A function
- 🔵 A global variable
- 🔵 A static variable
- 🔵 An external reference

---

## Symbol Format

```bash
nm testLib.o
```

Output:
```
0000000000000000 T libFuncPrint
                 U printf
```

### Breaking It Down

```
0000000000000000  T  libFuncPrint
│                 │  │
│                 │  └─ Symbol name
│                 └───── Symbol type
└───────────────────── Memory address (in this file)
```

---

## Symbol Types (The Most Important Part!)

| Type | Meaning | Defined? | Usage |
|---|---|---|---|
| **T** | Text (code) - Global | ✅ YES | Defined in this file, exported |
| **t** | Text (code) - Local | ✅ YES | Defined locally, not exported |
| **D** | Data - Initialized | ✅ YES | Global variable with initial value |
| **d** | Data - Local | ✅ YES | Local variable (static) |
| **B** | BSS (uninitialized) | ✅ YES | Global variable, no initial value |
| **b** | BSS - Local | ✅ YES | Local uninitialized variable |
| **U** | Undefined | ❌ NO | Needs to be linked from elsewhere |
| **R** | Read-only data | ✅ YES | Constants, strings |
| **N** | Debugging symbol | ❓ SPECIAL | Debug info |
| **W** | Weak symbol | ⚠️ MAYBE | Can be overridden |

---

## Real-World Examples

### Example 1: Object File (testLib.o)

```bash
nm testLib.o
```

Output:
```
0000000000000000 T libFuncPrint
                 U printf
```

**What this means:**
- ✅ `libFuncPrint` is **defined** (T = text/code) in this file
- ❌ `printf` is **undefined** (U) — needs to be linked from libc

---

### Example 2: Static Library (libtestLib.a)

```bash
nm libtestLib.a
```

Output:
```

testLib.o:
0000000000000000 T libFuncPrint
                 U printf
```

**What this means:**
- Shows symbols from **all objects inside** the archive

---

### Example 3: Dynamic Library (libtestLib.so)

```bash
nm libtestLib.so
```

Output:
```
0000000000001109 T libFuncPrint
0000000000001100 T printf@plt
                 U printf
```

**What this means:**
- ✅ `libFuncPrint` is defined (address 0x1109)
- 🔄 `printf@plt` is a **PLT entry** (trampoline for dynamic linking)
- ❌ `printf` is still undefined (will be resolved from libc at runtime)

---

### Example 4: Executable (testProgram_static)

```bash
nm testProgram_static
```

Output (abbreviated):
```
0000000000001060 T _start
0000000000001149 T main
0000000000001109 T libFuncPrint
0000000000007e5c T printf
...
```

**What this means:**
- ✅ **All symbols are DEFINED** (T type)
- No `U` (undefined) symbols!
- Everything was statically linked ✅

---

### Example 5: Executable (testProgram_dynamic)

```bash
nm testProgram_dynamic
```

Output (abbreviated):
```
0000000000001060 T _start
0000000000001149 T main
                 U libFuncPrint
0000000000001050 T libFuncPrint@plt
                 U printf
```

**What this means:**
- ❌ `libFuncPrint` is undefined (U)
- 🔄 `libFuncPrint@plt` is the PLT trampoline
- ❌ `printf` is undefined
- Both will be resolved at **runtime** 🔄

---

## Useful nm Options

### `-t` : Display Symbol Addresses

```bash
nm -t d testLib.o  # Show addresses in decimal
nm -t x testLib.o  # Show addresses in hexadecimal
```

### `-g` : Show Only Global Symbols

```bash
nm -g testLib.o
# Output: Only exported symbols, hides internal ones
```

### `-u` : Show Only Undefined Symbols

```bash
nm -u testLib.o
# Output:
#                  U printf
```

**Use case:** Find what a library depends on!

### `-D` : Show Dynamic Symbols (for .so files)

```bash
nm -D libtestLib.so
# Shows symbols visible at runtime
```

### `-C` : Demangle C++ Names

```bash
nm -C libstdc++.so.6
# Decodes C++ mangled names to readable form
```

### `-j` : Just Show Symbol Names (no addresses/types)

```bash
nm -j testLib.o
# Output:
# libFuncPrint
# printf
```

### `-r` : Sort in Reverse Order

```bash
nm -r testLib.o
```

### `-s` : Print Sections Used

```bash
nm -s testLib.o
```

---

## 🎯 Practical Examples

### Find All Undefined Symbols (Dependencies)

```bash
nm -u testProgram_dynamic
# Shows everything your program needs to link
```

### Compare Static vs Dynamic

```bash
echo "=== STATIC ===" && \
nm testProgram_static | grep -c "^[0-9a-f]"

echo "=== DYNAMIC ===" && \
nm testProgram_dynamic | grep -c "^[0-9a-f]"
# Static will have MORE symbols (library code included)
# Dynamic will have FEWER (symbols referenced from .so)
```

### Find Symbol in Library

```bash
nm libc.so.6 | grep " T printf"
# Shows: 000000000007e5c0 T printf
```

### Show What a Library Exports

```bash
nm -D libtestLib.so
# Only shows symbols visible to users of the library
```

---

## nm vs objdump

| Command | Purpose | Output |
|---|---|---|
| `nm` | **Show symbols** (names, addresses, types) | Clean, easy to read |
| `objdump -t` | **Show symbol table** (same as nm but more detailed) | More verbose |
| `objdump -d` | **Disassemble** (show assembly code) | Low-level code |

---

## 🎯 Complete Example: Trace a Symbol

```bash
# 1. Where is libFuncPrint defined?
nm testLib.o
# Output: 0000000000000000 T libFuncPrint

# 2. Is it in the library?
nm libtestLib.a | grep libFuncPrint
# Output: 0000000000000000 T libFuncPrint

# 3. Where does testProgram reference it?
nm testProgram_dynamic | grep libFuncPrint
# Output:                  U libFuncPrint (undefined - needs to link)

# 4. After static linking, is it defined?
nm testProgram_static | grep libFuncPrint
# Output: 0000000000001109 T libFuncPrint (now defined!)
```

---

## Summary Table

| Task | Command |
|---|---|
| See all symbols in a file | `nm file` |
| See only undefined symbols | `nm -u file` |
| See only global symbols | `nm -g file` |
| See symbols in a library | `nm libtestLib.a` |
| See exported symbols (.so) | `nm -D libtestLib.so` |
| Find where printf is defined | `nm libc.so.6 \| grep printf` |
| Compare static vs dynamic | `nm testProgram_static \| wc -l` vs `nm testProgram_dynamic \| wc -l` |

---

## Try These Commands NOW

```bash
# 1. See symbols in object file
nm testLib.o

# 2. See symbols in static library
nm libtestLib.a

# 3. See symbols in dynamic library
nm -D libtestLib.so

# 4. Compare static executable
nm testProgram_static | grep libFuncPrint

# 5. Compare dynamic executable
nm testProgram_dynamic | grep libFuncPrint

# 6. Find all undefined symbols in dynamic version
nm -u testProgram_dynamic

# 7. Find where printf really is
nm libc.so.6 | grep " T printf"
```

**Run these and show me the output!** 🎯
