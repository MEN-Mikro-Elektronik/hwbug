/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: display.c
 *      Project: HwBug
 *
 *      Author: GLeonhardt 
 *
 *  Description: dump memory contents
 *                      
 *                      
 *     Required: - 
 *     Switches: - 
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  display_data( argc, argv )          
 *  
 *---------------------------------------------------------------------------
 * Copyright 1993-2019, MEN Mikro Elektronik GmbH
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

#include "hwbug.h"

int hex_dump(unsigned long adr, int32_t n, int32_t fmt); 

int display_data( int argc, char **argv)
{
	static int o_size = 4;		/* default=long accesses */
	unsigned long adr, count=256;
	static unsigned long last_adr, temp_adr;
	char *cmdarg = argv[0];
	
	/*--------------+
	| get options   |
	+--------------*/
	if( cmdarg ){
		cmdarg++;							/* skip 'D' */

		while( !isspace(*cmdarg) && *cmdarg ){
			switch( *cmdarg ){
			case 'B':					/* byte accesses */
				o_size = 1;
				break;
			case 'W':					/* word accesses */
				o_size = 2;
				break;
			case 'L':					/* long accesses */
				o_size = 4;
				break;
			}

			cmdarg++;
		}
	}

	/*--------------+
	| get address   |
	+--------------*/
	if( argc == 0 ){
		adr = temp_adr;
	}
	else {
		if( argc > 1 ){
			if( sscanf(argv[1],"%lx", &adr ) < 1 )
				return 1;		
			if( argc > 2 ){
				if( sscanf( argv[2],"%lx", &count ) < 1 )
					return 1;		
			}
		}
		else
			adr = last_adr;
				
		temp_adr = last_adr = adr;
	}

	/*--------------+
	| dump memory   |
	+--------------*/
	if (hex_dump(adr,count,o_size))
		return(0);

	temp_adr += count;
	return 0;
}

int hex_dump(adr,n,fmt)
unsigned long adr;
int32_t n,fmt; 
{
	extern int control_c;
    uint32_t i, buserr, value;
	char *buf = (char*)adr;
    char  *k, *k0, *kmax = buf+n;
	char  lbuf[16];
    
    for (k=k0=buf; k0<kmax; k0+=16) {   
        printf("%08lx+%08x: ", (unsigned long)buf,  (uint32_t)(k-buf));

        switch(fmt) {                                       			/* dump hex: */
           case 4 : for (k=k0,i=0; i<16 && !control_c; i+=4, k+=4) {    /* long  */
                       if (k<kmax) {  
						   value = os_access_address(k,4,1,0,&buserr) & 0xffffffff;
						   if (buserr) return(1);
						   *(unsigned long*)&lbuf[i] = value;
						   printf("%08x ",value);
					   }
                       else         
						   printf("         ");
				    }
                    break;
           case 2 : for (k=k0,i=0; i<16 && !control_c; i+=2, k+=2) {     /* word  */
                       if (k<kmax) { 
						   value = os_access_address(k,2,1,0,&buserr) & 0xffff;
						   if (buserr) return(1);
						   *(uint16_t*)&lbuf[i] = value;
						   printf("%04x ",value);
					   }
                       else
						   printf("     ");
				    }
                    break;
           default: for (k=k0,i=0; i<16 && !control_c; i++, k++) {    /* byte  */
                       if (k<kmax) {
						   value = os_access_address(k,1,1,0,&buserr) & 0xff;
						   if (buserr) return(1);
						   *(uint8_t*)&lbuf[i] = value;
						   printf("%02x ",value);
					   }
                       else
						   printf("   ");
				    }
        }

        for (k=k0,i=0; i<16 && k<kmax; i++, k++) {               /* dump ascii */
            if ( (unsigned int) lbuf[i]>=32 && (unsigned int) lbuf[i]<=127 )
               printf("%c", lbuf[i]);
            else
               printf(".");
		}

        printf("\n");
    }

	if( control_c ) control_c = 0;
	return(0);
}
