#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <byteswap.h>
#include "ihex2bin.hpp"


typedef union 
{
    struct
    {
        uint8_t byte_count;
        uint16_t address;
        uint8_t record_type;
        union
        {
            uint8_t data[16];
            uint32_t base_address;
        };
    };
    uint8_t input[22];
}Packet_t;


void print(uint8_t *buff,int size)
{
	for(int i =0 ; i < size ;i++)
	{
		printf("%.2x",buff[i]);
	}
	puts("");
}

bool Hex2Bin::ChecksumValidation(uint8_t *record,int size)
{
    uint8_t sum=0;

    int i;

    for(i=0;i<size - 1; i++)
    {
        sum = sum + record[i];
    }
    sum = (~sum)+1;
	//printf("%x ",sum);
    return ((sum == record[size-1]));
	return sum;
}

size_t Hex2Bin::ascii2hex(char *record,size_t len, uint8_t *buffer)
{
    int hex_buffer_size=0;
    uint8_t hex_value = 0;
    int i;
    for(i = 1; i < len-1; i+=2)
    {
        
        record[i] = toupper(record[i]);
        if(record[i]=='\n')
        {
            break;
        }
        hex_value = (((record[i]>='0' && record[i]<='9')?(record[i]-'0'):(record[i]-'A'+10)));
        hex_value *= 16;
        hex_value += (((record[i+1]>='0' && record[i+1]<='9')?(record[i+1]-'0'):(record[i+1]-'A'+10)));
        buffer[hex_buffer_size++] = hex_value;
        //printf("%.2x",hex_value);
    }
    return hex_buffer_size;
}

int Hex2Bin::ParseRecord(uint8_t *buffer, uint32_t *addr)
{
    char *ascii_record = (char *)(malloc(128*sizeof(char)));
    uint8_t *hex_buff = (uint8_t *)(malloc(128*sizeof(uint8_t)));
    int rsize=0,ascii_size=0;
    Packet_t ihex_packet;
    if(hexfile == NULL)
    {
        printf("file not opened\n");
        return -1;
    }
    gets(ascii_record);
    ascii_size = strlen(ascii_record);

    if(ascii_size==0)
    {
        fclose(hexfile);
        return -1;
    }
    
    
    rsize = ascii2hex(ascii_record,ascii_size,hex_buff);

    if(ChecksumValidation(hex_buff,rsize)==true)
    {
        memcpy(ihex_packet.input,hex_buff,rsize-1);

        if(ihex_packet.record_type == 0x00)
        {
            memcpy(buffer,ihex_packet.data,ihex_packet.byte_count);
           *addr = ihex_packet.address;
           return ihex_packet.byte_count;
           
        }

        else if(ihex_packet.record_type == 0x05)
        {
            start_linear_address = __bswap_32(ihex_packet.base_address);
            printf("%x, %x\n",ihex_packet.base_address,start_linear_address);
            return 0;
        }

        else if(ihex_packet.record_type == 0x01){
            return -1;
        }

        else return 0;
    }
}


uint32_t Hex2Bin::find_start_address()
{
    char *ascii_record = (char *)(malloc(128*sizeof(char)));
    uint8_t *hex_buff = (uint8_t *)(malloc(128*sizeof(uint8_t)));
    int rsize=0,ascii_size=0;
    Packet_t ihex_packet;
    
    
    FILE *fp = freopen(filename,"r",stdin);

    if(fp==NULL)
    {
        printf("cannot open hexfile");
    }

    while(1)
    {
        gets(ascii_record);
        ascii_size = strlen(ascii_record);

        if(ascii_size==0)
        {
            break;
        }


        rsize = ascii2hex(ascii_record,ascii_size,hex_buff);

        if(ChecksumValidation(hex_buff,rsize)==true)
        {
            memcpy(ihex_packet.input,hex_buff,rsize-1);

            if(ihex_packet.record_type == 0x05)
            {
                start_linear_address = __bswap_32(ihex_packet.base_address);
                //printf("%x, %x\n",ihex_packet.base_address,start_linear_address);
                fclose(fp);
                return start_linear_address;
            }
        }
    }
    fclose(fp);
    return -1;
}

bool Hex2Bin::OpenHexFile()
{
    printf("opening %s \n",filename);

    hexfile = freopen(filename,"r",stdin);

    if(hexfile==NULL) return false;
    else return true;
}