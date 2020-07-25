#ifndef _MEMORYY_MGT_H_
#define _MEMORYY_MGT_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <list>
#include "types.h"

using namespace std;

typedef struct memory_struct
{	
	
	uint32 data_size;
	uint8 	data[0];
}MEMORY_STRUCT,*PMEMORY_STRUCT;

class memory_mgt
{
	
private:
	static memory_mgt *instance;
	map < string, void* > str_map;
	map < uint32, void* > int_map;
	uint64 malloced_size;	//已分配了多大内存
	uint64 malloced_num;	//分配了多少次数
	memory_mgt(){};
public:
	static memory_mgt* getInstance();
	static void end_memory();

	~memory_mgt();

	/**
		size:大小
		alias: 别名	
	*/
	struct memory_struct *new_memory(uint32 size,string alias,uint8 *data=NULL,uint8 type = 1);	//分配内存,0:如果查到存在,则返回null,1:直接覆盖
	struct memory_struct *new_memory(uint32 size,uint32 alias,uint8 *data=NULL,uint8 type = 1);	//分配内存
	struct memory_struct *find_memory(string alias);
	struct memory_struct *find_memory(uint32 alias);

	void delete_memory(string alias);	//释放内存
	void delete_memory(uint32 alias);	//释放内存
	
};

#endif


















