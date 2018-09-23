#include <bits/stdc++.h>
#include "ihex2bin.hpp"
using namespace std;

typedef struct MemoryBuffer{
    uint8_t buffer[256];
	int size=0;
}MemoryBuffer;
extern void print(uint8_t *buffer, int size);

typedef struct MemoryBuffer{
    uint8_t buffer[256];
	int size;
}MemoryBuffer;

void write_to_flash(char *filename)
{

	Hex2Bin parser(filename);

	uint8_t *data_buffer;
	uint32_t write_addr;
	uint32_t start_addr;
	int data_size;
	MemoryBuffer mem_buff;
	
	mem_buff.size = 0;

	data_buffer = (uint8_t *)malloc(128*sizeof(uint8_t));
	start_addr = parser.find_start_address();

	//printf("start linear address %.8x\n",start_addr);

	parser.OpenHexFile();
	while(1)
	{	memset(data_buffer,0,128);
		data_size = parser.ParseRecord(data_buffer,&write_addr);

		if( data_size == -1){
				break;
		}
		else{
			memcpy(&mem_buff.buffer[mem_buff.size],data_buffer,data_size);
			mem_buff.size += data_size;

			if(mem_buff.size == 256){
				int ret=up_progmem_write(start_addr,mem_buff.buffer,mem_buff.size);
				if(ret < 0){
					printf("cannot write successfully,error:%d\n",errno);
				}
				start_addr += mem_buff.size;
				mem_buff.size=0;
				memset(mem_buff.buffer,0,mem_buff.size);
			}

			if((256 - mem_buff.size)<16)
			{
				int ret=up_progmem_write(start_addr,mem_buff.buffer,mem_buff.size);
				if(ret < 0){
					printf("cannot write successfully,returns %d,error:%d\n",ret,errno);
				}
				start_addr += mem_buff.size;
				mem_buff.size=0;
				memset(mem_buff.buffer,0,mem_buff.size);
			}
		}
	}
	printf("buffer size:%d writing to addr :%.8x\n",mem_buff.size,start_addr);

	//print(mem_buff.buffer,mem_buff.size);
	
	if(mem_buff.size & 1){
		mem_buff.size++;
	}
	
	int ret=up_progmem_write(start_addr,mem_buff.buffer,mem_buff.size);
	
	if(ret < 0)
	{
		printf("cannot write successfully,returns %d,error:%d\n",ret,errno);
	}
}

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
