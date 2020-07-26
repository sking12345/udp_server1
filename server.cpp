#include <iostream>
#include <string>
#include "core/p2p_base.h"
#include "core/types.h"
#include "core/memory_mgt.h"
#include <stdlib.h>
#include <string.h>




int main() {

	/*
	 * udp 服务
	 */
	p2p_base *base =  new p2p_base();
	int udp_server_fd = base->create_udp_server(9009);


	/*
	 * tcp 服务
	 */
	int tcp_server_fd = base->create_tcp_server(9009);





	printf("%d\n", udp_server_fd);
	printf("%d\n", tcp_server_fd);

	sleep(5);

	return 0;
}

