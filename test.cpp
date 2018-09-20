#include <bits/stdc++.h>
#include "ihex2bin.hpp"
using namespace std;

typedef struct MemoryBuffer{
    uint8_t buffer[256];
	int size=0;
}MemoryBuffer;

extern void print(uint8_t *buff, int size);

int main()
{

	Hex2Bin parser("nuttx.hex");


	uint8_t *data_buffer;
	uint32_t write_addr;
	uint32_t start_addr=0x00;
	int data_size=0;
	MemoryBuffer mem_buff;

	data_buffer = (uint8_t *)malloc(128*sizeof(uint8_t));
	start_addr = parser.find_start_address();
	printf("start linear address %x\n",start_addr);

	parser.OpenHexFile();
	while(1)
	{
		data_size = parser.ParseRecord(data_buffer,&write_addr);
		//printf("%d bytes received\n",data_size);
		if( data_size == -1){
				break;
		}
		else{
			//print(data_buffer,data_size);
			memcpy(&mem_buff.buffer[mem_buff.size],data_buffer,data_size);
			mem_buff.size += data_size;
			
			if(mem_buff.size == 256){
				//print(mem_buff.buffer,256);
				start_addr += mem_buff.size;
				//write buffer to flash
				mem_buff.size=0;
				printf("\nwrite to %x\n",start_addr);
			}

			if((256 - mem_buff.size)<16)
			{
				//print(mem_buff.buffer,256);
				//write buffer to flash, adding next segment may cause overflow
				start_addr += mem_buff.size;
				//printf("buffer size:%d\n",mem_buff.size);
				mem_buff.size=0;
				printf("\nwrite to %x\n",start_addr);
			}
			
		}
	}
	print(mem_buff.buffer,mem_buff.size);
	

}
