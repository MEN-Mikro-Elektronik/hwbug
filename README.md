# HWBUG - Memory (HW Register) read/write Utility

Note: This repo contains currently only the HWBUG for Linux implementation.

There are two tools, that supports read/write access to physical memory locations (e.g. device registers):

- hwbug - classic hwbug tool with CLI
- hwbug_cmd - scriptable hwbug tool with call parameters

The repo contains the following hwbug binaries:

- x86 32-bit    : hwbug_x86, hwbug_cmd_x86
- PowerPc 32-bit: hwbug_ppc, hwbug_cmd_ppc

**Accessing PCI devices**

Because UEFI boot doesn't enable the memory regions of PCI devices as the legacy BIOS boot did, you may run into trouble if you try to access these memory regions with hwbug under Linux.
As workaround, you can manually enable the memory regions under Linux from the command line:

```
# setpci -s 07:00.0 COMMAND
0100
# setpci -s 07:00.0 COMMAND=0x103
```

Where 07:00.0 is the PCI bus:dev.func location of the PCI device. Read the current value of the PCI configuration space command register, and write back the value ORed with 0x1 to enable the I/O space and 0x2 to enables the memory space.

## hwbug

### hwbug Command Line Interface

```
Commands:
 C[B|W|L][N][<#>] <adr> [<val>] Change memory
                                  (B)yte/(W)ord/(L)ong access, (N)oRead,
                                  increment B/W/L or <#> bytes,
                                  use '-' to go back and '.' to quit
 F[B|W|L]  <adr> <cnt> <val>    Fill memory with <val>
                                  (B)yte (W)ord (L)ong
 F[B|W|L]P <adr> <cnt>          Fill memory with linear pattern
                                  (B)yte (W)ord (L)ong
 D[B|W|L] [<adr>] [<cnt>]       Display memory
                                  (B)yte (W)ord (L)ong
 I                              i/o mapped accesses (if supported)
 M                              memory mapped accesses (default)
 H                              Show this usage info
 Q                              Quit

Line Editing Features:
 ^W = up history       ^N = down history
 ^H = delete char      ^X = kill line

Note: Only 32-bit addresses are supported
```

### hwbug Calling Examples

Dump 0x40 bytes from specified memory range (word access)
```
HwBug> DW D9214400 40
d9214400+0000: 0100 0302 0504 0706 0908 0b0a 0d0c 0f0e ................
d9214400+0010: 1110 1312 1514 1716 1918 1b1a 1d1c 1f1e ................
d9214400+0020: 2120 2322 2524 2726 2928 2b2a 2d2c 2f2e  !"#$%&'()*+,-./
d9214400+0030: 3130 3332 3534 3736 3938 3b3a 3d3c 3f3e 0123456789:;<=>?
```

Write value into 32-bit register and read it back (long access)
```
HwBug> CL B0405000
b0405000: 00000000 -> 300
b0405004: 00000000 -> -
b0405000: 00000300
```

## hwbug_cmd

### hwbug_cmd Usage

```
Usage:    hwbug_cmd <startaddr> [<endaddr>] [<opts>]

Function: Mem R/W Utility
          Only 32-bit addresses are supported

Options:                             [default]
    [-d]       dump memory
    [-v=<hex>  write hex to memory   [0x0]
    [-b|-w|-l] byte/word/long access [byte]
    [-i]       i/o mapped access     [memory mapped]

Calling examples:
 - dump memory address range (memory mapped, byte access):
    hwbug_cmd 0xfdbe0000 0xfdbeffff -d
 - write value to memory (memory-mapped, long access):
    hwbug_cmd 0xfdbe0000 -l -v=0x12345678
 - disable F11S FPGA watchdog (i/o-mapped, word access):
    hwbug_cmd 0xd910 -w -v=0x7fff -i
 - enable F11S FPGA watchdog (i/o-mapped, word access):
    hwbug_cmd 0xd910 -w -v=0x8000 -i

(c) 2018-2019, MEN Mikro Elektronik GmbH, Version 1.2
```

### hwbug_cmd Calling Examples

Dump 0x20 bytes from specified memory range (long access)
```
# ./hwbug_cmd 0xb8200000 0xb8200020 -l -d
Dump memory b8200000..b8200020
b8200000+00000000: 00004101 0000abce 2d353132 43493030 .A......215-00IC
b8200000+00000010: 41313030 0060133f 00000000 00000000 001A?.`.........
```

Write value into specified memory range and read it back (long access)
```
# ./hwbug_cmd 0xb82003f0 0xb82003f8 -l -v=73
Write value 0x73 to memory 0xb82003f0..0xb82003f8

# ./hwbug_cmd 0xb82003f0 0xb82003f8 -l -d
Dump memory b82003f0..b82003f8
b82003f0+00000000: 00000073 00000073                   s...s...
```

Dump 0x30 bytes from specified memory range (byte access, i/o mapped)
```
# ./hwbug_cmd -d -i 0x10 0x40

i/o mapped access

Dump memory 10..40
00000010+00000000: 00 d7 01 d5 03 d5 12 d7 00 ff ff ff ff ff ff 0f ................
00000010+00000010: 80 ff ff ff 80 ff ff ff 80 ff ff ff 80 ff ff ff ................
00000010+00000020: 80 ff ff ff 80 ff ff ff 80 ff ff ff 80 ff ff ff ................
```

## Build

To build hwbug and hwbug_cmd, you have two possibilities:

### Standalone build

```
# make
```

### Building with MDIS project

Edit MDIS project Makefile to contain:
```
    ALL_COM_TOOLS = \
        HWBUG/program.mak
```
or add it with mdiswiz tool (Project->Settings->Software Modules)

