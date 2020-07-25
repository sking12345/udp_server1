#ifndef _P2P_BASE_H_
#define _P2P_BASE_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>      //添加头文件
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <list>
#include "types.h"
#include "memory_mgt.h"
using namespace std;

class p2p_base
{
public:
	string 	public_ip;		//本机外网ip,
	uint8 	public_port;	//本机外网port
	uint32 	from_id; 
	uint32 	server_udp_fd; //链接服务器socket_fd
	uint32 	server_tcp_fd; //链接服务器socket_fd
	uint32 	udp_connect_fd; //链接服务器socket_fd
	uint32 	tcp_connect_fd; //链接服务器socket_fd
	struct 	sockaddr_in tcp_connect_addr; //链接指定服务器的网络信息
	struct 	sockaddr_in udp_connect_addr; //链接指定服务器的网络信息
	struct 	sockaddr_in tcp_server_addr; //本机作为服务器的网络信息
	struct 	sockaddr_in udp_server_addr; //本机作为服务器的网络信息
	
	map< uint32,struct listeners_info > client_map;	//存放本机作为服务器监听时,链接到本机的socket信息	
	list< struct  wait_send* > wait_send_list;
	list< struct  wait_send* >::iterator wait_send_list_iter;	//发送数据的当前位置

	list< struct  wait_send* > wait_sended_list;	//已发送完数据的队列；
	list< struct recve_data* > recve_data_list;


public:
	p2p_base();
	virtual ~p2p_base();
	uint32 conect_server_tcp(string server_ip,uint32 port);	//tcp链接服务器
	uint32 conect_server_udp(string server_ip,uint32 port);	//dup链接服务器

	uint32 listener_tcp(uint32 port);	//监听某个端口，用于将本机作为服务器，tcp，监听
	uint32 listener_udp(uint32 port);	//监听某个端口，用于将本机作为服务器 udp，监听
	uint32 set_from_id(uint32 from_id);

	uint32 send_p2p_tcp(uint32 client_key,uint32 alias,uint8 task);
	uint32 send_p2p_udp(uint32 client_key,uint32 alias,uint8 task);
	uint32 send_psp_tcp(uint32 client_key,uint32 alias,uint8 task);
	uint32 send_psp_udp(uint32 client_key,uint32 alias,uint8 task);

	uint32 send_p2p_tcp(uint32 client_key,string alias,uint8 task);
	uint32 send_p2p_udp(uint32 client_key,string alias,uint8 task);
	uint32 send_psp_tcp(uint32 client_key,string alias,uint8 task);
	uint32 send_psp_udp(uint32 client_key,string alias,uint8 task);

	uint32 send_server_tcp(uint32 alias,uint8 task);	
	uint32 send_server_udp(uint32 alias,uint8 task);
	uint32 send_server_tcp(string alias,uint8 task);	
	uint32 send_server_udp(string alias,uint8 task);

};

#endif





