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
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <list>
#include "types.h"
#include "memory_mgt.h"
using namespace std;

class socekt_server {

  public:
	uint32 tcp_socket_fd;
	uint32 udp_soccket_fd;
  public:
	socekt_server(string ip, port);
	~socekt_server();
	virtual void recved_data(struct recve_data* recve_ptr, struct sockaddr_in addr) {};	//接受到的数据包
	uint32 create_tcp();
	uint32 create_udp(uint8 type = 0);	//0:立即回调recved_data,1:接受完整数据回调recved_data

};
#endif