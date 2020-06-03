#include <iostream>
#include "core/udp_base.h"
#include "src/exception.h"
#include "src/udp_client.h"

int main() {

	UdpBase * udp_base = new UdpClient("127.0.0.1", 9002);
	udp_base->create_send_thread();
	udp_base->create_read_thread();
	const char *str = "12345dddddxx";
	std::cout << strlen(str) << std::endl;
	for (int j = 0; j < 1; ++j) {
		for (int i = 0; i < 1; ++i) {
			base->send_data((void*)str, strlen(str) + 1, 2345, _TASK_TEXT_, _UDP_PACK_P2P_);
		}
	}
	sleep(5);
	delete base;
	return 0;
}