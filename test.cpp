#include <bits/stdc++.h>
#include "ihex2bin.hpp"
using namespace std;

#ifndef CONFIG_MAX_LEN_HEX_BUFFER
    #define CONFIG_MAX_LEN_HEX_BUFFER	128
#endif //CONFIG_MAX_LEN_HEX_BUFFER

#ifndef CONFIG_MEMORY_WRITE_BUFFER
	#define CONFIG_MEMORY_WRITE_BUFFER	256
#endif //CONFIG_MEMORY_WRITE_BUFFER

#ifndef CONFIG_HEX_DATA_BYTE_COUNT
	#define CONFIG_HEX_DATA_BYTE_COUNT	16
#endif //CONFIG_HEX_DATA_BYTE_COUNT



/****************************************************************************
 * Public Functions
 ****************************************************************************/

extern void print(uint8_t *buff,int size);
/** Buffer capacity is CONFIG_MEMORY_WRITE_BUFFER bytes is set from Kconfig , data from each records is copied here,
buffer is if the buffer is full write buffer content to flash.
 */
struct MemoryBuffer
{
    uint8_t *buffer; /**< Binary data buffer, data from each intel hex record is copied into buffer. */
	int size;			 /**< Buffer size, if size is 256 bytes or less than 16 bytes are free write the buffer to flash.*/
};


void WriteToFlash(char *filename)
{
	Hex2Bin parser(filename);

	uint8_t *data_buffer;
	uint32_t write_addr=0;
	uint32_t start_addr=0;
	uint32_t buffer_index=0;
	int data_size=0;
	MemoryBuffer mem_buff;
	mem_buff.buffer  = (uint8_t *) malloc(CONFIG_MEMORY_WRITE_BUFFER*sizeof(uint8_t));
	memset(mem_buff.buffer,0xff,CONFIG_MEMORY_WRITE_BUFFER);
	mem_buff.size = 0;
	data_buffer = (uint8_t *)malloc(CONFIG_MAX_LEN_HEX_BUFFER*sizeof(uint8_t));

	start_addr = parser.find_start_address();


	parser.OpenHexFile();

	while(1)
	{
		memset(data_buffer,0xff,CONFIG_MAX_LEN_HEX_BUFFER);
		data_size = parser.ParseRecord(data_buffer,&write_addr);
		buffer_index = write_addr % CONFIG_MAX_LEN_HEX_BUFFER;

		if(data_size == -1){
			break;
		}

		else{
			if(data_size > 0)
			{
				buffer_index = (write_addr%CONFIG_MEMORY_WRITE_BUFFER);
				mem_buff.size = (buffer_index+data_size);
				memcpy(&mem_buff.buffer[buffer_index],data_buffer,data_size);
			}
			//printf("index=%d, size=%d\n",buffer_index,mem_buff.size);

			if(mem_buff.size == CONFIG_MEMORY_WRITE_BUFFER){
				printf("buffer size:%d writing to addr :%.8x\n",mem_buff.size,start_addr);
				//print(mem_buff.buffer,mem_buff.size);
				//write to flash memory
				//int ret=up_progmem_write(start_addr,mem_buff.buffer,mem_buff.size);
				//if(ret < 0){
				//	printf("cannot write successfully,error:%d\n",errno);
				//}
				start_addr += mem_buff.size;
				mem_buff.size=0;
				memset(mem_buff.buffer,0xff,CONFIG_MEMORY_WRITE_BUFFER);
			}
			if((CONFIG_MEMORY_WRITE_BUFFER - mem_buff.size)<CONFIG_HEX_DATA_BYTE_COUNT)
			{
				printf("buffer size:%d writing to addr :%.8x\n",mem_buff.size,start_addr);
				//print(mem_buff.buffer,mem_buff.size);
				//int ret=up_progmem_write(start_addr,mem_buff.buffer,mem_buff.size);
				//if(ret < 0){
				//	printf("cannot write successfully,returns %d,error:%d\n",ret,errno);
				//}
				start_addr += mem_buff.size;
				mem_buff.size=0;
				memset(mem_buff.buffer,0xff,CONFIG_MEMORY_WRITE_BUFFER);
			}
		}
	}
	printf("buffer size:%d writing to addr :%.8x\n",mem_buff.size,start_addr);
	//print(mem_buff.buffer,mem_buff.size);

	if(mem_buff.size & 1){
		mem_buff.size++;
	}

	//int ret=up_progmem_write(start_addr,mem_buff.buffer,mem_buff.size);
	//if(ret < 0)
	//{
	//	printf("cannot write successfully,returns %d,error:%d\n",ret,errno);
	//}
	free(data_buffer);
	free(mem_buff.buffer);
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
