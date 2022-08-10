- [IA-32 hardware](#ia-32-hardware)
  - [Registers](#registers)
  - [Segment](#segment)
  - [Instruction pointer](#instruction-pointer)
  - [Floating-point data](#floating-point-data)
  - [Control](#control)
  - [Debug](#debug)
  - [EFLAGS register](#eflags-register)
    - [Status flags](#status-flags)
    - [Control flags](#control-flags)
    - [System flags](#system-flags)

# IA-32 hardware

## Registers
General purpose(Eight 32-bit registers used for storing working data)
|Register|Contents|
|-|-|
|EAX|(RAX for 64-bit) Accumulator for operands and results data|
|EBX|Pointer to data in the data memory segment|
|ECX|Counter for string and loop operations|
|EDX|I/O pointer|
|EDI|Data pointer for destination of string operations|
|ESI|Data pointer for source of string operations|
|ESP|Stack pointer|
|EBP|Stack base pointer|

ESP is the top of the stack. EBP is usually set to esp at the start of the function. Local variables are accessed by subtracting a constant offset from EBP. All x86 calling conventions define EBP as being preserved across funciton calls. EBP itself actually points to the previous frame's base pointer, which enables stack walking in a debugger and viewing other frames local variables to work.

## Segment
Six 16-bit registers used for handling memory access
three model:Flat memory model,Segment memory model,Real-address model
|Segment|Contents|
|-|-|
|CS|Code segment|
|DS|Data segment|
|SS|Stack segment|
|ES|Extra segment pointer|
|FS|Extra segment pointer|
|GS|Extra segment pointer|

## Instruction pointer
32-bit register pointing to next instruction code

EIP register, sometimes called the program counter In a flat memory model, the instruction pointer contain the linear address of the memory location for the next instruction code. If the application is using a segmented memory model, the instruction pointer points to a logical memory address, referenced by the contents of the CS register.

## Floating-point data
Eight 80-bit registers for floating-point data

## Control
Five 32-bit registers used to determine the operating mode
|Register|Contents|
|-|-|
|CR0|System flags that control mode and status of the processor|
|CR1|Not currently used|
|CR2|Memory page fault information|
|CR3|Memory page directory information|
|CR4|Flags enable processor features and indicate capabilities|

## Debug
Eight 32-bit registers used to contain information when debugging the processor

## EFLAGS register

### Status flags
* CF(bit 0) Carry flag
* PF(bit 2) Parity flag
* AF(bit 4) Adjust flag
* ZF(bit 6) Zero flag
* SF(bit 7) Sign flag
* OF(bit 11) Overflow flag

### Control flags
DF flag, or direction flag(DF flag is set(set to one), string instruction automatically decrement memory addresses to get the next byte in the string. When the DF flag is cleared(set to zero), string instruction automatically increment memory address to get the next byte in the string)

### System flags
* TF(bit 8) Trap flag
* IF(bit 9) Interrupt enable flag
* IOPL(bit 12 and 13) I/O privilege level flag
* NT(bit 14) Nested task flag
* RF(bit 16) Resume flag
* VM(bit 17) Virtual-8086 mode flag
* AC(bit 18) Alignment check flag
* VIF(bit 19) Virtual interrupt flag
* VIP(bit 20) Virtual interrupt pending flag
* ID(bit 21) Identification flag


