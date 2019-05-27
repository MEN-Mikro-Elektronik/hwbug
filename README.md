# Simple Debugger for Hardware Development Purpose

**Purpose:**

Memory management. Perform read/write to memory at given address.

**Options:**

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



**Build:**

Edit MDIS project Makefile to contain:
(can be added by mdiswiz tool, Project->Settings->Software Modules)

    $ ALL_COM_TOOLS = \
    $     HWBUG/program.mak

or run make separately:

    $ cd ~/HWBUG
    $ make

The output will be a binary file: hwbug

**Package is delivered with binaries for platforms:**

-  32bit x86 (hwbug_x86)
-  PowerPc / cisco 4500 (hwbug_ppc)