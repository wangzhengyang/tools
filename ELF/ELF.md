- [format of Executable and Linkable Format(ELF) files](#format-of-executable-and-linkable-formatelf-files)
  - [Global file structure](#global-file-structure)
  - [Section vs segment](#section-vs-segment)
  - [ELF header](#elf-header)
  - [Section header table](#section-header-table)
  - [Sections](#sections)
    - [Index 0 section](#index-0-section)
    - [`.data` section](#data-section)
      - [`.text` section](#text-section)
    - [`SHT_STRTAB`](#sht_strtab)
    - [`.shstrtab`](#shstrtab)
    - [`.symtab`](#symtab)
      - [`STT_FILE`](#stt_file)
      - [`STT_SECTION`](#stt_section)
      - [`STT_NOTYPE`](#stt_notype)
        - [`SHN_ABS`](#shn_abs)
      - [`SHT_SYMTAB` on the executable](#sht_symtab-on-the-executable)
    - [`.strtab`](#strtab)
    - [`.rela.text`](#relatext)
      - [`.rela.text`](#relatext-1)
  - [Program header table](#program-header-table)

# format of Executable and Linkable Format(ELF) files

[reference:ELF Hello World Tutorial](https://cirosantilli.com/elf-hello-world)

## Global file structure

An ELF file contains the following parts:
* ELF header. Points to the position of the section header table and the program header table.
* Section header table(optional on executable). Each has `e_shnum` section headers, each pointing to the position of a section.
* N sections, with `N <= e_shnum`(optional on executable)
* Program header table(only on executable). Each has `e_phnum` program headers, each pointing to the position of a segment.
* N segments, with `N <= e_phnum`(only on executable)

The order of those parts is not fixed; the only fixed thing is the ELF header that must be the first thing on the file.

sample object file with three sections:

<pre><code>            
            +-------------------+
            | ELF header        |---+
+---------&gt; +-------------------+   | e_shoff
|           |                   |&lt;--+
| Section   | Section header 0  |
|           |                   |---+ sh_offset
| Header    +-------------------+   |
|           | Section header 1  |---|--+ sh_offset
| Table     +-------------------+   |  |
|           | Section header 2  |---|--|--+
+---------&gt; +-------------------+   |  |  |
            | Section 0         |&lt;--+  |  |
            +-------------------+      |  | sh_offset
            | Section 1         |&lt;-----+  |
            +-------------------+         |
            | Section 2         |&lt;--------+
            +-------------------+
</code></pre>

![](img/ELF_Executable_and_Linkable_Format_diagram_by_Ange_Albertini.png)

## Section vs segment

* section: exists before linking, in object files.

One or more sections will be put inside a single segment by the linker.

* segment: exists after linking, in the executable file.

Contains information about how each segment should be loaded into memory by the OS, notably locaiton and permissions.

## ELF header
<pre><code>
readelf -h hello_world.o
</code></pre>

outputs:

<pre><code>
Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
Class:                             ELF64
Data:                              2's complement, little endian
Version:                           1 (current)
OS/ABI:                            UNIX - System V
ABI Version:                       0
Type:                              REL (Relocatable file)
Machine:                           Advanced Micro Devices X86-64
Version:                           0x1
Entry point address:               0x0
Start of program headers:          0 (bytes into file)
Start of section headers:          64 (bytes into file)
Flags:                             0x0
Size of this header:               64 (bytes)
Size of program headers:           0 (bytes)
Number of program headers:         0
Size of section headers:           64 (bytes)
Number of section headers:         7
Section header string table index: 3
</code></pre>

<pre><code>
readelf -h hello_world.out
</code></pre>

outputs:

<pre><code>
Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
Class:                             ELF64
Data:                              2's complement, little endian
Version:                           1 (current)
OS/ABI:                            UNIX - System V
ABI Version:                       0
Type:                              EXEC (Executable file)
Machine:                           Advanced Micro Devices X86-64
Version:                           0x1
Entry point address:               0x4000b0
Start of program headers:          64 (bytes into file)
Start of section headers:          272 (bytes into file)
Flags:                             0x0
Size of this header:               64 (bytes)
Size of program headers:           56 (bytes)
Number of program headers:         2
Size of section headers:           64 (bytes)
Number of section headers:         6
Section header string table index: 3
</code></pre>

bytes in the object file:

<pre><code>
00000000  7f 45 4c 46 02 01 01 00  00 00 00 00 00 00 00 00  |.ELF............|
00000010  01 00 3e 00 01 00 00 00  00 00 00 00 00 00 00 00  |..&gt;.............|
00000020  00 00 00 00 00 00 00 00  40 00 00 00 00 00 00 00  |........@.......|
00000030  00 00 00 00 40 00 00 00  00 00 40 00 07 00 03 00  |....@.....@.....|
</code></pre>

executable:

<pre><code>
00000000  7f 45 4c 46 02 01 01 00  00 00 00 00 00 00 00 00  |.ELF............|
00000010  02 00 3e 00 01 00 00 00  b0 00 40 00 00 00 00 00  |..&gt;.......@.....|
00000020  40 00 00 00 00 00 00 00  10 01 00 00 00 00 00 00  |@...............|
00000030  00 00 00 00 40 00 38 00  02 00 40 00 06 00 03 00  |....@.8...@.....|
</code></pre>

structure represented:

```c
# define EI_NIDENT 16

typedef struct {
    //This array of bytes specifies how to interpret the file. more details see `man elf`.
    unsigned char   e_ident[EI_NIDENT];
    //This member of the structure identifies the object file type.
    Elf64_Half      e_type;
    //This member specifies the required architecture for an individual file.
    Elf64_Half      e_machine;
    //This member identifies the file version.
    Elf64_Word      e_version;
    //This member gives the virtual address to which the system first transfers control, thus starting the process.
    Elf64_Addr      e_entry;
    //This member holds the program header table's file offset in bytes.
    Elf64_Off       e_phoff;
    //This member holds the section header table's file offset in bytes.
    Elf64_Off       e_shoff;
    //This member holds processor-specific flags associated with the file.
    Elf64_Word      e_flags;
    //This member holds the ELF header's size in bytes.
    Elf64_Half      e_ehsize;
    //This member holds the size in bytes of one entry in the file's program header table.
    Elf64_Half      e_phentsize;
    //This member holds the number of entries in the program header table.
    Elf64_Half      e_phnum;
    //This member holds a sections header's size in bytes.
    Elf64_Half      e_shentsize;
    //This member holds the number of entries in the sectin header table.
    Elf64_Half      e_shnum;
    //This member holds the section header table index of the entry associated with the section name string table.
    Elf64_Half      e_shstrndx;
} Elf64_Ehdr;
```

manual breakdown:
* 0 0:`EI_MAG` = `7f 45 4c 46` = `0x7f 'E', 'L', 'F'`:ELF magic number
* 0 4:`EI_FLAGS` = `02` = `ELFCLASS64`:64 bit elf
* 0 5:`EI_DATA` = `01` = `ELFDATA2LSB`:little endian data
* 0 6:`EI_VERSION` = `01`:format version
* 0 7:`EI_OSABI`(only in 2003 Update) = `00` = `ELFOSABI_NONE`:no extensionszheg
* 0 8:`EI_PAD` = 8x`00`:reserved bytes. Must be set to 0
* 1 0:`e_type` = `01 00` = 1(big endian) = `ET_RE1`:relocatable format
on the executable it is `02 00` for `ET_EXEC`
another important possibility for the executable is `ET_DYN` for PIE executables and shared libraries. `ET_DYN` tells the Linux kernel that the code is position independent, and can loaded at a random memory location with ALSR.
* 1 2:`e_machine` = `3e 00` = `62` = `EM_X86_64`:AMD64 architecture
* 1 4:`e_version` = `01 00 00 00`:must be 1
* 1 8:`e_entry` = 8x `00`:execution address entry point, or 0 if not applicable like for the object file since there is no entry point.
on the executable, it it `b0 00 40 00 00 00 00 00`. The kernel puts the `RIP` directly on that value when executing. 
* 2 0:`e_phoff` = 8x `00`:program header table offset, 0 if not present
`40 00 00 00` on the executable, i.e. it starts immediately after the ELF header.
* 2 8:`e_shoff` = `40` 7x `00`:section header table file offset, 0 if not present.
* 3 0:`e_flags` = `00 00 00 00` Arch specific.
* 3 4:`e_ehsize` = `40 00`:size of this elf header.
* 3 6:`e_phentsize` = `00 00`:size of each program header, 0 if not present.
`38 00` on executable:it is 56 bytes long
* 3 8:`e_phnum` = `00 00`:number of program header entries, 0 if not present.
`02 00` on executable: there are 2 entries
* 3 A:`e_shentsize` and `e_shnum` = `40 00 07 00`:section header size and number of entries
* 3 E:`e_shstrndx`(Section Header STRing iNDex) = `03 00`:index of the `.shstrtab` section.

## Section header table

Array of `Elf64_Shdr` structs.

```c
typedef struct {
    Elf64_Word  sh_name;
    Elf64_Word  sh_type;
    Elf64_Xword sh_flags;
    Elf64_Addr  sh_addr;
    Elf64_Off   sh_offset;
    Elf64_Xword sh_size;
    Elf64_Word  sh_link;
    Elf64_Word  sh_info;
    Elf64_Xword sh_addralign;
    Elf64_Xword sh_entsize;
} Elf64_Shdr;
```

Each entry contains metadata about a given section.
`e_shoff` of the ELF header gives the starting position, 0x40 here.
`e_shentsize` and `e_shnum` from the ELF header say that we have 7 entries, each `0x40` bytes long.
So the table takes bytes from 0x40 to `0x40 + 7 * 0x40 - 1` = 0x1FF.

<pre><code>
readelf -S hello_world.o
</code></pre>

outputs:

<pre><code>
There are 7 section headers, starting at offset 0x40:

Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .data             PROGBITS         0000000000000000  00000200
       000000000000000d  0000000000000000  WA       0     0     4
  [ 2] .text             PROGBITS         0000000000000000  00000210
       0000000000000027  0000000000000000  AX       0     0     16
  [ 3] .shstrtab         STRTAB           0000000000000000  00000240
       0000000000000032  0000000000000000           0     0     1
  [ 4] .symtab           SYMTAB           0000000000000000  00000280
       00000000000000a8  0000000000000018           5     6     4
  [ 5] .strtab           STRTAB           0000000000000000  00000330
       0000000000000034  0000000000000000           0     0     1
  [ 6] .rela.text        RELA             0000000000000000  00000370
       0000000000000018  0000000000000018           4     2     4
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), l (large)
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)
  O (extra OS processing required) o (OS specific), p (processor specific)
  </code></pre>

## Sections

### Index 0 section

contained in bytes 0x40 to 0x7F.

The first section is always magic, in index 0, `SHT_NULL` is mandatory.

### `.data` section

`.data` is section 1:

<pre><code>
00000080  01 00 00 00 01 00 00 00  03 00 00 00 00 00 00 00  |................|
00000090  00 00 00 00 00 00 00 00  00 02 00 00 00 00 00 00  |................|
000000a0  0d 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
000000b0  04 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
</code></pre>

* 80 0:`sh_name` = `01 00 00 00`:index 1 in the `.shstrtab` string table
`1` says the name of this section starts at the first charactor of that section, and ends at the first NULL charactor, making up the string `.data`.
* 80 4:`sh_type` = `01 00 00 00`:`SHT_PROGBITS`:the section content is not specified by ELF, only by how the program interprets it. Normal since a `.data` section.
* 80 8:`sh_flags`:`03` 7x `00`:`SHF_WRITE` and `SHF_ALLOC`.
* 90 0:`sh_addr` = 8x `00`:TODO. the member always contains 0.
* 90 8:`sh_offset` = `00 02 00 00 00 00 00 00` = `0x200`:number of bytes from the start of the program to the first byte in this section.
* a0 0:`sh_size` = `0d 00 00 00 00 00 00 00`
if we take 0xD bytes starting at `sh_offset` 200, we see:
<pre><code>
00000200  48 65 6c 6c 6f 20 77 6f  72 6c 64 21 0a 00        |Hello world!..  |
</code></pre>

once we graduate from `hd`, we will look this up like:

<pre><code>
readelf -x .data hello_world.o
</code></pre>

which outputs:

<pre><code>
Hex dump of section '.data':
  0x00000000 48656c6c 6f20776f 726c6421 0a       Hello world!.
</code></pre>

#### `.text` section

use the `readelf -S` to see the `.text` sections:

<pre><code>  
[Nr] Name              Type             Address           Offset
     Size              EntSize          Flags  Link  Info  Align
[ 2] .text             PROGBITS         0000000000000000  00000210
     0000000000000027  0000000000000000  AX       0     0     16
</code></pre>

`.text` is executable but not writable.

<pre><code>
objdump -d hello_world.o
</code></pre>

gives:

<pre><code>
hello_world.o:     file format elf64-x86-64

Disassembly of section .text:

0000000000000000 &lt;_start&gt;:
   0:       b8 01 00 00 00          mov    $0x1,%eax
   5:       bf 01 00 00 00          mov    $0x1,%edi
   a:       48 be 00 00 00 00 00    movabs $0x0,%rsi
  11:       00 00 00
  14:       ba 0d 00 00 00          mov    $0xd,%edx
  19:       0f 05                   syscall
  1b:       b8 3c 00 00 00          mov    $0x3c,%eax
  20:       bf 00 00 00 00          mov    $0x0,%edi
  25:       0f 05                   syscall
</code></pre>

The most interesting part is line `a` which does:

```c
movabs $0x0, %rsi
```

to pass the address of the string to the system call. Currently, the `0x0` is just a placeholder. After linking happens, it will be modified to contain:

<pre><code>
4000ba: 48 be d8 00 60 00 00    movabs $0x6000d8,%rsi
</code></pre>

this modification is possible because of the data of the `.rela.text` section.

### `SHT_STRTAB`

sections with `sh_type == SHT_STRTAB` are called string tables.

they hold a null separated array of strings.

such sections are used by other sections when string names are to be used. The using section says:
* which string table they are using
* what is the index on the target string table where the string starts

for example, we could have a string table containing:

<pre><code>
Data: \0 a b c \0 d e f \0
Index: 0 1 2 3  4 5 6 7  8
</code></pre>

the first byte must be a 0. And if another section wants to use the string `d e f`, they have to point to index 5 of this section(letter `d`).

notable string table sections:
* `.shstrtab`
* `.strtab`

### `.shstrtab`

section type:`sh_type == SHT_STRTAB`.

common name:"section header string table".

this section gets pointed to by the `e_shstrnd` field of the ELF header itself.

string indexes of this section are pointed by the `sh_name` field of section headers, which denote strings.

this section does not have `SHF_ALLOC` marked, so it will not appear on the executing program.

<pre><code>
readelf -x .shstrtab hello_world.o
</code></pre>

outputs:

<pre><code>
Hex dump of section '.shstrtab':
  0x00000000 002e6461 7461002e 74657874 002e7368 ..data..text..sh
  0x00000010 73747274 6162002e 73796d74 6162002e strtab..symtab..
  0x00000020 73747274 6162002e 72656c61 2e746578 strtab..rela.tex
  0x00000030 7400                                t.
</code></pre>

### `.symtab`

section type:`sh_type == SHT_SYMTAB`.

common name:"symbol table".

first the we note that:
* `sh_link` = `5`
* `sh_info` = `6`

for `SHT_SYMTAB` sections, those numbers mean that:
* strings that give symbol names are in section 5, `.strtab`
* the relocation data is in section 6, `.rela.text`

a good high level tool to disassemble that section is:
<pre><code>
nm hello_world.o
</code></pre>

which gives:

<pre><code>
0000000000000000 T _start
0000000000000000 d hello_world
000000000000000d a hello_world_len
</code></pre>

a more detailed disassembly can be obtained with:
<pre><code>
readelf -s hello_world.o
</code></pre>

which gives:

<pre><code>
Symbol table '.symtab' contains 7 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS hello_world.asm
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    2
     4: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT    1 hello_world
     5: 000000000000000d     0 NOTYPE  LOCAL  DEFAULT  ABS hello_world_len
     6: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT    2 _start
</code></pre>

the data is:

<pre><code>
readelf -x .symtab hello_world.o
</code></pre>

which gives:

<pre><code>
Hex dump of section '.symtab':
  0x00000000 00000000 00000000 00000000 00000000 ................
  0x00000010 00000000 00000000 01000000 0400f1ff ................
  0x00000020 00000000 00000000 00000000 00000000 ................
  0x00000030 00000000 03000100 00000000 00000000 ................
  0x00000040 00000000 00000000 00000000 03000200 ................
  0x00000050 00000000 00000000 00000000 00000000 ................
  0x00000060 11000000 00000100 00000000 00000000 ................
  0x00000070 00000000 00000000 1d000000 0000f1ff ................
  0x00000080 0d000000 00000000 00000000 00000000 ................
  0x00000090 2d000000 10000200 00000000 00000000 -...............
  0x000000a0 00000000 00000000                   ........
</code></pre>

then entries are of type:

```c
typedef struct {
     Elf64_Word st_name;
     unsigned char st_info;
     unsigned char st_other;
     Elf64_Half st_shndx;
     Elf64_Addr     st_value;
     Elf64_Xword    st_size;
}Elf64_Sym;
```

like in the section table, the first entry is magical and set to a fixed meaningless values.

#### `STT_FILE`

entry 1 has `ELF64_R_TYPE == STT_FILE.ELF64_R_TYPE` is continued inside of `st_info`.

Byte analysis:
* 10 8:`st_name` = `01000000` = character 1 in the `.strtab`, which until the following `\0` makes `hello_world.asm`

This piece of information file may be used by the linker to decide on which segment sections go:e.g. in `ld` linker script we write:

<pre><code>
segment_name :
{
    file(section)
}
</code></pre>

to pick a section from a given file:

most of the time however, we will just dump all sections with a given name together with:

<pre><code>
segment_name :
{
    *(section)
}
</code></pre>

* 10 12:`st_info` = 04

Bits 0-3 = `ELF64_R_TYPE` = Type = `4` = `STT_FILE`: the main purpose of this entry is to use `st_name` to indicate the name of the file which generated this object file.

Bits 4-7 = `ELF64_ST_BIND` = binding = `0` = `STB_LOCAL`. Required value for `STT_FILE`.

* 10 13:`st_shndx` = Symbol Table Section header index = `f1ff` = `SHN_ABS`. Required for `STT_FILE`.
* 20 0:`st_value`= 8x `00`:required for value for `STT_FILE`
* 20 8:`st_size` = 8x `00`:no allocated size

#### `STT_SECTION`

there are two such entries, one pointing to `.data` and the other to `.text`(section indexes 1 and 2).

<pre><code>
Num:    Value          Size Type    Bind   Vis      Ndx Name
  2: 0000000000000000     0 SECTION LOCAL  DEFAULT    1
  3: 0000000000000000     0 SECTION LOCAL  DEFAULT    2
</code></pre>

#### `STT_NOTYPE`

then come the most important symbols:

<pre><code>
Num:    Value          Size Type    Bind   Vis      Ndx Name
  4: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT    1 hello_world
  5: 000000000000000d     0 NOTYPE  LOCAL  DEFAULT  ABS hello_world_len
  6: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT    2 _start
</code></pre>

`hello_world` string is in the `.data` section(index 1). It's value is 0:it points to the first byte of that seciton.

`_start` is marked with `GLOBAL` visibility since we wrote:

<pre><code>
global _start
</code></pre>

##### `SHN_ABS`

`hello_world_len` points to the special `st_shndex == SHN_ABS == 0xF1FF`.

`0xF1FF` is chosen so as to not conflict with other sections.

`st_value == 0xD == 13` which is the value we have stored there on the assembly:the length of the string `Hello World!`.

this means that relocation will no affect this value:it is a constant.

#### `SHT_SYMTAB` on the executable

be default, NASM places a .symtab on the ececutable as well.

### `.strtab`

holding strings for the symbol table.

this section has `sh_type == SHT_STRTAB`.

it is pointed to by `sh_link == 5` of the `symtabl` section.

<pre><code>
readelf -x .strtab hello_world.o
</code></pre>

outputs:

<pre><code>
Hex dump of section '.strtab':
  0x00000000 0068656c 6c6f5f77 6f726c64 2e61736d .hello_world.asm
  0x00000010 0068656c 6c6f5f77 6f726c64 0068656c .hello_world.hel
  0x00000020 6c6f5f77 6f726c64 5f6c656e 005f7374 lo_world_len._st
  0x00000030 61727400                            art.
</code></pre>

this implies that it is an ELF level limitation that global variables cannot contain NUL characters.

### `.rela.text`

section type:`sh_type == SHT_RELA`.

common name:"relocation section".

`.rela.text` holds relocation data which says how the address should be modified when the final executable is linked. This points to bytes of the text area that must be modified when linking happens to point to the correct memory locations.

basically, it translates the object text containing the placeholder 0x0 address:

<pre><code>
   a:       48 be 00 00 00 00 00    movabs $0x0,%rsi
  11:       00 00 00
</code></pre>

to the local executable code containing the final 0x6000d8:

<pre><code>
4000ba: 48 be d8 00 60 00 00    movabs $0x6000d8,%rsi
4000c1: 00 00 00
</code></pre>

it was ponited to by `sh_info = 6` of the `.symtab` section.

<code>
readelf -r hello_world.o
</code>

outputs:

<pre><code>
Relocation section '.rela.text' at offset 0x3b0 contains 1 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
00000000000c  000200000001 R_X86_64_64       0000000000000000 .data + 0
</code></pre>

the section does not exist in the executable.

the actual bytes are:

<pre><code>
00000370  0c 00 00 00 00 00 00 00  01 00 00 00 02 00 00 00  |................|
00000380  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|</code></pre>

the struct represented is:

```c
typedef struct{
     Elf64_Addr r_offset;
     Elf64_Xword r_info;
     Elf64_Sxword r_addend;
}Elf64_Rela;
```

so:
* 370 0:`r_offset` = 0xC:address into the `.text` whose address this relocation will modify
* 370 8:`r_info` = 0x20000001. Contains 2 fileds:
  * `ELF64_R_TYPE` = 0x1: meaning depends on the exact architecture.
  * `ELF64_R_SYM` = 0x2: index of the section to which the address points, so `.data` which is at index 2.
  The AMD64 ABI says that type `1` is called R_X86_64_64` and that it represents the operation `S + A` where:
  * `S`:the value of the symbol on the object file, here `0` because we point to the `00 00 00 00 00 00 00 00` of `movabs $0x0, %rsi`
  * `A`: the addend, present in field `r_added`
  this address is added to the section on which the relocation operates.
  this relocation operation acts on a total 8 bytes.
* 380 0:`r_addend` = 0

so in this example we conclude that the new address will be:`S + A` = `.data + 0`, and thus the first thing in the data section.

#### `.rela.text`

besides `sh_type == SHT_RELA`, there also exists `SHT_REL`, which would have section name `.text.rel`(not present in this object file)

those represent the same `struct`, but without the addend, e.g:

```c
typedef struct{
     Elf64_Addr r_offset;
     Elf64_Xword r_info;
}Elf64_Rela;
```

the ELF standard syas that in many cases the both can be used, and it it just a matter of convenience.

## Program header table

only appears in the executable.

contains information of how the executable should be put into the process virtual memory.

the executable is generated from object files by the linker. the main jobs that the linker does are:
* determine which sections of the object files will go into which segments of the executable. In Binutils, this comes down to parsing a linker script, and dealing with a bunch of defaults.
you can get the linker script used with `ld --verbose`, and set a custom one with `ld -T`.

* do relocation according to the `.rela.text` section. this depends on how the multiple sections are put into memory.

<code>
readelf -l hello_world.out
</code>

gives:

<pre><code>
Elf file type is EXEC (Executable file)
Entry point 0x4000b0
There are 2 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x00000000000000d7 0x00000000000000d7  R E    200000
  LOAD           0x00000000000000d8 0x00000000006000d8 0x00000000006000d8
                 0x000000000000000d 0x000000000000000d  RW     200000

 Section to Segment mapping:
  Segment Sections...
   00     .text
   01     .data
</code></pre>

on the ELF header, `e_phoff`, `e_phnum` and `e_phentszie` told us that there are 2 program headers, which start at 0x40 and are 0x38 bytes long each, so they are:

<pre><code>
00000040  01 00 00 00 05 00 00 00  00 00 00 00 00 00 00 00  |................|
00000050  00 00 40 00 00 00 00 00  00 00 40 00 00 00 00 00  |..@.......@.....|
00000060  d7 00 00 00 00 00 00 00  d7 00 00 00 00 00 00 00  |................|
00000070  00 00 20 00 00 00 00 00                           |.. .....        |
</code></pre>

and:

<pre><code>
00000070                           01 00 00 00 06 00 00 00  |        ........|
00000080  d8 00 00 00 00 00 00 00  d8 00 60 00 00 00 00 00  |..........`.....|
00000090  d8 00 60 00 00 00 00 00  0d 00 00 00 00 00 00 00  |..`.............|
000000a0  0d 00 00 00 00 00 00 00  00 00 20 00 00 00 00 00  |.......... .....|
</code></pre>

structure represented:
```c
typedef struct{
     Elf64_Word p_type;
     Elf64_Word p_flags;
     Elf64_Off p_offset;
     Elf64_Addr p_vaddr;
     Elf64_Addr p_paddr;
     Elf64_Xword p_filesz;
     Elf64_Xword p_memsz;
     Elf64_Xword p_align;
}Elf64_Phdr;
```

breakdown of the first one:
* 40 0:`p_type` = `01 00 00 00` = `PT_LOAD`:this is a regular segment that will get loaded in memory.
* 40 4:`p_flags` = `05 00 00 00` = execute and read permissions.
* 40 8:`p_offset` = 8x `00`: this member gives the offset from the begining of the file at which the first byte of the segment resides.
* 50 0:`p_vaddr` = `00 00 40 00 00 00 00 00`:initial virtual memory address to load this segemnt to
* 50 8:`p_paddr` = `00 00 40 00 00 00 00 00`:unspecified effect.
* 60 0:`p_filesz` = `d7 00 00 00 00 00 00 00`:size that the segment occupies in memory. If smaller than `p_memsz`, the OS fills it with zeroes to fit when loading the program.
* 60 8:`p_memsz` = `d7 00 00 00 00 00 00 00`:size that the segment occupies in memory
* 70 0:`p_align` = `00 00 20 00 00 00 00 00`:0 or 1 mean no alignment required.

the second segment(`.data`) is analogous

then the:
<pre><code>
Section to Segment mapping:
</code></pre>

section of the `readelf` tells us that:
* 0 is the `.text` segment
* 1 is the `.data` segment






