#include "udp_client.h"



UdpClient::UdpClient(const char* server_ip, int port) {
	if ((this->psp_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Exception::save_error("socket error");
	}
	memset(&this->psp_server_addr, 0, sizeof( struct sockaddr_in));
	this->psp_server_addr.sin_family = AF_INET;
	this->psp_server_addr.sin_port = htons(port);
	this->psp_server_addr.sin_addr.s_addr = inet_addr(server_ip);
	this->extranet_port = 0;
	this->server_port = port;
	memset(&this->server_ip, 0x00, 20);
	memcpy(&this->server_ip, server_ip, 20);
	this->login_status = false;

}
UdpClient::~UdpClient() {

}


void UdpClient::login(uint32 userid) {	//登录
	this->user_id = userid;
	struct udp_pack send_pack = {0x00};
	send_pack.task = _TASK_LOGIN_;
	send_pack.type = _UDP_TYPE_PSP_;		//表示是否启用p2p 模式
	send_pack.version = _VERSION_;
	send_pack.from_id = userid;
	send_pack.time = this->get_mstime();
	this->psp_send(&send_pack, this->psp_server_addr);
}
void UdpClient::p2p_login(uint32 userid) {
	this->user_id = userid;
	struct udp_pack send_pack = {0x00};
	send_pack.task = _TASK_LOGIN_;
	send_pack.type = _UDP_TYPE_P2P_;		//表示是否启用p2p 模式
	send_pack.version = _VERSION_;
	send_pack.from_id = userid;
	send_pack.time = this->get_mstime();
	this->psp_send(&send_pack, this->psp_server_addr);

}
void UdpClient::quit_login(uint32 userid) {	//退出登录
	this->user_id = userid;
	struct udp_pack send_pack = {0x00};
	send_pack.task = _TASK_QUITE_;
	send_pack.version = _VERSION_;
	send_pack.from_id = userid;
	send_pack.time = this->get_mstime();
	this->psp_send(&send_pack, this->psp_server_addr);
}

void UdpClient::p2p_start() {	//p2p2 开始

	printf("IP:%s,port:%d\n", this->extranet_ip, this->extranet_port);
	/*先尝试p2p 监听本机,然后关闭读线程*/
	if ((this->p2p_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		Exception::save_error("socket error");
		return ;
	}
	memset(&(this->p2p_server_addr), 0, sizeof(struct sockaddr_in));
	this->p2p_server_addr.sin_family = AF_INET;
	this->p2p_server_addr.sin_port = htons(this->extranet_port);
	this->p2p_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->p2p_socket_fd, (struct sockaddr *)&this->p2p_server_addr, sizeof(struct sockaddr_in)) < 0) {
		Exception::save_error("bind error");
		return ;
	}
	this->read_thread_status = _THREAD_EXT_;
	close(this->psp_socket_fd);
	this->psp_socket_fd = 0;

	printf("server_ip:%s\n", this->server_ip );
	printf("server_port:%d\n", this->server_port );
	printf("p2p_socket_fd:%d\n", this->p2p_socket_fd);
	this->close_read_thread();

	this->create_read_thread();		//创建p2p 包的读线程

	/*发送p2p 验证包*/
	struct udp_pack send_pack = {0x00};
	send_pack.task = _TASK_VERIFICATION_P2P_;
	send_pack.type = _UDP_TYPE_P2P_;		//表示是否启用p2p 模式
	send_pack.version = _VERSION_;
	send_pack.from_id = this->user_id;
	send_pack.time = this->get_mstime();
	this->p2p_send(&send_pack, this->psp_server_addr);


}


