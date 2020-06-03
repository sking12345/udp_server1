#include "udp_server.h"


UdpServer::UdpServer(int port) {
	this->extranet_port = port;
	if ((this->psp_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Exception::save_error("socket error");
	}
	memset(&(this->psp_server_addr), 0, sizeof(struct sockaddr_in));
	this->psp_server_addr.sin_family = AF_INET;
	this->psp_server_addr.sin_port = htons(this->extranet_port);
	this->psp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->psp_socket_fd, (struct sockaddr *)&this->psp_server_addr, sizeof(struct sockaddr_in)) < 0) {
		Exception::save_error("bind error");
		exit(0);
	}
	this->user_id = 0x00;

}
UdpServer::~UdpServer() {

}
void UdpServer::recved_pack(struct udp_pack, struct sockaddr_in addr) {
	printf("%s\n", "xxx");
}