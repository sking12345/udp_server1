
#ifndef _TYEPS_H_
#define _TYEPS_H_


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>



typedef signed char             int8;
typedef short int               int16;
typedef int                     int32;
typedef long int       			int64;

typedef unsigned char           uint8;
typedef unsigned short int      uint16;
typedef unsigned int            uint32;
typedef unsigned long int       uint64;


#define DATA_DIVIDE_SIZE  500	//数据分界线,小于该值的优友先发送
#define WAIT_REPLY_TIME	 500	//等待一个数据包回复的最大时长,如果超出则回调函数

#define DATA_TYPE_P2P 0x01		//p2p 发送
#define DATA_TYPE_PSP 0x02		//服务器转发
#define DATA_TYPE_PS 0x03		//支发送给服务器

#define TCP_SERVER 0x01
#define UDP_SERVER 0x02

/*
* 用于发送数据的线程
 */
#define P2P_TCP 0x01
#define P2P_UDP 0x02
#define PSP_TCP 0x03
#define PSP_UDP 0x04

#define _VERSION_ 0x01		//版本
#define PACK_DATA_SIZE (1470 - 32*4)

typedef struct pack {	//分包时的数据结构
	uint8 	version;		//版本号
	uint8 	type;		// tcp,udp
	uint32 	send_id;		//发送给谁的id，//-1:不转发
	uint32 	from_id;		//来自谁的id
	uint8  	task;		//任务编号,接受端根据任务编号判断该数据包应该怎么处理
	uint16 	unique;		//系统序列
	uint16 	sequence;
	uint16 	data_size;	//数据大小
	uint8 	data[PACK_DATA_SIZE];
} PACK, *PPACK;

typedef struct  wait_send {	//待发送队列
	uint8 	status;			//用于线程发送数据判断, 0:继续发送,1:查看接受对方回复信息的时间,如果大于WAIT_REPLY_TIME 则回调某个方法,
	uint8 	version;		//版本号
	uint8 	type;		//通信类型(Communication type): p2p,psp,告诉服务器是否需要转发
	uint32 	send_id;		//发送给谁的id，//-1:不转发
	uint32 	from_id;		//来自谁的id
	uint8  	task;		//任务编号
	uint16 	unique;		//系统序列
	uint32 	send_num;	//已发数据次数，如果对方接受到一次数据包，则+1
	uint32 	data_size;	//数据大小
	uint8 	*data_ptr;	//数据
} WAIT_SEND_LIST, *PWAIT_SEND_LIST;

typedef struct recve_data { //接收数据包	，
	uint8 type;	//tcp: udp
	uint8 status;		// 0x00: 数据不完整,0x01数据完整
	uint32 	from_id;
	uint8  	task;		//任务编号
	// uint32 	receive_num; //接受包数
	uint32 	data_size;	//数据大小
	uint8 	data[0];	//数据
} recve_list;


typedef struct listeners_info {
	uint8 	type;			// 通信类型(Communication type): p2p,psp
	uint8 	ip_dotdec[20];	//本机作为服务器，接受到的数据
	uint32 	port;
	uint64 	last_time; //上次登录日期
	struct 	sockaddr_in addr;

} LISTEERS_INFO, *PLISTEERS_INNFO;


#endif
















