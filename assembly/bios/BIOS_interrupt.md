- [BIOS Interrupt](#bios-interrupt)
  - [Interrupt 10H (Int 10h) Video Services](#interrupt-10h-int-10h-video-services)
    - [Interrupt 10H Service 3 : Read Cursor Position And Size](#interrupt-10h-service-3--read-cursor-position-and-size)
    - [Interrupt 10H Service 15 : Get Current Video Mode](#interrupt-10h-service-15--get-current-video-mode)
    - [Interrupt 10H Service 18 : Alternate Select](#interrupt-10h-service-18--alternate-select)
    - [Interrupt 10H Service 19 : Write Character String](#interrupt-10h-service-19--write-character-string)
  - [Interrupt 13H (Int 13h) Disk I/O Services, Floppy And Hard Disks](#interrupt-13h-int-13h-disk-io-services-floppy-and-hard-disks)
    - [Interrupt 13H Service 0 : Reset Fixed Disk / Diskette System](#interrupt-13h-service-0--reset-fixed-disk--diskette-system)
    - [Interrupt 13H Service 2 : Read Sectors Into Memory](#interrupt-13h-service-2--read-sectors-into-memory)
    - [Interrupt 13H Service 8 : Get Current Drive Parameters (Fixed Disk)](#interrupt-13h-service-8--get-current-drive-parameters-fixed-disk)
    - [Interrupt 13H Service 21 : Read DASD Type](#interrupt-13h-service-21--read-dasd-type)
  - [Interrupt 15H (Int 15h) Cassette And Extended Service](#interrupt-15h-int-15h-cassette-and-extended-service)
    - [Interrupt 15H Service 136 : Get Extended Memory Size](#interrupt-15h-service-136--get-extended-memory-size)
# BIOS Interrupt
**[BIOS中断说明](https://grandidierite.github.io/bios-interrupts/)**

## Interrupt 10H (Int 10h) Video Services

### Interrupt 10H Service 3 : Read Cursor Position And Size
Reports the cursor position (row and column) and size for a specified display page.
**Input**:
AH = 03h
BH = Display page number
**Output**:
CH = Cursor start line
CL = Cursor end line
DH = row
DL = column
**Register destroyed**: AX, SP, BP, SI, DI

### Interrupt 10H Service 15 : Get Current Video Mode
Reports the current video mode, the number of character positions per line in that video mode, and the current display page.
**Input**:
AH = 0fh
**Output**:
AL = Video mode
AH = Number of character columns per line
BH = Active display page number

Register destroyed: AX, SP, BP, SI, DI

### Interrupt 10H Service 18 : Alternate Select
Returns information on the Enhanced Display Adapter (EGA) and allows the selection of an alternative EGA print screen routine.
**Input**:
AH = 12h
AL = subservice
      10h : return EGA information
      20h : select alternative EGA print screen routine
**Output**:
BH = Display mode in effect
      00h : Color mode (3Dxh address range)
      01h : Monochrome mode (3Bxh address range)
BL = EGA memory
      00h : 64k
      01h : 128k
      02h : 192k
      03h : 256k
CH = Adapter bits
CL = Switch setting

The value returned in CL (Switch setting)represents the DIP switch settings on the EGA. The adapter bits (CH) is defined as follows

```c
7 6 5 4 3 2 1 0
x x x x . . . .     Unused
. . . . x x . .     Reserved
. . . . . . x .     FEAT1 (pin 17)
. . . . . . . x     FEAT0 (pin 19)
```

To print the screen contents when the number of rows changes, use subservice 20h (Set alternative print screen) to install a print screen routine that will work correctly with the row change.

### Interrupt 10H Service 19 : Write Character String
Writes a string of characters with the specified attributes to any display page.
**Input**:
AH = 13h
AL = Subservice (0-3)
BH = Display page number
BL = Attribute (subservice 0 and 1)
CX = Length of string
DH = Row position where string is to be written
DL = Column position where string is to be written
ES:SP = Pointer to string to write
**Output**: none
**Register destroyed**: AX, SP, BP, SI, DI

This service is available only for XTs dated 1/19/86 and later, ATs, EGAs, and PC Convertibles.

The service has four subservices, as follows:
AL = 00h : Assign all characters the attribute in BL; do not update cursor
AL = 01h : Assign all characters the attribute in BL; update cursor
AL = 02h : Use attributes in string; do not update cursor
AL = 03h : Use attributes in string; update cursor

In subservices 0 and 1, all characters in the string are written to the screen with the same attribute – the attribute specified in BL.

In subservices 2 and 3, the attribute byte for each character is found in the string itself. The string itself consists of a character followed by its attribute, another character followed by its attribute, and so on. The string is copied directly to the video buffer as is.

In subservices 0 and 2, the cursor position is not updated after the string is written.

In subservices 1 and 3, the cursor is moved to the first position following the last character in the string.

Like service 0eh, service 13h responds appropriately to ASCII 07h (bell), 08h (backspace), 10h (line feed), and 0dh (carriage return). All other characters are printed.

## Interrupt 13H (Int 13h) Disk I/O Services, Floppy And Hard Disks

### Interrupt 13H Service 0 : Reset Fixed Disk / Diskette System
Resets the fixed disk or diskette controller and drive, forcing recalibration of the read/write head.
**Input**:
AH = 00h
DL = Drive number
**Output**:
AH = Status of operation
CF = Set if error, cleared otherwise

If DL is greater than or equal to 80h, the floppy diskette controller is reset, and then the fixed disk controller is reset. The status returned in AH in this case is the status of the fixed disk; to get the status of the floppy diskette controller, examine memory location 0:441h.

If an error is reported by the disk BIOS routines, use this service to reset the floppy and/or fixed disk controllers and retry the operation.

A fixed disk reset is executed only if the value in DL is less than or equal to the last fixed disk on the system. That is, the lower 7 bits of DL should be less than or equal to the number of fixed disks.

For the AT, XT-286, and PC convertible, the BIOS executes int 15h, service 90h (Device busy), for the diskette (type = 01h) and the fixed disk (type = 00h) prior to waiting for the interrupt. Int 15h, service 91h (interrupt complete), is executed upon completion. Also diskette operations that require the diskette motor to be on will call int 15h, service 90h (device busy), with the type equal to “diskette drive motor start” (type = fdh). This allows the system to perform another task while the drive motor is waiting to get up to speed.

### Interrupt 13H Service 2 : Read Sectors Into Memory
Reads one or more sectors from a fixed or floppy disk into memory.
**Input**:
AH = 02h
AL = Number of sectors to read
CH = Cylinder number (10 bit value, upper 2 bits in CL)
CL = Starting sector number
DH = Head number
DL = Drive number
ES:BX = Address of memory buffer
**Output**:
AH = Status of operation
AL = Number of sectors read
CF = Set if error, else cleared

Reads the specified number of sectors starting at the specified location (head, cylinder, and track) from a disk into a buffer starting at ES:BX.

Value in DL less than 80h specify floppy disks; value greater than 80h specify fixed disks. For example, 0 means the first floppy diskette, while 80h means the first fixed disk.

The cylinder number is ten-bit quantity (0 through 1023). Its most significant two bits are in bits 7 and 6 of CL; the remaining eight bits are in CH. The starting sector number fits in the low-order portion (lower 6 bits) of CL.

The value returned in AL (number of sectors read) may not give the correct number of sectors, even though no error has occurred. Use the results of the carry flag and AH (status flag) to determine the status of the operation.

The sectors written must all be on the same cylinder and same side for diskettes, while a hard drive can write at most 128 sectors at one time.

If an error is encountered while reading a sector, use service 0 to reset the drive and retry the operation. It is recommended that at least 3 retries be attempted before an error is signalled, since the error may have resulted from the diskette motor not being up to speed.

Because the architecture of the DMA channel, an error will occur if the buffer in memory for the sectors overlaps a 64k page boundary. A 64k page boundary is a memory which is one of the following (10000h, 20000h, 30000h, etc). Ensure that no part of your buffer falls on such a boundary. If it does, create a new buffer or start the buffer just after the boundary.

If an error 11h is returned, the data is good but the BIOS reports that is was corrected using the ECC error-correcting algorithm. The error may not occur again if the information is written back out.

This service differs from the DOS int 25h (Read absolute sector) in that the DOS int 25h works with logical devices RAM disks, bernoulli drives, etc). It is much more flexible. Also int 25h works with a linear address, whereas this service works with 3 coordinates to address a disk location. int 25h doesn’t have a DMA problem mentioned above, and there is no limit on the number of sectors that can be read in one operation. In addition, int 25h will do all the error-retry attempts itself. int 25h should be used instead of this service for obtaining absolute sector control. Programs that may need to use this service rather than int 25h include partition manipulators, system software, and disk cache software.

For the AT, XT-286, and PC Convertible, the BIOS executes INT 15h, Service 90h (Device Busy), for the diskette (Type = 01h) and the fixed disk (Type = 00h) prior to waiting for the interrupt. INT 15h, Service 91h (Interrupt Complete), is executed upon completion. Also diskette operations that require the diskette motor to be on will call INT 15h, Service 90 (Device Busy), with the type equal to “Diskette Drive Motor Start” (Type = FDh). This allows the system to perform another task while the drive motor is waiting to get up to speed.

### Interrupt 13H Service 8 : Get Current Drive Parameters (Fixed Disk)
Reports disk drive parameters, such as the number of heads, tracks, and sectors per track.
**Input**:
AH = 08h
DL = Drive number
**Output**:
CH = Maximum value for cylinder (10-bit value; upper 2 bits in CL)
CL = Maximum value for sector
DH = Maximum value for heads

For fixed disks:
AH = Status of operation
DL = Number of fixed disks
CF = Set if error; otherwise cleared

For Diskette:
AX = 0
BL = Bits 7 to 4 = 0
Bits 3 to 0 – Valid drive type in CMOS
BH = 0
DL = Number of diskettes
ES:DI = Pointer to 11-byte diskette drive parameter table

This service is available for diskettes on PC Convertibles, XT-286s, and ATs dated after 1/10/84. All machines support the fixed disk mode of this service.

Values in DL less than 80h specify diskettes; values greater than 80h specify fixed disks. For example, 0 means the first diskette, while 80h means the first fixed disk.

The cylinder number is a ten-bit quantity (0 through 1023). Its most significant two bits are in bits 7 and 6 of CL; the remaining eight bits are in CH. The starting sector number fits in the low-order portion (lower 6 bits) of CL.

If the drive is a fixed disk and there is an error, the Carry Flag will be set. If the drive number is invalid, AH will be equal to 7. And if no fixed disk drive is present or the fixed disk drive adapter is not installed, AH will be equal to 1.

If the drive is a diskette and there is an error, then:

If the drive type is known but 1) the CMOS is invalid, not present, 2) the battery is discharged, or 3) the CMOS checksum is invalid, then all registers will contain valid information, but BL will be 0.

If the drive type is not known or the requested drive is not installed, then BX, CX, DH, ES, and DI will all be 0. DL will contain the number of diskette drives.

**Valid Diskette Drive Types in CMOS**
|BL(bits 3-0)|Meaning|
|-|-|
|00h|Unknown drive type|
|01h|360K,5.25inch,40track|
|02h|1.2M,5.25inch,80track|
|03h|729K,3.5inch,80track|

For the AT, XT-286, and PC Convertible, the BIOS executes INT 15h, Service 90h (Device Busy), for the diskette (Type = 01h) and the fixed disk (Type = 00h) prior to waiting for the interrupt. INT 15h, Service 91h (Interrupt Complete), is executed upon completion. Also diskette operations that require the diskette motor to be on will call INT 15h, Service 90 (Device Busy), with the type equal to “Diskette Drive Motor Start” (Type = FDh). This allows the system to perform another task while the drive motor is waiting to get up to speed.

### Interrupt 13H Service 21 : Read DASD Type
Returns the type of interface associated with a drive and also the number of sectors the drive contains. Can be used to determine if a diskette drive supports the sensing of a diskette change.
**Input**:
AH = 15h
DL = Drive number (Bit 7 must be set)
**Output**:
CF = Set if error; otherwise cleared
AH = Disk type
       Status of operation if error
CX:DX = If no error and AH <> 0, then 4-byte integer with number of sectors

This service is not available for PCs, PCjrs, and XTs dated before 1/10/86.

This service allows you to determine whether a drive supports the Change Line Status service (Service 16h). The Change Line Status informs the system whether or not a diskette has changed since the last access to it.

Use Service 17h (Set DASD Type) to set the type.

**Disk Type**
|AH|Meaning|
|-|-|
|00h|Drive not present|
|01h|Diskette, no change line available|
|02h|Diskette, change line available|
|03h|Fixed Disk|

For the AT, XT-286, and PC Convertible, the BIOS executes INT 15h, Service 90h (Device Busy), for the diskette (Type = 01h) and the fixed disk (Type = 00h) prior to waiting for the interrupt. INT 15h, Service 91h (Interrupt Complete), is executed upon completion. Also diskette operations that require the diskette motor to be on will call INT 15h, Service 90 (Device Busy), with the type equal to “Diskette Drive Motor Start” (Type = FDh). This allows the system to perform another task while the drive motor is waiting to get up to speed.

## Interrupt 15H (Int 15h) Cassette And Extended Service

### Interrupt 15H Service 136 : Get Extended Memory Size
Reports amount of memory above 1024k.
**Input**:
AH = 88h
**Output**:
CF = set if error, else cleared
AX = Number of 1k blocks above 1024k

This service is available only for the AT and XT-286.

The amount of memory returned by this service is stored in the CMOS battery. It is determined and set during power-on.

For the memory above the 1024K range to be usable, the planar memory (memory on the system board) must be fully populated. The memory above the 1024K range is often called “extended” memory.