#include "udp_client.h"



UdpClient::UdpClient(const char* server_ip, int port) {
	if ((this->psp_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Exception::save_error("socket error");
	}
	memset(&this->psp_server_addr, 0, sizeof( struct sockaddr_in));
	this->psp_server_addr.sin_family = AF_INET;
	this->psp_server_addr.sin_port = htons(port);
	this->psp_server_addr.sin_addr.s_addr = inet_addr(server_ip);
}
UdpClient::~UdpClient() {

}
void UdpClient::login(uint32 userid) {

}
void UdpClient::quit_login(uint32 userid) {

}
void UdpClient::p2p_start() {	//p2p2 开始

}
void UdpClient::recved_pack(struct udp_pack, struct sockaddr_in addr) {

}
void UdpClient::recved_data(uint8 * recv_data, uint32 data_size, uint16 task, uint32 userid, uint8 type , uint64 unique = 0x00 ) {

}