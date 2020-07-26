#include "memory_mgt.h"

memory_mgt* memory_mgt::instance = NULL;

memory_mgt *memory_mgt::getInstance() {
	if (instance == NULL) {
		instance = new memory_mgt();
	}
	return instance;
}

memory_mgt::~memory_mgt() {
	printf("%s\n", "~memory_mgt");
}

void *memory_mgt::new_memory(uint32 size, string alias, uint8 *data, uint8 type) {
	void *ptr = NULL;
	map < string, void* >::iterator iter = this->str_map.find(alias);
	if (iter == this->str_map.end()) {
		ptr = (void*)malloc(size);
	} else {
		ptr = (void *)iter->second;
		if (type == true) {	//直接修改内存大小覆盖
			free(ptr);
			ptr = NULL;
			ptr = (void*)malloc(size);
		} else {
			return NULL;
		}
	}
	memset(ptr, 0, size);

	if (data != NULL) {
		memcpy(ptr, data, size);
	}
	this->str_map.insert( pair< string, void* >(alias, (void*)ptr));
	return ptr;
}
void *memory_mgt::new_memory(uint32 size, uint32 alias, uint8 *data, uint8 type) {
	void *ptr = NULL;
	map < uint32, void* >::iterator iter = this->int_map.find(alias);
	if (iter == this->int_map.end()) {
		ptr = (void*)malloc(size);
	} else {
		ptr = (void *)iter->second;
		if (type == true) {	//直接修改内存大小覆盖
			free(ptr);
			ptr = NULL;
			ptr = (void*)malloc(size);
		} else {
			return NULL;
		}
	}
	memset(ptr, 0, size);
	if (data != NULL) {
		memcpy(ptr, data, size);
	}
	this->int_map.insert(pair<uint32, void*>(alias, ptr));
	return ptr;
}


void *memory_mgt::find_memory(string alias) {
	map < string, void* >::iterator iter = this->str_map.find(alias);
	if (iter == this->str_map.end()) {
		return NULL;
	}
	return iter->second;
}
void *memory_mgt::find_memory(uint32 alias) {
	map < uint32, void* >::iterator iter = this->int_map.find(alias);
	if (iter == this->int_map.end()) {
		return NULL;
	}
	return iter->second;
}

void memory_mgt::delete_memory(string alias) {
	map < string, void* >::iterator iter = this->str_map.find(alias);
	if (iter != this->str_map.end()) {
		void *ptr = (void*)iter->second;
		free(ptr);
		ptr = NULL;
		this->str_map.erase(iter);
	}
}
void memory_mgt::delete_memory(uint32 alias) {
	map < uint32, void* >::iterator iter = this->int_map.find(alias);
	if (iter != this->int_map.end()) {
		void *ptr = (void*)iter->second;
		free(ptr);
		ptr = NULL;
		this->int_map.erase(iter);
	}

}

void memory_mgt::end_memory() {
	delete instance;
	instance = NULL;
}
















