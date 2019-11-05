/******************************************************************************
 *************                                                    *************
 *************                     HWBUG_CMD                      *************
 *************                                                    *************
 ******************************************************************************/
/*!
 *         Author: Yi.Xu@men.de
 *
 *        \brief Simple tool to change or dump memory (register)
 *               (I/O or Memory-mapped)
 *
 *-----------------------------------------------------------------------------
 * Copyright (c) 2018-2019, MEN Mikro Elektronik GmbH
 ******************************************************************************/
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

/*-------------------------------------+
|    INCLUDES                          |
+-------------------------------------*/
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <asm/errno.h>

#include <fcntl.h>
#define FLUSH fflush(stdout)
#define ONEBYTE        1
#define ONEWORD        2
#define ONELONG        4
#define MEMORY_MAPPED  0
#define IO_MAPPED      1

/*-------------------------------------+
|    PROTOTYPES                        |
+-------------------------------------*/
int access_io_mapped();
int32_t os_init_io_mapped();
static void dump_register ( int type, unsigned long p_adr, uint32_t size, int mapmode );
static void write_register( int type, unsigned long p_adr, uint32_t size, uint32_t value, int mapmode );
unsigned long os_access_address( );
long int strtol (const char* str, char** endptr, int base);

/************************************ usage ***********************************/
/** Prints the program usage
 */
static void usage( void )
{
    printf(
        "\n"
        "Usage:    hwbug_cmd <startaddr> [<endaddr>] [<opts>]\n"
        "\n"
        "Function: Mem R/W Utility\n"
        "          Only 32-bit addresses are supported\n"
        "\n"
        "Options:                             [default]\n"
        "    [-d]       dump memory\n"
        "    [-v=<hex>  write hex to memory   [0x0]\n"
        "    [-b|-w|-l] byte/word/long access [byte]\n"
        "    [-i]       i/o mapped access     [memory mapped]\n"
        "\n"
        "Calling examples:\n"
        " - dump memory address range (memory mapped, byte access):\n"
        "    hwbug_cmd 0xfdbe0000 0xfdbeffff -d\n"
        " - write value to memory (memory-mapped, long access):\n"
        "    hwbug_cmd 0xfdbe0000 -l -v=0x12345678\n"
        " - disable F11S FPGA watchdog (i/o-mapped, word access):\n"
        "    hwbug_cmd 0xd910 -w -v=0x7fff -i\n"
        " - enable F11S FPGA watchdog (i/o-mapped, word access):\n"
        "    hwbug_cmd 0xd910 -w -v=0x8000 -i\n"
        "\n"
        "(c) 2018-2019, MEN Mikro Elektronik GmbH, Version 1.2\n\n");
}

/******************************************************************************/
/** Program main function
 *
 *  \param argc  \IN  argument counter
 *  \param argv  \IN  argument vector
 *
 *  \return      success (0) or error (-1)
 */
int main(int argc, char *argv[])
{
    int      mapmode = MEMORY_MAPPED;
    int      type = ONEBYTE;
    unsigned long startadr=0x0, endadr=0x0;  /* start and end address of memory */
    unsigned long size=0;                    /* size between startadr and endadr */
    unsigned long value=0x0;
    int      read=0, write=0, i, retval=0;

    /*--------------------+
    |  get arguments      |
    +--------------------*/

    for( i=1; i<argc; i++ ){

        if( *argv[i] != '-' ){
            if( startadr == 0x0 ){
                sscanf( argv[i], "%lx", &startadr );
            }
            else
                sscanf( argv[i], "%lx", &endadr );
        }
        else {
            switch (*(((unsigned char *)argv[i]) + 1))
            {
            case 'i':
               mapmode = IO_MAPPED;
               break;
            case 'b':
               type = ONEBYTE;
               break;
            case 'w':
               type = ONEWORD;
               break;
            case 'l':
               type = ONELONG;
               break;
            case 'd':
               read = 1;
               break;
            case 'v':
               write = 1;
               value = strtol(argv[i] + 3, NULL, 16);
               break;
            }
        }
    }

    /*--------------------+
    |  verify arguments   |
    +--------------------*/
    if( startadr == 0x0 ){
        usage();
        retval = (-1);
        goto EXIT;
    }

    if( endadr == 0x0 ){
        endadr = startadr + type;
    }
    else {
        if( endadr < startadr ){
            printf("\nendadr is smaller than startadr. Please enter a valid end address!\n");
            usage();
            retval = (-1);
            goto EXIT;
        }
    }

    if( !read && !write){
        printf("\nPlease use -d or -v to dump or change register values!\n");
        usage();
        retval = (-1);
        goto EXIT;
    }

    if( value == -1 ){
        printf("\nPlease enter a valid new value, eg: -v=0xff\n");
        usage();
        retval = (-1);
        goto EXIT;
    }

    size = endadr - startadr;

    if( mapmode == IO_MAPPED ){
        printf( "\ni/o mapped access\n\n" );
    }
    /* dump register values */
    if( read ){
        printf("Dump memory %lx..%lx\n", startadr, endadr);
        dump_register( type, startadr, size, mapmode );
    }
    /* change register values */
    else if ( write ){
        printf("Write value 0x%lx to memory 0x%lx..0x%lx\n", value, startadr, endadr);
        write_register( type, startadr, size, value, mapmode );
    }
    else {
        usage();
        retval = (-1);
        goto EXIT;
    }

EXIT:
    return retval;
}

/******************************* dump_register ********************************/
/** Routine to dump register values
 *
 *  \param type    \IN  Byte(1), Word(2) or Long(4)
 *  \param p_adr   \IN  physical adress
 *  \param size    \IN  address range
 *  \param io_mem  \IN  I/O or memory mapped mode
 */
