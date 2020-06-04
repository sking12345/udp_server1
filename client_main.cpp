#include <iostream>
#include "src/udp_base.h"
#include "src/exception.h"
#include "src/udp_client.h"

int main() {

	UdpClient * udp_base = new UdpClient("127.0.0.1", 9002);
	udp_base->create_send_thread();
	udp_base->create_read_thread();
	udp_base->login(12345);
	udp_base->set_verify_recv_log(true);
	sleep(4);
	const char *str = "12345dddddxx";
	for (int j = 0; j < 5; ++j) {
		sleep(2);
		for (int i = 0; i < 1; ++i) {
			printf("-----:%d\n", j * 5 + i);
			udp_base->send_data((void*)str, strlen(str) + 1, 2345, _TASK_TEXT_, _UDP_TYPE_P2P_);
		}
	}

	sleep(2);
	printf("%s\n", "delete");
	delete udp_base;
	sleep(100);
	return 0;
}