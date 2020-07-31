#include "p2p_base.h"



p2p_base::p2p_base() {
	this->from_id 		= 0;
	this->server_udp_fd = 0;
	this->server_tcp_fd = 0;
	this->udp_connect_fd = 0;
	this->tcp_connect_fd = 0;
	this->wait_send_num  = 0;		//等待发送的数据量
	this->close_send_thread = 0;	//是否关闭写数据线程
	this->send_task_unique	  = 0;

	pthread_cond_init(&(this->send_cond), NULL);
	pthread_mutex_init(&(this->send_mutex), NULL);


}
p2p_base::~p2p_base() {

	pthread_cond_destroy(&(this->send_cond));
	pthread_mutex_destroy(&(this->send_mutex));
}

// //接受到tcp 数据
// void p2p_base::recv_tcp_read() {

// 	socklen_t len = sizeof(struct sockaddr);
// 	int recv_size = sizeof(struct pack);
// 	int recv_num = 0;
// 	int recv_socket_fd = this->read_thread_socket_id;
// 	while (1) {
// 		struct sockaddr_in client_addr = {0x00};
// 		struct pack recv_pack = {0};
// 		int new_fd = accept(recv_socket_fd, (struct sockaddr*)(&client_addr), &len);
// 		printf("server get connection from %s\n", inet_ntoa(client_addr.sin_addr));
// 		int recv_num = recv(new_fd, &recv_pack, sizeof(struct pack), 0);
// 		if (recv_num <= 0) {
// 			close(new_fd);
// 		}

// 	}

// }
//接受到udp 数据
// void p2p_base::recv_udp_read() {
// 	socklen_t len = sizeof(struct sockaddr_in);
// 	int recv_size = sizeof(struct pack);
// 	int recv_num = 0;
// 	int recv_socket_fd = this->read_thread_socket_id;
// 	while (1) {
// 		struct sockaddr_in client_addr = {0x00};
// 		struct pack recv_pack = {0};
// 		recv_num = recvfrom(recv_socket_fd, &recv_pack, recv_size, 0, (struct sockaddr *)&client_addr, (socklen_t *)&len);
// 		if (recv_num <= 0) {
// 			return;
// 		}

// 	}
// }

