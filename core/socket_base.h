#ifndef _SOCKET_BASE_H_
#define _SOCKET_BASE_H_

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

class socekt_base {
  public:

  public:
	socekt_base();
	~socekt_base();
	uint32 tcp_send(uint32 socket_fd, uint8 *data);	//tcp 发送数据
	uint32 udp_send(uint32 socekt_fd, uint8 *data, struct sockaddr_in addr); //udp 发送数据


};
#endif