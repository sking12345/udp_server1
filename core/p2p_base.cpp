#include "p2p_base.h"

p2p_base::p2p_base(){
	this->from_id = 0;
	this->server_udp_fd = 0;
	this->server_tcp_fd = 0;
	this->udp_connect_fd = 0;
	this->tcp_connect_fd = 0;
}
p2p_base::~p2p_base(){

}

//链接服务器
uint32 p2p_base::conect_server_tcp(string server_ip,uint32 port){	

	if( (this->tcp_connect_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        return 0;
    }

    memset(&this->tcp_connect_addr, 0, sizeof(this->tcp_connect_addr));
    this->tcp_connect_addr.sin_family = AF_INET;
    this->tcp_connect_addr.sin_port = htons(port);
    if( inet_pton(AF_INET, server_ip.c_str(), &this->tcp_connect_addr.sin_addr) <= 0){
        printf("inet_pton error for %s\n",server_ip.c_str());
        return 0;
    }
    if( connect(this->tcp_connect_fd, (struct sockaddr*)&this->tcp_connect_addr, sizeof(this->tcp_connect_addr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

	return 1;
}
//链接服务器
uint32 p2p_base::conect_server_udp(string server_ip,uint32 port){	

	this->udp_connect_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	memset(&this->udp_connect_addr, 0, sizeof( struct sockaddr_in));
	this->udp_connect_addr.sin_family = AF_INET;
	this->udp_connect_addr.sin_port = htons(port);
	this->udp_connect_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
	return 1;

}
//监听本机某个端口，用于将本机作为服务器，tcp，监听
uint32 p2p_base::listener_tcp(uint32 port){	

    if( (this->server_tcp_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

    memset(&this->tcp_server_addr, 0, sizeof(this->tcp_server_addr));
    this->tcp_server_addr.sin_family = AF_INET;
    this->tcp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    this->tcp_server_addr.sin_port = htons(port);

    if( bind(this->server_tcp_fd, (struct sockaddr*)&this->tcp_server_addr, sizeof(this->tcp_server_addr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }
    if( listen(this->server_tcp_fd, 10) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }
	return 1;
}
//监听本机某个端口，用于将本机作为服务器 udp，监听
uint32 p2p_base::listener_udp(uint32 port)	{
	if ((this->server_udp_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		 printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
		 return 0;
	}
	memset(&(this->udp_server_addr), 0, sizeof(struct sockaddr_in));
	this->udp_server_addr.sin_family = AF_INET;
	this->udp_server_addr.sin_port = htons(port);
	this->udp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->server_udp_fd, (struct sockaddr *)&this->udp_server_addr, sizeof(struct sockaddr_in)) < 0) {
		 printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
		return 0;
	}
	return 1;
}


uint32 p2p_base::set_from_id(uint32 from_id){
	this->from_id = from_id;
	return 1;
}



uint32 p2p_base::send_p2p_tcp(uint32 client_id,uint32 alias,uint8 task){

	memory_mgt *mgt = memory_mgt::getInstance();
	struct memory_struct *m_struct = mgt->find_memory(alias);
	struct  wait_send  wait_send = {0};
	wait_send.status 	= 0;
	wait_send.version 	= _VERSION_;
	wait_send.type 		= P2P_TCP;
	wait_send.send_id 	= client_id;
	wait_send.from_id 	= this->from_id;
	wait_send.task 		= task;
	wait_send.data_size = m_struct->data_size;
	wait_send.data_ptr = m_struct->data;
	
	this->wait_sended_list.insert();

	return 1;
}
uint32 p2p_base::send_p2p_udp(uint32 client_id,uint32 alias,uint8 task){

	return 1;
}

uint32 p2p_base::send_psp_tcp(uint32 client_id,uint32 alias,uint8 task){

	return 1;
}

uint32 p2p_base::send_psp_udp(uint32 client_id,uint32 alias,uint8 task){

	return 1;
}




/*
* tcp发送给服务器
* data:数据指针
* data_size: 数据大小
* task:任务编码
*/
uint32 p2p_base::send_server_tcp(uint32 alias,uint8 task){


	return 1;
}
/**
* @param tcp发送给服务器
* @param data:数据指针
* @param data_size: 数据大小
* @param task:任务编码
*/
uint32 p2p_base::send_server_udp(uint32 alias,uint8 task){

	return 1;
}
