static void dump_register( int type, unsigned long p_adr, uint32_t size, int mapmode )
{
    uint32_t i, value=0, buserr;
    char *p_buf = (char*)p_adr;
    char *k, *k0;
    char lbuf[16];
    char *v_kmax = p_buf+size;

    for( k=k0=p_buf; k0<v_kmax; k0+=16 ){
        printf("%08lx+%08lx: ", (unsigned long)p_buf, (k-p_buf));
        switch( type ){
            case ONELONG:
                for( k=k0,i=0; i<16; i+=4, k+=4 ){
                    if( k<v_kmax ) {
                        value = os_access_address(k, 4, 1, 0, &buserr, mapmode);
                        *(unsigned long*)&lbuf[i] = value;
                        printf( "%08x ", value );
                    }
                    else
                        printf( "         " );
                }
            break;
            case ONEWORD:
                for( k=k0,i=0; i<16; i+=2, k+=2 ){
                    if( k<v_kmax ){
                        value = os_access_address(k, 2, 1, 0, &buserr, mapmode) & 0xffff;
                        *(uint16_t*)&lbuf[i] = value;
                        printf( "%04x ", value );
                    }
                    else
                        printf( "     " );
                }
                break;
            case ONEBYTE:
                for( k=k0,i=0; i<16; i++, k++ ){
                    if( k<v_kmax ){
                        value = os_access_address(k, 1, 1, 0, &buserr, mapmode) & 0xff;
			*(uint8_t*)&lbuf[i] = value;
                        printf( "%02x ", value );
                    }
                    else
                        printf( "   " );
                }
                break;
          p_adr += type;
        }
        /* dump ascii */
        for( k=k0,i=0; i<16 && k<v_kmax; i++, k++ ){
            if( (unsigned int)lbuf[i]>=32 && (unsigned int)lbuf[i]<=127 )
                printf("%c", lbuf[i]);
            else
                printf(".");
        }
        printf("\n");
    }
    printf("\n");
}

/******************************* write_register *******************************/
/** Routine to change register values
 *
 *  \param type    \IN  Byte(1), Word(2) or Long(4)
 *  \param p_adr   \IN  physical adress
 *  \param size    \IN  range of address to be written
 *  \param value   \IN  value to write in register
 *  \param io_mem  \IN  I/O or memory mapped mode
 */
static void write_register( int type, unsigned long p_adr, uint32_t size, uint32_t value, int mapmode )
{
    uint32_t buserr;
    unsigned long start_p_adr=p_adr;

    while ( p_adr - start_p_adr < size ) {
      switch( type ){
        case ONELONG:
            os_access_address( p_adr, 4, 0, value, &buserr, mapmode );
            break;
        case ONEWORD:
            os_access_address( p_adr, 2, 0, value, &buserr, mapmode );
            break;
        default: /* ONEBYTE */
            os_access_address( p_adr, 1, 0, value, &buserr, mapmode );
            break;
      }
      p_adr += type;
   }
}

/******************************* os_access_address *******************************/
/** os_access_address - read or write memory
 * returns: nothing on writes
 *                      or read value
 *                      be_flag is set if buserror occurred (not yet implemented)
 */
unsigned long os_access_address(unsigned long physadr, int type, int read, uint32_t value, \
                                int *be_flag, int mapmode )
{
    static int Memdev;
    void *map_adr, *adr;
    long sz = sysconf(_SC_PAGESIZE);

#ifdef MAC_IO_MAPPED_EN
    if( mapmode == IO_MAPPED) {
        access_io_mapped();
        if( read ){
            switch(type){
                case 1:         value = inb(physadr); break;
                case 2:         value = inw(physadr); break;
                case 4:         value = inl(physadr); break;
            }
        }
        else {
            switch(type){
                case 1:         outb((unsigned char)value,      physadr); break;
                case 2:         outw((unsigned short int)value, physadr); break;
                case 4:         outl((unsigned int)value,       physadr); break;
            }
        }
    }
#endif /*MAC_IO_MAPPED_EN*/

    else {
        if( (Memdev = open( "/dev/mem", O_RDWR )) < 0 ){
            perror("can't open /dev/mem");
            goto cleanup;
        }

        /* mmap offset parameter must be a multiple of the page size (_SC_PAGESIZE) */
        map_adr = mmap( NULL, sz, PROT_READ|PROT_WRITE, MAP_SHARED, Memdev, physadr & ~0xfff );

        if( map_adr == MAP_FAILED ){
            perror("Couldn't map physical memory");
            goto cleanup;
        }

        adr = map_adr + (physadr & 0xfff);

        /*--------------+
        | access memory |
        +--------------*/
        if( read ){
            switch(type){
                case 1:         value = *(unsigned char *)adr; break;
                case 2:         value = *(unsigned short *)adr; break;
                case 4:         value = *(unsigned long *)adr; break;
            }
        }
        else {
            switch(type){
                case 1:         *(unsigned char *)adr = value; break;
                case 2:         *(unsigned short *)adr = value; break;
                case 4:         *(unsigned long *)adr = value; break;
            }
            msync( map_adr, 4, MS_SYNC );
        }

        munmap( map_adr, sz );

        close( Memdev );

    }

    cleanup:
        *be_flag = 0;
        return value;
}

int access_io_mapped()
{
    int32_t error = 0;
    if( (error = os_init_io_mapped()) < 0)
        printf("IO-Mapped access not supported or initialization failed\n");
    return error;
}

int32_t os_init_io_mapped()
{
#ifdef MAC_IO_MAPPED_EN
    int error = 0;
    error = iopl(3);
    if(error)
        return -error;
    return 0;
#else
    return -EOPNOTSUPP;
#endif
}