//链接服务器
uint32 p2p_base::conect_server_tcp(string server_ip, uint32 port) {

	if ( (this->tcp_connect_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	memset(&this->tcp_connect_addr, 0, sizeof(this->tcp_connect_addr));
	this->tcp_connect_addr.sin_family = AF_INET;
	this->tcp_connect_addr.sin_port = htons(port);
	if ( inet_pton(AF_INET, server_ip.c_str(), &this->tcp_connect_addr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", server_ip.c_str());
		return 0;
	}
	if ( connect(this->tcp_connect_fd, (struct sockaddr*)&this->tcp_connect_addr, sizeof(this->tcp_connect_addr)) < 0) {
		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	return 1;
}
//链接服务器
uint32 p2p_base::conect_server_udp(string server_ip, uint32 port) {

	this->udp_connect_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	memset(&this->udp_connect_addr, 0, sizeof( struct sockaddr_in));
	this->udp_connect_addr.sin_family = AF_INET;
	this->udp_connect_addr.sin_port = htons(port);
	this->udp_connect_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
	return 1;
}



//监听本机某个端口，用于将本机作为服务器，tcp，监听
uint32 p2p_base::create_tcp_server(uint32 port) {

	if ( (this->server_tcp_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	memset(&this->tcp_server_addr, 0, sizeof(this->tcp_server_addr));
	this->tcp_server_addr.sin_family = AF_INET;
	this->tcp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	this->tcp_server_addr.sin_port = htons(port);

	if ( bind(this->server_tcp_fd, (struct sockaddr*)&this->tcp_server_addr, sizeof(this->tcp_server_addr)) == -1) {
		printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	if ( listen(this->server_tcp_fd, 10) == -1) {
		printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	this->read_thread_socket_id = this->server_tcp_fd;
	pthread_create(&(this->udp_read_thread_id), NULL, tcp_read_thread_function, (void *)this);


	return this->server_tcp_fd;
}
//监听本机某个端口，用于将本机作为服务器 udp，监听
uint32 p2p_base::create_udp_server(uint32 port)	{
	if ((this->server_udp_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	memset(&(this->udp_server_addr), 0, sizeof(struct sockaddr_in));
	this->udp_server_addr.sin_family = AF_INET;
	this->udp_server_addr.sin_port = htons(port);
	this->udp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->server_udp_fd, (struct sockaddr *)&this->udp_server_addr, sizeof(struct sockaddr_in)) < 0) {
		printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	this->read_thread_socket_id = this->server_udp_fd;
	pthread_create(&(this->udp_read_thread_id), NULL, udp_read_thread_function, (void *)this);

	return this->server_udp_fd;
}


uint32 p2p_base::set_from_id(uint32 from_id) {
	this->from_id = from_id;
	return 1;
}

uint32 p2p_base::send_p2p_tcp(uint32 client_id, uint32 data_sze, uint32 alias, uint8 task) {
	this->send_task_unique++;
	struct  wait_send *wait_send = (struct  wait_send*)memory_mgt::getInstance()->new_memory(sizeof(struct  wait_send), this->send_task_unique);
	if (wait_send == NULL) {
		return -1;
	}
	wait_send->status 	= 0;
	wait_send->version 	= _VERSION_;
	wait_send->type 	= P2P_TCP;
	wait_send->send_id 	= client_id;
	wait_send->from_id 	= this->from_id;
	wait_send->task 	= task;
	wait_send->data_size = data_sze;
	wait_send->unique = this->send_task_unique;
	wait_send->data_ptr = (uint8*)memory_mgt::getInstance()->find_memory(alias);

	pthread_mutex_lock(&(this->send_mutex));
	this->wait_send_list.push_back(wait_send);
	pthread_mutex_unlock(&(this->send_mutex));

	return 1;
}
uint32 p2p_base::send_p2p_udp(uint32 client_id, uint32 data_sze, uint32 alias, uint8 task) {
	this->send_task_unique++;
	struct  wait_send *wait_send = (struct  wait_send*)memory_mgt::getInstance()->new_memory(sizeof(struct  wait_send), this->send_task_unique);
	if (wait_send == NULL) {
		return -1;
	}

	wait_send->status 	= 0;
	wait_send->version 	= _VERSION_;
	wait_send->type 	= P2P_UDP;
	wait_send->send_id 	= client_id;
	wait_send->from_id 	= this->from_id;
	wait_send->task 	= task;
	wait_send->data_size = data_sze;
	wait_send->unique = this->send_task_unique;
	wait_send->data_ptr = (uint8*)memory_mgt::getInstance()->find_memory(alias);

	pthread_mutex_lock(&(this->send_mutex));
	this->wait_send_list.push_back(wait_send);
	pthread_mutex_unlock(&(this->send_mutex));

	return 1;
}

uint32 p2p_base::send_psp_tcp(uint32 client_id, uint32 data_sze, uint32 alias, uint8 task) {
	this->send_task_unique++;
	struct  wait_send *wait_send = (struct  wait_send*)memory_mgt::getInstance()->new_memory(sizeof(struct  wait_send), this->send_task_unique);
	if (wait_send == NULL) {
		return -1;
	}
	wait_send->status 	= 0;
	wait_send->version 	= _VERSION_;
	wait_send->type 	= PSP_TCP;
	wait_send->send_id 	= client_id;
	wait_send->from_id 	= this->from_id;
	wait_send->task 	= task;
	wait_send->data_size = data_sze;
	wait_send->unique = this->send_task_unique;
	wait_send->data_ptr = (uint8*)memory_mgt::getInstance()->find_memory(alias);

	pthread_mutex_lock(&(this->send_mutex));
	this->wait_send_list.push_back(wait_send);
	pthread_mutex_unlock(&(this->send_mutex));

	return 1;
}

uint32 p2p_base::send_psp_udp(uint32 client_id, uint32 data_sze, uint32 alias, uint8 task) {
	this->send_task_unique++;
	struct  wait_send *wait_send = (struct  wait_send*)memory_mgt::getInstance()->new_memory(sizeof(struct  wait_send), this->send_task_unique);
	if (wait_send == NULL) {
		return -1;
	}
	wait_send->status 	= 0;
	wait_send->version 	= _VERSION_;
	wait_send->type 	= PSP_UDP;
	wait_send->send_id 	= client_id;
	wait_send->from_id 	= this->from_id;
	wait_send->task 	= task;
	wait_send->data_size = data_sze;
	wait_send->unique = this->send_task_unique;
	wait_send->data_ptr = (uint8*)memory_mgt::getInstance()->find_memory(alias);
	pthread_mutex_lock(&(this->send_mutex));
	this->wait_send_list.push_back(wait_send);
	pthread_mutex_unlock(&(this->send_mutex));
	return 1;
}


/*
* tcp发送给服务器
* data:数据指针
* data_size: 数据大小
* task:任务编码
*/
uint32 p2p_base::send_server_tcp(uint32 alias, uint8 task) {


	return 1;
}
/**
* @param tcp发送给服务器
* @param data:数据指针
* @param data_size: 数据大小
* @param task:任务编码
*/
uint32 p2p_base::send_server_udp(uint32 alias, uint8 task) {

	return 1;
}
































