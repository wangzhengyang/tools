- [GCC Inline Assembley](#gcc-inline-assembley)
  - [GCC Assembler Syntax](#gcc-assembler-syntax)
  - [Basic Inline](#basic-inline)
  - [Extended Asm](#extended-asm)
    - [Assembler Template](#assembler-template)
    - [Operands](#operands)
    - [Clobber List](#clobber-list)
    - [Volatile](#volatile)
  - [More about constraints](#more-about-constraints)
    - [Commonly used constraints](#commonly-used-constraints)
    - [Constraint Modifiers](#constraint-modifiers)
# GCC Inline Assembley
## GCC Assembler Syntax
GCC, the GNU C Complier for Linux, uses AT&T/UNIX assembly syntax.

1. Source-Destination Order

The direction of the operands in AT&T syntax is opposite to that of Intel. In Intel syntax the first operands is the destination, and the second operand is the source whereas in AT&T syntax the first operand is the source and the second operand is the destination.

"Op-code dst src" in Intel syntax changes to
"Op-code src dst" in AT&T syntax.

2. Register Naming

Register names are prefixed by % ie, if eax is to be used, write %eax.

3. Immediate Operand

AT&T immediate operands are preceded by '$'. For static "C" variables alse prefix a '$'. In Intel syntax, for hexadecimal constants an 'h' is suffixed, instead of that, here we prefix '0x' to the constant. So, for hexadecimals, we first see a '$', then '0x' and finally the constants.

4. Operand Size

In AT&T syntax the size of memory operands is detemined from the last charactor of the op-code name, Op-code suffixes of 'b','w',and'l' specify byte(8-bit),word(16-bit),and long(32-bit) memory references. Intel syntax accomplishes this by prefixing memory operands (not the op-codes) with 'byte ptr','word ptr',and 'dword ptr'.

Thus, Intel "mov al, byte ptr foo" is "movb foo, %al" in AT&T syntax.

5. Memory Operands

In Intel syntax the base register is enclosed in '[' and ']' where as in AT&T thery change to '(' and ')'. Additionally, in Intel syntax an indirect memory reference is like

section:[base + index * scale + disp], which changes to

section:disp(base, index, scale) in AT&T.

One point to bear in mind is that, when a constant is used for disp/scale, '$' shoudn't be prefixed.

|Intel Code|AT&T Code|
|-|-|
|mov eax, 1|movl $1, %eax|
|mov ebx, 0ffh|movl $0xff, %ebx|
|int 80h|int $0x80|
|mov ebx, eax|movl %eax, %ebx|
|mov eax, [ecx]|movl (%ecx), %eax|
|mov eax, [ebx + 3]| movl 3(%ebx), %eax|
|mov eax, [ebx + 20h]|movl 0x20(%ebx), %eax|
|add eax, [ebx + ecx * 2h]|movl (%ebx, %ecx $0x2), %eax|
|lea eax, [ebx + ecx]|lea (%ebx, %ecx), %eax|
|sub eax, [ebx + ecx * 4h - 20h]|sub -0x20(%ebx, %ecx, $0x4), %eax|

## Basic Inline
The format of basic inline assembly is very much straight forward. It's basic format is

```c
asm("assembly code");
```

Example:
```c
asm("movl %ecx, %eax") /*moves the contents of ecx to eax*/
```

```c
__asm__("movb %bh (%eax)") /*moves the byte from the bh to the memory pointed by eax*/
```

We can use `__asm__` if the keyword `asm` conflicts with something in our program. If we have more than one instructions, we write one per line in double quotes, and also suffix a '\n' and '\t' to the instruction. This is because gcc sends each instruction as a string to **as**(GAS) and by using the newline/tab we send correctly formatted lines to the assembler.

Example:

```c
__asm__("movl %eax, %ebx\n\t"
        "movl $56, %esi\n\t"
        "movl %ecx, $label(%edx, %ebx, $4)\n\t"
        "movb %ah, (%ebx)");
```

## Extended Asm

If in our code we touch(ie, change the contents) some registers and return from asm without fixing those changes, something bad is going to happen. This is where we want some extended functionality. Extended asm provides us with that functionality.

In extended assembly, we can also specify the operands. It allows us to specify the input registers, output registers and a list of clobbered registers. It is not mandatory to specify the registers to use, we can leave that head ache to GCC and that probably fit into GCC's optimization  scheme better. 

```c
asm(assembler template
   :output operands /*optinal*/
   :input operands  /*optional*/
   :list of clobbered registers /*optional*/
   );
```

The assembler template consists of assembly instructions. Each operand is describled by an operand-constraint string followed by the C expression in parentheses. A colon separates the assembler template from the first output operand and another separates the last output operand from the first input, if any. Commas separate the operands within each group. The total number of operands is limited to ten or to the maximum number of operands in any instruction pattern in the machine description, whichever is greater.

If there are no output operands but there are input registers, you must place two consecutive colons surrounding the place where the output operands whould go.

Example:

```c
asm("cld\n\t"
    "rep\n\t"
    "stosl"
    :/*no output registers*/
    :"c"(count), "a"(fill_value), "D"(dest)
    :"%ecx", "%edi"
    );
```

Let us see one more example to make things more clearer.

```c
int a=10, b;
ams("movl %1, %%eax;
     movl %%eax, %0;"
    :"=r"(b) /*output*/
    :"r"(a)  /*input*/
    :"%eax"  /*clobbered register*/)
```

Some points of interest are:
* "b" is the output operand, referred to by %0 and "a" is the input operand, referred to by %1.
* "r" is a constraint on the operands. "r" says to GCC to use any register for storing the operands. output operand constraint should have a constrint modifier "=". And this modifier says that it is the output operand and is write-only.
* There are two %'s prefixed to the register name. This helps GCC to distinguish between the operands and registers. operands have a single % as prefix.
* The clobbered register %eax after the third colon tells GCC that the value of %eax is to be modified inside "asm", so GCC won't use this register to store any other value.

When the execution of "asm" is complete, "b" will reflect the updated value, as it is specified as an output operand. In other words, the change made to "b" inside "asm" is supposed to be reflected outside the "asm".

### Assembler Template

The assembler template contains the set of assembly instructions that gets inserted inside the C program. The format is like:either each instruction should be enclosed within double quotes, or the entire group of instructions should be within double quotes. Each instruction should also end with a delimiter. The valid delimiters are newline(\n) and semicolon('). '\n' may be follow by a tab(\t). Operands corresponding to the C expressions are represented by %0,%1 ... etc.

### Operands

C expressions serve as operands for the assembly instructions inside "asm". Each operand is written as first an operand constraint in double quotes. For output operands, there'll be a constraint also within the quotes and then follows the C expression which stands for the operand. ie,

"constraint"(C expression) is the general form. For output operands an additional modifier will be there. Constraints are primarily used to decide the addressing modes for operands. They are also used in specifying the registers to be used.

If we use more than one operand, they are separted by comma.

In the assembler template, each operand is referenced by numbers. Numbering is down as follows. If there are a total of n operands(both input and output inclusive), then the first output operand is numbered 0, continuing in increasing order, and the last input operand is numbered n-1. The maximum number of operands is as we saw in the previous section.

Output operand expressions must be lvalues. The input operands are not restricted like this. They may be expressions. The extended asm feature is most often used for machine instructions the compiler itself does not know as existing ;-). If the output expression can't be directly addressed(for example, it is a bit-field), our constraint must allow a register. In that case, GCC will use the register as the output of the asm, and then store that register contents into the output.

As stated obove, ordinary output operands must be write-only; GCC will assume that the values in these operands before the instruction are dead and need not be generated. Extended asm also supports input-output or read-write operands.

Example:

```c
asm("leal (%1,%1,4), %0"
    :"=r"(file_times_x)
    :"r"(x)
    );
```
Here out input is in 'x'. We didn't specify the register to be used. GCC will choose some register for input, one for output and does what we desired. If we want the input and output to reside in the same register, we can instruct GCC to do so. Here we use those types of read-write operands. By specifying proper constraints, here we do it.

```c
asm("leal (%0,%0,4), %0""
    :"=r"(five_times_x)
    :"0"(x)
    );
```

Now the input and out operands are in the same register. But we don't know which register. Now if we want to specify that also, there is a way.

```c
asm("leal (%0,%0,4), %0"
    :"=c"(five_times_x)
    :"c"(x));
```

In the three example above, why did't we put any register to the clobber list. In the first two examples, GCC decides the registers and it knows what changes happen. In the last one, we don't have to put `ecx` on the clobber list, gcc knows it goes into x. Therefore, since it can know the value of `ecx`, it isn't considered clobbered.

### Clobber List

Some instructions clobber some hardware registers. We have to list those reigsters in the clobber-list, ie the field after the third ':' in the asm function. This is to inform gcc that we will use and modify them ourselves. So gcc will not assume that the values it loads into these registers will be valued. We shouldn't list the input and output registers in this list. Because, gcc knows that "asm" uses them(because they are specified explicitly as constraints). If the instructions use any other registers, implicitly or explicitly(and the registers are not represent either in input or in the output constraint list), then those registers have to be specified in the clobbered list.

If our instruction can alter the condition code register, we have to add "cc" to the list of clobbered registers.

If our instruction modifies memory in an unpredictable fashion, add "memory" to the list of clobbered registers. This will cause GCC to not keep memory values cached in the registers across the assembler instruction. We also have to add the **volatile** keyword if the memory affected is not listed in the inputs or outputs of the asm.

Example

```c
asm("movl %0,%%eax;
     movl %1,%%ecx;
     call _foo"
     :/*no outputs*/
     :"g"(from),"g"(to)
     :"eax", "ecx"
     );
```

### Volatile

If our assembly statement must execute where we put it,(i.e. must not be moved out of a loop as an optimizaiton), put the keyword `volatile` after asm and before the ()'s. So to keep it from moving, deleting and all, we declare it as 
`asm volatile (...:...:...:...)`

Use `__volatile__` when we have to be verymuch careful.

If our assembly is just for doing some calculations and does't have any side effects, it's better not to use the keyword `volatile`. Avoiding it helps gcc in optimizing the code and make it more beautiful.

## More about constraints

Constraints can say whether an operand may be in a register, and which kinds of register; whether the operand can be a memory reference, and which kinds of address; whether the operand may be an immediate constant, and which possible values (ie range of values) it may have... etc.

### Commonly used constraints
1. **Register operand constraint(r)**

When operands are specified using this constraint, they get stored in General Purpose Registers(GPR). Take the following example:

```c
asm("movl %%eax, %0\n" : "=r"(myval));
```

Here the variable myval is kept in a register, the value in register `eax` is copied onto that register, and the value of `myval` is updated into the memory from this register. When the "r" constraint is specified, gcc may keep the variable in any of the available GPRs. To specify the register, you must directly specify the register names by using specific register constraints. They are:

|r|Registers(s)|
|-|-|
|a|%eax,%ax,%al|
|b|%ebx,%bx,%bl|
|c|%ecx,%cx,%cl|
|d|%edx,%dx,%dl|
|S|%esi,%si|
|D|%edi,%di|

2. **Memory operand constraint(m)**

When the operands are in the memory, any operations performed on them will occur directly in the memory location, as opposed to register constraints, which first store the value in a register to be modified and then write it back to the memory location. But register constraints are usually used only when they are absolutely necessary for an instruction or they significantly speed up the process. Memory constraints can be used most efficiently in cases where a C variable needs to be updated inside "asm" and you really don't want to use a register to hold its value. 

Example, the value of idtr is stored in the memory location loc:

```c
asm("sidt %0\n"::"m"(loc));
```

3. **Matching(Digit) constraints**

In some cases, a single variable may serve as both the input and the output operand. Such cases may be specified in "asm" by using matching constraints.

```c
asm("incl %0":"=a"(var):"0"(value));
```

We saw similar examples in operands subsection also. In this example for matching constraints, the register %eax is used as both the input and the output variable. var input is read to %eax and updated %eax is stored in var again after increment. "0" here specifies the same constraint as the 0th output variable. That is, it specifies that the output instance of var should be stored in %eax only. This constraint can be used:

* In cases where input is read from a variable or the variable is modified and modification is written back to the same variable.
* In cases where separte instances of input and output operands are not necessary.

The most important effect of using matching restraints is that they lead to the efficient use of available registers.

Some other constraints used are:
* "m":A memory operand is allowed, with any kind of address that the machine supports in general.
* "o":A memory operand is allowed, but only if the address is offsettable. ie, adding a small offset to the address give a valid address.
* "V":A memory operand that is not offsettable. In other words, anything that would fit the 'm' constraint but not the 'o' constraint.
* "i":An immediate integer operand(one with constant value) is allowed. This includes synbolic constants whose values will be known only at assembly time.
* "n":An immediate integer operand with a known numeric value is allowed. Many systems cannot support assembly-time constants for operands less than a word wide. Constraints for these operands should use 'n' rather than 'i'.
* "g":Any register, memory or immediate integer operand is allowed, except for registers that are not general registers.

Following constraitns are x86 specific.
* "r":Register operand constraint.
* "q":Registers a,b,c or d.
* "I":Constant in range 0 to 31(for 32-bit shifts).
* "J":Constant in range 0 to 63(for 64-bit shifts).
* "K":0xff.
* "L":0xffff.
* "M":0,1,2, or 3(shift for lea instruction).
* "N":Constant in range 0 to 255(for out instruction).
* "f":floating point register.
* "t":First(top of stack) floating point register
* "u":Second floating point register.
* "A":Specifies the 'a' or 'd' registers. This is primarily useful for 64-bit integer values intended to be returned with the 'd' register holding the most significant bits and the 'a' register holding the least significant bits.

### Constraint Modifiers
While using constraints, for more precise constrol over the effects of constraints, GCC provides us with constraint modifiers. Mostly used constraint modifiers are
1. "=":Means that this operand is write-only for this instruction; the previous value is discarded and replaced by output data.
2. "&":Means that this operand is an earlyclobber operand, which is modified before the instruction is finished using the input operands. Therefore, this operand may not lie in a register that is used as an input operand or as part of any memory address. An input operand can be tied to an earlyclobber operand if its only use as an input occurs before the early result is written.
