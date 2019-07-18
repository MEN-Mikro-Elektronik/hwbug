# Simple Debugger for Hardware Development Purpose

There are two tools: hwbug and hwbug_cmd. They are
independent from each other and the build processes don't interfere.

**Purpose:**

Memory management. Perform read/write to memory at given address.

hwbug_cmd is quite suitable to use in scripts. 

hwbug is the simple tool to read/write registers.

**Options hwbug**
	 H                            Display help
	 C[B|W|L|N|#] <adr>           Change Memory
	                              (B)yte (W)ord (L)ong (N)oRead (#)Increm.
	 F[B|W|L]  <adr> <cnt> <val>  Fill Memory with fixed Values
	                              (B)yte (W)ord (L)ong
	 F[B|W|L]P <adr> <cnt>        Fill Memory with linear Pattern
	                              (B)yte (W)ord (L)ong
	 D[B|W|L] [<adr> <cnt>]       Display Memory
	                              (B)yte (W)ord (L)ong
	 Q                            Quit
	 I                             perform io_mapped accesses
		                           when supported
	 M                             perform mem_mapped accesses

	Line Editing Features:
	 ^W = up history       ^Z = down history
	 ^H = delete char      ^X = kill line

**Options hwbug_cmd**
        startaddr  PCI register address                
        [endaddr]  PCI register end address            
        [-d]       dump register content               
        [-b|-w|-l] Byte OR Word OR Long access  [Byte] 
        [-i]       activate I/O mode                   
        [-v=hex]   value to write in register   [0x0]  

    Calling examples:
     - dump a (valid) PCI address range (memory-mapped, byte access):
        hwbug 0xfdbe0000 0xfdbeffff -d 
     - change a (valid) PCI register (memory-mapped, long access):
        hwbug 0xfdbe0000 -l -v=0x12345678 
     - disable F11S FPGA watchdog (i/o-mapped): 
        hwbug 0xd910 -w -v=0x7fff -i
     - enable F11S FPGA watchdog (i/o-mapped): 
        hwbug 0xd910 -w -v=0x8000 -i

**Usage:**

    Calling Examples:
    linux-box:/home/linux/HWBUG_CMD # ./hwbug db 0xc2501000 20

    c2501000+00000000: 00 00 01 00 00 60 0f 01 00 60 00 01 00 60 00 01 .....`...`...`..
    c2501000+00000010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
    linux-box:/home/linux/HWBUG_CMD # ./hwbug cb 0xc2501001 0f

    Change the value at C2501001 into 0f
    linux-box:/home/linux/HWBUG_CMD # ./hwbug db 0xc2501000 20

    c2501000+00000000: 00 0f 02 00 00 60 00 01 00 60 00 01 00 60 00 01 .....`...`...`..
    c2501000+00000010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
    linux-box:/home/linux/HWBUG_CMD # 


    An example of using hwbug and hwbug_cmd for IO mapped access:

    root@men-pc:/home/men# /opt/menlinux/BIN/fpga_load_x86-32 1a88 4d45 d7 0 -t
    Cham_Info for device 9/0/0 (bus/dev/fun):
     chaRev: 2;     busId: 0;       tableNbr 1;
     unitNbr: 12;   bridgeNbr: 0;   cpuNbr 0;
    BAR0: 0x92201000; size: 0x00000000, mapType: MEM;
    BAR1: 0x0000e001; size: 0x00000000, mapType: IO;
    BAR2: 0x92000000; size: 0x00000000, mapType: MEM;
    BAR3: 0x92200000; size: 0x00000000, mapType: MEM;
    BAR4: 0x0000ec01; size: 0x00000000, mapType: IO;
    BAR5: 0x0000e801; size: 0x00000000, mapType: IO;
    CHAMELEONV2_HEADER # 0:
      busType=0x00, busId=0, model=A, MajorRevision=0x07,  MinorRevision=0x00,
     file=   16F614-00, magicWord=0xabce
    CHAMELEONV2_UNIT:
    Idx DevId  Module                   Grp Inst Var Rev IRQ BAR Offset     Address
    --- ------ ------------------------ --- ---- --- --- --- --- ---------- ----------
      0 0x007e 16Z126_SERFLASH            0    0   1  11  63   0 0x00000100 0x92201100
      1 0x0045 16Z069_RST                 0    0   0  13  63   1 0x00000100 0xe101
      2 0x0022 16Z034_GPIO                0    0   0  10   0   1 0x00000300 0xe301
      3 0x0022 16Z034_GPIO                0    1   0  10   6   1 0x00000400 0xe401
      4 0x0018 16Z024_SRAM                0    0   2   6  63   2 0x00000000 0x92000000
      5 0x0057 16Z087_ETH                 0    0   0  24   1   3 0x00000000 0x92200000
      6 0x0057 16Z087_ETH                 0    1   0  24   7   3 0x00000100 0x92200100
      7 0x007d 16Z125_UART                0    0   0  21   2   4 0x00000000 0xec01
      8 0x007d 16Z125_UART                0    1   0  21   3   4 0x00000010 0xec11
      9 0x007d 16Z125_UART                0    2   0  21   4   4 0x00000020 0xec21
     10 0x007d 16Z125_UART                0    3   0  21   5   4 0x00000030 0xec31
     11 0x003a ?                          0    0   0   5  63   5 0x00000200 0xea01
    root@men-pc:/home/men# 

    root@men-pc:/home/men/HWBUG_CMD# ./hwbug_cmd -v=cc -i 0xec01 0xec31

    ============================
    ===      HWBUG_CMD       ===
    ===   $Revision: 1.1 $   ===
    ============================

    I/O mode

    Write register from address ec01 with value cc

    root@men-pc:/home/men/HWBUG_CMD# ./hwbug_cmd -d -i 0xec01 0xec31

    ============================
    ===      HWBUG_CMD       ===
    ===   $Revision: 1.1 $   ===
    ============================

    I/O mode

    Dump register from address ec01
    0000ec01+00000000: cc 01 cc 0c 60 00 ac 0c 60 00 ac 0c 60 00 ac cc ....`...`...`...
    0000ec01+00000010: cc 01 cc 0c 60 00 ec 0c 60 00 ec 0c 60 00 ec cc ....`...`...`...
    0000ec01+00000020: cc 04 cc 0c 61 00 ec 0c 61 00 ec 0c 61 00 ec 00 ....a...a...a...

    root@men-pc:/home/men/HWBUG_CMD# ./hwbug
    ,------------------------------,
    |   HwBug - Hardware Debugger  |
    |      (c) MEN 1993-2010       |
    |    by K.Popp, R.Seeberger    |
    |                              |
    |   Press 'H' for help-screen  |
     ------------------------------'

    HwBug> I
    HwBug> DB 0XEC01 30
    0000ec01+00000000: cc 01 cc 0c 60 00 ac 0c 60 00 ac 0c 60 00 ac cc ....`...`...`...
    0000ec01+00000010: cc 01 cc 0c 60 00 ec 0c 60 00 ec 0c 60 00 ec cc ....`...`...`...
    0000ec01+00000020: cc 04 cc 0c 61 00 ec 0c 61 00 ec 0c 61 00 ec 00 ....a...a...a...
    HwBug> 

**Build:**

Edit MDIS project Makefile to contain:
(can be added by mdiswiz tool, Project->Settings->Software Modules)

    $ ALL_COM_TOOLS = \
    $     HWBUG/program.mak

or run make separately:

    $ cd ~/HWBUG
    $ make

The output will be a binary file: hwbug, hwbug_cmd

**Package is delivered with hwbug binaries for platforms:**

-  32bit x86 (hwbug_x86)
-  PowerPc / cisco 4500 (hwbug_ppc)
