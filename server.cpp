#include <iostream>
#include <string>
#include "core/p2p_udp.h"
#include "core/types.h"
#include "core/memory_mgt.h"
#include <stdlib.h>
#include <string.h>




int main()
{

	memory_mgt * mgt = new memory_mgt();
	const char* str  = "xxddxxddxxddxxddxxddxxddxxddxxddxxddxxdds";
	printf("%s\n", "start");
	sleep(5);
	printf("%s\n", "---");
	for (int i = 0; i < 1000000; ++i)
	{
		mgt->new_memory(strlen(str),i,(uint8*)str);
		
		// mgt->delete_memory(i);
	}
	printf("%s\n","start free" );
	sleep(5);
	for (int i = 0; i < 1000000; ++i)
	{
		mgt->delete_memory(i);
	}
	printf("%s\n","end free" );
	delete mgt;
	sleep(5);
	return 0;
}

