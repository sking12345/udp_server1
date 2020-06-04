#include <iostream>
#include <signal.h>
#include "src/exception.h"
#include "src/udp_base.h"
#include "src/udp_server.h"



static UdpBase * udp_base = NULL;


void tick(int signo) {
	if (udp_base != NULL) {
		udp_base->execute_timer();
	}
	alarm(1);
}

int main() {

	// signal(SIGALRM, tick);
	// alarm(1); // 1s的周期心跳

	udp_base = new UdpServer(9002);
	udp_base->create_send_thread();
	// udp_base->create_read_thread();
	udp_base->recv_start();

	delete udp_base;
	udp_base = NULL;


	return 0;
}




