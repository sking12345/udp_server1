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

class p2p_base {
  public:
	string 	public_ip;		//本机外网ip,
	uint8 	public_port;	//本机外网port
	uint32 	from_id;
	uint32 	server_udp_fd; //链接服务器socket_fd
	uint32 	server_tcp_fd; //链接服务器socket_fd
	uint32 	udp_connect_fd; //链接服务器socket_fd
	uint32 	tcp_connect_fd; //链接服务器socket_fd

	int read_thread_socket_id;

	struct 	sockaddr_in tcp_connect_addr; //链接指定服务器的网络信息
	struct 	sockaddr_in udp_connect_addr; //链接指定服务器的网络信息
	struct 	sockaddr_in tcp_server_addr; //本机作为服务器的网络信息
	struct 	sockaddr_in udp_server_addr; //本机作为服务器的网络信息

	map< uint32, struct listeners_info > client_map;	//存放本机作为服务器监听时,链接到本机的socket信息

	list< struct  wait_send* > wait_send_list;
	// list< struct  wait_send* >::iterator wait_send_list_iter;	//发送数据的当前位置

	list< struct  wait_send* > wait_sended_list;	//已发送完数据的队列；
	list< struct recve_data* > recve_data_list;


	pthread_t tcp_read_thread_id;	//读数据线程
	pthread_t tcp_send_thread_id;	//写数据线程

	pthread_t udp_read_thread_id;	//读数据线程
	pthread_t udp_send_thread_id;	//写数据线程


	pthread_mutex_t send_mutex; //互斥信号量
	pthread_cond_t send_cond;	//条件变量
	uint32 wait_send_num;		//等待发送的数据量
	uint8 close_send_thread;	//是否关闭写数据线程

	uint16 send_task_unique;	//系统任务编号


  public:
	p2p_base();
	virtual ~p2p_base();
	void recv_tcp_read();	//接受到tcp 数据
	void recv_udp_read();	//接受到udp 数据

	void creart_send_thread();	//创建写线程
	void create_read_thread();	//创建读线程


	uint32 conect_server_tcp(string server_ip, uint32 port);	//tcp链接服务器
	uint32 conect_server_udp(string server_ip, uint32 port);	//dup链接服务器

	uint32 create_tcp_server(uint32 port);	//监听某个端口，用于将本机作为服务器，tcp，监听
	uint32 create_udp_server(uint32 port);	//监听某个端口，用于将本机作为服务器 udp，监听
	uint32 set_from_id(uint32 from_id);

	uint32 send_p2p_tcp(uint32 client_key, uint32 data_sze, uint32 alias, uint8 task);
	uint32 send_p2p_udp(uint32 client_key, uint32 data_sze, uint32 alias, uint8 task);
	uint32 send_psp_tcp(uint32 client_key, uint32 data_sze, uint32 alias, uint8 task);
	uint32 send_psp_udp(uint32 client_key, uint32 data_sze, uint32 alias, uint8 task);

	uint32 send_p2p_tcp(uint32 client_key, uint32 data_sze, string alias, uint8 task);
	uint32 send_p2p_udp(uint32 client_key, uint32 data_sze, string alias, uint8 task);
	uint32 send_psp_tcp(uint32 client_key, uint32 data_sze, string alias, uint8 task);
	uint32 send_psp_udp(uint32 client_key, uint32 data_sze, string alias, uint8 task);

	uint32 send_server_tcp(uint32 alias, uint8 task);
	uint32 send_server_udp(uint32 alias, uint8 task);
	uint32 send_server_tcp(string alias, uint8 task);
	uint32 send_server_udp(string alias, uint8 task);

};

#endif





