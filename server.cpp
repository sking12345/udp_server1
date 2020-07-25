#include <iostream>
#include <string>
#include "core/p2p_udp.h"
#include "core/types.h"
#include "core/memory_mgt.h"
#include <stdlib.h>
#include <string.h>




int main()
{

	memory_mgt * mgt = memory_mgt::getInstance();
	const char* str  = "xxddxxddxxddxxddxxddxxddxxddxxddxxddxxdds";
	struct memory_struct* str_prt = mgt->new_memory(strlen(str),"str",(uint8*)str);
	printf("%s\n", str_prt->data);
	mgt->delete_memory("str");
	struct memory_struct* str_prt1 = mgt->find_memory("str");
	if(str_prt1 != NULL)
	{
		printf("%s\n", str_prt1->data);
	}else{
		printf("%s\n", "eroor");
	}
	
	memory_mgt::end_memory();
	sleep(5);
	return 0;
}

