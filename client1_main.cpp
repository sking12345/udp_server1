#include <iostream>
#include "src/udp_base.h"
#include "src/exception.h"
#include "src/udp_client.h"

int main() {

	UdpClient * udp_base = new UdpClient("127.0.0.1", 9002);
	udp_base->create_send_thread();
	udp_base->create_read_thread();
	// udp_base->login(2345);
	udp_base->p2p_login(2345);
	udp_base->set_verify_recv_log(true);


	// sleep(1);
	// const char *str = "12345dddddxx";
	// for (int j = 0; j < 1; ++j) {
	// 	for (int i = 0; i < 1; ++i) {
	// 		udp_base->send_data((void*)str, strlen(str) + 1, 2345, _TASK_TEXT_, _UDP_TYPE_P2P_);
	// 	}
	// }

	sleep(500);
	delete udp_base;
	return 0;
}