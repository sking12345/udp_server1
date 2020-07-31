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

class socket_base {
  public:
  	pthread_t send_thread_fd;	//写线程文件描述符
  	pthread_mutex_t send_mutex; //写互斥信号量
	pthread_cond_t send_cond;	//写条件变量
	uint8 send_thread_status;	//写线程状态

	pthread_t read_thread_fd;	//读线程描述符
	pthread_mutex_t read_mutex; //写互斥信号量
	pthread_cond_t read_cond;	//写条件变量
	uint8 read_thread_status;	//读线程状态

  public:
	socket_base();
	~socket_base();
	uint32 tcp_send(uint32 socket_fd, uint8 *data,uint32 size);	//tcp 发送数据
	uint32 udp_send(uint32 socekt_fd, uint8 *data, uint32 size,struct sockaddr_in addr); //udp 发送数据
	uint32 create_read_thread(void *(*call_function)(void *),void *arg);	//创建读线程
	uint32 create_send_thread(void *(*call_function)(void *),void *arg);	//创建写线程
	uint32 close_read_thread();	//关闭读线程
	uint32 close_send_thread();	//关闭写线程

};
#endif