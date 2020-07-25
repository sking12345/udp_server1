#include "memory_mgt.h"

// memory_mgt* memory_mgt::instance = NULL;

// memory_mgt *memory_mgt::getInstance(){
// 	if (instance == NULL) {
//         instance = new memory_mgt();
//     }
//     return instance;
// }
memory_mgt::memory_mgt()
{

}
memory_mgt::~memory_mgt()
{
	
}

struct memory_struct *memory_mgt::new_memory(uint32 size,string alias,uint8 *data,uint8 type)
{
	struct memory_struct *ptr = NULL;
	map < string, void* >::iterator iter = this->str_map.find(alias);
	if(iter == this->str_map.end()){
		ptr = (struct memory_struct*)malloc(sizeof(struct memory_struct)+size);
	}else{
		ptr = (struct memory_struct *)iter->second;
		if(type == true)	//直接修改内存大小覆盖
		{
			free(ptr);
			ptr = NULL;
			ptr = (struct memory_struct*)malloc(sizeof(struct memory_struct)+size);
		}else{
			return NULL;
		}
	}
	memset(ptr,0,sizeof(struct memory_struct)+size);

	if(data!=NULL)
	{
		memcpy(ptr->data,data,size);
	}
	this->str_map.insert( pair< string, void* >(alias,(void*)ptr));
	return ptr;
}
struct memory_struct *memory_mgt::new_memory(uint32 size,uint32 alias,uint8 *data,uint8 type)
{
	struct memory_struct *ptr = NULL;
	map < uint32, void* >::iterator iter = this->int_map.find(alias);
	if(iter == this->int_map.end()){
		ptr = (struct memory_struct*)malloc(sizeof(struct memory_struct)+size);
	}else{
		ptr = (struct memory_struct *)iter->second;
		if(type == true)	//直接修改内存大小覆盖
		{
			free(ptr);
			ptr = NULL;
			ptr = (struct memory_struct*)malloc(sizeof(struct memory_struct)+size);
		}else{
			return NULL;
		}
	}
	memset(ptr,0,sizeof(struct memory_struct)+size);
	if(data!=NULL)
	{
		memcpy(ptr->data,data,size);
	}
	this->int_map.insert(pair<uint32,struct memory_struct*>(alias,ptr));
	return ptr;
}

struct memory_struct *memory_mgt::find_memory(string alias){
	map < string, void* >::iterator iter = this->str_map.find(alias);
	if(iter == this->str_map.end()){
		return NULL;
	}
	return (struct memory_struct*)iter->second;
}
struct memory_struct *memory_mgt::find_memory(uint32 alias){
	map < uint32, void* >::iterator iter = this->int_map.find(alias);
	if(iter == this->int_map.end()){
		return NULL;
	}
	return (struct memory_struct*)iter->second;
}

void memory_mgt::delete_memory(string alias)
{
	map < string, void* >::iterator iter = this->str_map.find(alias);
	if(iter != this->str_map.end()){
		struct memory_struct *ptr = (struct memory_struct*)iter->second;
		free(ptr);
		ptr = NULL;
		this->str_map.erase(iter);
	}
}
void memory_mgt::delete_memory(uint32 alias)
{
	map < uint32, void* >::iterator iter = this->int_map.find(alias);
	if(iter != this->int_map.end()){
		struct memory_struct *ptr = (struct memory_struct*)iter->second;
		free(ptr);
		ptr = NULL;
		this->int_map.erase(iter);
	}
	
}
















