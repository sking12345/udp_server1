#ifndef _SOCKET_SERVER_H_
#define _SOCKET_SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>      //添加头文件
#include <signal.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <list>

#ifdef _WIN32

#elif __APPLE__
	   
#elif __linux__
	#include <sys/epoll.h>
#endif

#include "types.h"
#include "memory_mgt.h"
#include "socket_base.h"
using namespace std;

class socket_server : public socket_base {

  public:
	uint32 tcp_socket_fd;
	uint32 udp_soccket_fd;
	uint32 tcp_max_size;
	#ifdef __linux__
	struct epoll_event event,events[20];
	int32 epollfd = -1;
	#endif

	struct 	sockaddr_in tcp_server_addr; //本机作为服务器的网络信息
	struct 	sockaddr_in udp_server_addr; //本机作为服务器的网络信息

  public:
	socket_server();
	~socket_server();
	virtual void recved_data(struct recve_data* recve_ptr, struct sockaddr_in addr);	//接受到的数据包
	uint32 create_tcp(uint32 port,uint32 fd_size);
	uint32 create_udp(uint32 port);
	uint32 tcp_read_abnormal(uint32 client_fd);	//tcp 读取数据异常
	uint32 udp_read_abnormal(struct sockaddr_in *client_addr);	//udp 读取数据异常
	uint32 udp_readed_data(struct udp_pack*pack,struct sockaddr_in *addr);	//udp 读取到的数据
	uint32 tcp_readed_data(struct tcp_pack*pack,uint32 socket_fd);	//tcp 读取到的数据

	uint32 tcp_new_fd(int32 new_fd);	//新的tcp 链接
	uint32 tcp_new_data(int32 cfd,struct tcp_pack *pack);	//tcp 连接的新数据
	uint32 tcp_close_fd(uint32 cfd); //tcp 连接断开
	
};
#endif