void UdpClient::recved_pack(struct udp_pack recved_pack, struct sockaddr_in from_addr) {

	if (recved_pack.task == _TASK_LOGIN_SUCCESS_) {	//登录成功
		this->login_status = true;
		struct extranet_addr my_addr = {0x00};
		memcpy(&my_addr, recved_pack.data, sizeof(struct extranet_addr));
		this->extranet_port = my_addr.port;
		memcpy(&this->extranet_ip, my_addr.IPdotdec, 20);
		if (recved_pack.type == _UDP_TYPE_P2P_) {
			this->p2p_start();
		}
		return;
	} else if (recved_pack.task == _TASK_LOGIN_FAIL_) {	//登录失败
		Exception::save_info("_TASK_LOGIN_FAIL_");
		this->login_status = false;
		return;
	} else if (recved_pack.task == _TASK_BACK_ADDR_) {	//返回某个用户的外网地址
		Exception::save_info("_TASK_BACK_ADDR_");
		struct extranet_addr user_addr = {0x00};
		memcpy(&user_addr, recved_pack.data, sizeof(struct extranet_addr));
		printf("recved_pack.send_id:%d\n", recved_pack.send_id );
		this->save_addr(recved_pack.send_id, user_addr.addr, user_addr.type);
		return;
	} else if (recved_pack.task == _TASK_EXEIT_PACK_) { //包不存在
		Exception::save_info("_TASK_EXEIT_PACK_");
		uint64 unique = recved_pack.time * 10000 + recved_pack.unique;
		this->free_send_map(unique);
		return;
	} else if (recved_pack.task == _TASK_END_) {
		uint64 unique = recved_pack.time * 10000 + recved_pack.unique;
		this->free_send_map(unique);
		Exception::save_info("_TASK_END_");
		return;
	} else if (recved_pack.task == _TASK_USER_EXITE_) {	//如果用户未登录,数据已传入服务器端
		Exception::save_info("_TASK_USER_EXITE_");
		uint64 unique = recved_pack.time * 10000 + recved_pack.unique;

		this->free_send_map(unique);
		return;
	} else if (recved_pack.task == _TASK_HEARTBEAT_) {
		Exception::save_info("_TASK_HEARTBEAT_");	//回复服务器,表示还存活
		if (this->p2p_socket_fd > 0) {
			this->p2p_send(&recved_pack, this->psp_server_addr);	//通过p2p 文件描述符,发送数据到服务器
		} else {
			this->psp_send(&recved_pack, this->psp_server_addr);	//通过psp 文件描述符，发送到服务器
		}
		return;
	} else if (recved_pack.task == _TASK_VERIFICATION_P2P_) {	//p2p 验证ok
		Exception::save_info("_TASK_VERIFICATION_P2P_");
		return;
	}

	uint64 unique = recved_pack.time * 10000 + recved_pack.unique;
	int data_pack_num = this->get_pack_num(recved_pack.max_size);
	uint8 *recv_data = NULL;
	pthread_mutex_lock(&(this->read_mutex));
	std::map<uint32, std::map<uint64, uint8*> >::iterator recv_map_list_iter;
	std::map<uint64, uint8*> user_recv_map_list;
	std::map<uint64, uint8*>::iterator user_recv_map_list_iter;
	recv_map_list_iter = this->recv_map_list.find(recved_pack.from_id);
	if (recv_map_list_iter == this->recv_map_list.end()) {
		recv_data =  (uint8*)malloc(data_pack_num * _UDP_DATA_SIZE_);
		user_recv_map_list.insert(std::pair<uint64, uint8*>(unique, recv_data));
		this->recv_map_list.insert(std::pair<uint32, std::map<uint64, uint8*> >(recved_pack.from_id, user_recv_map_list));
	} else {
		user_recv_map_list = recv_map_list_iter->second;
		user_recv_map_list_iter = user_recv_map_list.find(unique);
		if (user_recv_map_list_iter == user_recv_map_list.end()) {
			recv_data =  (uint8*)malloc(data_pack_num * _UDP_DATA_SIZE_);
			this->recv_map_list[recved_pack.from_id][unique] = recv_data;
		} else {
			recv_data = user_recv_map_list_iter->second;
		}
	}
	printf("%s\n", recved_pack.data);
	memcpy(recv_data + _UDP_DATA_SIZE_ * recved_pack.sequence, recved_pack.data, _UDP_DATA_SIZE_);
	pthread_mutex_unlock(&(this->read_mutex));
	if (this->recv_map_log[recved_pack.from_id][unique].size() == data_pack_num) {
		this->recved_data(recv_data, recved_pack.max_size, recved_pack.task, recved_pack.from_id, unique);
		this->free_recved_data(recved_pack.from_id, unique);
		recved_pack.send_id = recved_pack.from_id;
		recved_pack.from_id = this->user_id;
		recved_pack.task = _TASK_END_;
		this->p2p_send(&recved_pack, from_addr);
		//this->clear_recved_log(recved_pack.from_id, unique);

	}
}

void UdpClient::recved_data(uint8 * recv_data, uint32 data_size, uint16 task, uint32 userid, uint8 type , uint64 unique  ) {
	printf("recv_data::%s\n", recv_data);
}


































