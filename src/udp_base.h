#ifndef _UDP_BASE_
#define _UDP_BASE_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>      //添加头文件
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>

#include <list>
#include <map>
#include "types.h"
#include "exception.h"


class UdpBase {


  public:
	uint32 p2p_socket_fd;	//p2p 服务器的fd
	uint32 psp_socket_fd;	//连接服务端的fd
	uint32 user_id;			//用户id,服务器位0x00
	uint32 task_queue;		//任务队列	//
	uint32 wait_send_queue;
	uint32 current_num;			//用于定时任务操作	,1s 加一次.定时计算器
	int8 extranet_ip[20];	//存放外网的ip
	int8 server_ip[20];		//存放服务器的ip
	uint32 extranet_port;		//存入外网的端口
	uint32 server_port;			//存放服务器端口

	uint8 read_thread_status;	//读线程状态
	uint8 send_thread_status;	//写线程状态
	uint8 verify_recv_log;		//验证接受日志
	uint32 heartbeat_timer;
	pthread_t read_thread_id;	//读数据线程
	pthread_t send_thread_id;	//写数据线程

	pthread_mutex_t send_mutex; //互斥信号量
	pthread_cond_t send_cond;	//条件变量

	pthread_mutex_t read_mutex; //互斥信号量
	pthread_mutex_t recv_log_mutex; //互斥信号量	//接受日志锁
	pthread_mutex_t socket_addr_mutex; //互斥信号量	//用户外网地址信息操作


	struct sockaddr_in psp_server_addr;	//服务器的addr
	struct sockaddr_in p2p_server_addr;	//本机服务器的

	std::list<struct udp_pack*> send_list;	//线程发送数据队列
	std::map<uint64, std::map<uint16, struct udp_pack*> > send_map_list; // 发送数据包记录,用于补发数据包

	std::map<uint32, std::map<uint64, uint8*> > recv_map_list;	//保存接受队列
	//uint32：用户id,uint64:任务唯一编码，uint16：分包序,uint64:接受时间
	std::map<uint32, std::map<uint64, std::map<uint16, uint64>  > > recv_map_log; //用于包去重验证
	std::map<uint32, struct extranet_addr*> extranet_addr_map;	//存放各个用户的外网地址信息





  public:
	UdpBase();
	virtual ~UdpBase();
	virtual void recved_pack(struct udp_pack, struct sockaddr_in addr) {};	//接受到的数据包
	virtual void send_timeout_call(uint16 unique);	//发送数据包的数据回调
	uint32 get_pack_num(uint32 data_size);	//数据大小可以拆分成多少个数据包
	uint64 get_mstime();	//获取系统当前毫秒

	uint32 send_data(void*, int size, uint32 userid, uint16 task, uint8 type); //拆包发送数据
	uint32 psp_send(struct udp_pack*, struct sockaddr_in addr); //psp 发送数据包
	uint32 p2p_send(struct udp_pack*, uint32 uid);					//p2p 发送数据包给uid,如果uid 没有采用p2p 则psp 发送转发

	uint32 p2p_send(struct udp_pack*, struct sockaddr_in addr);		//p2p 发送数据包给uid,如果uid 没有采用p2p 则psp 发送转发

	uint32 send_server_command(uint8 task, uint32 uid = 0x00);	//向服务器发送指令
	uint32 send_client_command(uint8 task, uint32 uid);	//向p2p 另一段发送指令

	struct extranet_addr* save_addr(uint32 uid, struct sockaddr_in addr, uint8 type = _UDP_TYPE_PSP_);	//保存外网信息
	uint32 remove_addr(uint32 uid);	//移除用户外网信息
	struct extranet_addr* get_already_addrs(uint32); //获取外网信息
	uint32 get_socket_addrs(uint32); //向服务器获取外网信息
	uint32 clear_recved_log(uint32 userid, uint32 unique);	//清除日志

	struct udp_pack* get_missing_pack(struct udp_pack);	//获取确实的包,用于补包
	void set_verify_recv_log(int type);	//设置是否需要验证接受数据的记录

	void create_read_thread();	//传教读线程
	void create_send_thread();	//创建写线程
	void close_read_thread();
	void recv_start();
	void free_send_map(uint64 unique);	//结束释放某个任务的数据 :time+task_queue
	void free_recved_data(uint32 userid, uint64 unique);	//释放接受到的数据内存
	/*定时任务*/
	void timer_clear_recv_log();	//清除日志
	void timer_verify_send_pack();		//验证那些数据没有发送成功
	void timer_heartbeat();	//向服务器心跳包
	void set_timer_heartbeat(uint32 heartbeat_timer);	//向服务器心跳包
	void execute_timer();	//执行定时任务，1s 执行一次



};


#endif


























