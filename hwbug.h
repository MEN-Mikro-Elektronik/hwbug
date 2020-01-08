/****************************************************************************
 * Copyright 2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _HWBUG_H_
#define _HWBUG_H_

#if OS9
#undef NULL
#define NL 0x0d
#include <setjmp.h>
#include <setsys.h>
#include <stdio.h>
#include <ctype.h>
#define FLUSH fflush(stdout)
#endif


#if defined (LYNX) || defined (HPRT) || defined (LINUX)

#undef NULL
#define NL 0x0a
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef MAC_IO_MAPPED_EN
#include <sys/io.h>
#endif
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#define FLUSH fflush(stdout)
#endif

#ifdef LINUX

extern void os_exit(void);
extern int os_get_char(void);
extern void hist_init(void);
extern char *hist_prev(void);
extern char *hist_next(void);
extern void hist_save( char *command );
extern int change_data( int argc, char **argv);
extern int fill_data( int argc, char **argv);
extern int display_data( int argc, char **argv);
extern  void os_usage(int argc, char **argv);
extern int32_t os_init_io_mapped(void);
extern void os_init(void);
extern unsigned long os_access_address( );
#endif

int make_hex( );
int line_args( );
void free_args( );

extern uint8_t G_iomapped;  /**< flag for performing io-mapped access */

#endif /* _HWBUG_H_ */
