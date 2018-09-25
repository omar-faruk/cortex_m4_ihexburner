#ifndef __INCLUDE_IHEX2BIN_H
#define __INCLUDE_IHEX2BIN_H

//#include <nuttx/config.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>




class Hex2Bin{

    private:
        uint32_t base_addr;
        uint32_t end_addr;
        uint32_t curr_addr;
        char filename[32];
        FILE *hexfile;

        size_t ascii2hex(char *record,size_t len, uint8_t *buffer);
    public:
        uint32_t start_linear_address;

        Hex2Bin(char *fname)
        {
            strcpy(filename,fname);
            //printf(filename);
            start_linear_address = 0x00000000;
            base_addr = 0x00000000;
            end_addr = 0x00000000;
            hexfile = NULL;
        }
        Hex2Bin()
        {
            base_addr = 0x00000000;
            end_addr = 0x00000000;
            hexfile = NULL;
		}

        /*~Hex2Bin()
        {
            fclose(hexfile);
        }*/
        bool OpenHexFile();
        int ParseRecord(uint8_t *buffer, uint32_t *addr);
        bool OpenFile(char *fname);
        bool ChecksumValidation(uint8_t *record, int record_length);
        uint32_t find_start_address();

};

#endif //__INCLUDE_IHEX2BIN_H
