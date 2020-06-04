#include "udp_server.h"


UdpServer::UdpServer(int port) {
	if ((this->psp_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Exception::save_error("socket error");
	}
	memset(&(this->psp_server_addr), 0, sizeof(struct sockaddr_in));
	this->psp_server_addr.sin_family = AF_INET;
	this->psp_server_addr.sin_port = htons(port);
	this->psp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->psp_socket_fd, (struct sockaddr *)&this->psp_server_addr, sizeof(struct sockaddr_in)) < 0) {
		Exception::save_error("bind error");
		exit(0);
	}
	this->user_id = 0x00;

}
UdpServer::~UdpServer() {

}


void UdpServer::user_exits_call(struct udp_pack) {	//某个用户端不存在

}

void UdpServer::recved_pack(struct udp_pack recv_pack, struct sockaddr_in from_addr) {
	printf("%s\n", "xxxddd");
	if (recv_pack.task == _TASK_LOGIN_) {	//登录操作
		struct extranet_addr *login_addr =  this->save_addr(recv_pack.from_id, from_addr, recv_pack.type);
		memset(recv_pack.data, 0x00, _UDP_DATA_SIZE_);
		memcpy(recv_pack.data, login_addr, sizeof(struct extranet_addr));
		recv_pack.task = _TASK_LOGIN_SUCCESS_;
		Exception::save_info("_TASK_LOGIN_");
		this->psp_send(&recv_pack, from_addr);
	} else if (recv_pack.task == _TASK_QUITE_) { //退出登录
		Exception::save_info("_TASK_QUITE_");
		this->remove_addr(recv_pack.from_id);
	} else if (recv_pack.task == _TASK_GET_ADDR_) {	//获取用户信息
		struct extranet_addr *login_addr =  this->get_already_addrs(recv_pack.send_id);
		memset(recv_pack.data, 0x00, _UDP_DATA_SIZE_);
		memcpy(recv_pack.data, login_addr, sizeof(struct extranet_addr));
		recv_pack.task = _TASK_BACK_ADDR_;
		this->psp_send(&recv_pack, from_addr);
		Exception::save_info("_TASK_GET_ADDR_");

	} else if (recv_pack.task == _TASK_HEARTBEAT_) {	//心跳包
		struct extranet_addr *login_addr =  this->get_already_addrs(recv_pack.from_id);
		login_addr->last_time = this->get_mstime();
		Exception::save_info("_TASK_HEARTBEAT_");
	} else if (recv_pack.task == _TASK_VERIFICATION_P2P_) {
		struct extranet_addr *login_addr =  this->get_already_addrs(recv_pack.from_id);
		if (login_addr != NULL) {
			struct sockaddr_in send_addr = {0};
			send_addr.sin_family = AF_INET;
			send_addr.sin_port = htons(login_addr->port);
			send_addr.sin_addr.s_addr = inet_addr((char*)login_addr->IPdotdec);
			recv_pack.task = _TASK_VERIFICATION_P2P_;
			this->psp_send(&recv_pack, send_addr);
		}
	} else {
		printf("recv_data:%s\n", recv_pack.data);
		struct extranet_addr *login_addr =  this->get_already_addrs(recv_pack.send_id);
		if (login_addr != NULL) {
			if (login_addr->type == _UDP_TYPE_P2P_) {	//某用户端是p2p
				struct sockaddr_in send_addr = {0};
				send_addr.sin_family = AF_INET;
				send_addr.sin_port = htons(login_addr->port);
				send_addr.sin_addr.s_addr = inet_addr((char*)login_addr->IPdotdec);
				this->psp_send(&recv_pack, send_addr);
			} else {
				this->psp_send(&recv_pack, login_addr->addr);
			}
		} else {
			recv_pack.task = _TASK_USER_EXITE_;
			this->psp_send(&recv_pack, from_addr);
			this->user_exits_call(recv_pack);
		}
	}
}































