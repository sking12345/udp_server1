#include <iostream>
#include <string>
#include "core/p2p_base.h"
#include "core/types.h"
#include "core/memory_mgt.h"
#include <stdlib.h>
#include <string.h>




int main() {

	/*
	 * udp 连接
	 */
	p2p_base *base =  new p2p_base();
	int udp_server_fd = base->conect_server_udp(9009);

	/*
	 * tcp 连接
	 */
	int tcp_server_fd = base->conect_server_tcp(9009);



	printf("%d\n", udp_server_fd);
	printf("%d\n", tcp_server_fd);



	return 0;
}

