#include <iostream>
#include <string>
#include "core/socket_server.h"
#include "core/types.h"
#include "core/memory_mgt.h"
#include <stdlib.h>
#include <string.h>




int main() {

	/*
	 * udp 服务
	 */
	socket_server * server = new socket_server(9009, TCP_SERVER);
	server->start();
	sleep(5);

	return 0;
}

