#ifndef _TYEPS_H_
#define _TYEPS_H_


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



typedef signed char             int8;
typedef short int               int16;
typedef int                     int32;
typedef long int       			int64;

typedef unsigned char           uint8;
typedef unsigned short int      uint16;
typedef unsigned int            uint32;
typedef unsigned long int       uint64;



#define _UDP_PACK_SIZE_ 1470
#define _UDP_DATA_SIZE_ 1440

#define _OVER_SEND_TIME_ 500  //发送超时，数据包保留时长
#define _SAVE_RECV_LOG_TIME_ 500 //ms


#define _TIMER_CLEAER_RECV_LOG_  10		//5秒清除一次结束数据的日志记录
#define _TIMER_VERIFY_SEND_PACK_ 10		//5秒验证一次发送数据包的记录,清除发送失败的内存,并做回调
// #define _TIMER_HEARTBEAT_ 10  //10 发送一个心跳包给服务器,服务器记录外网地址信息


#define _VERSION_ 0x01
#define _MAX_TASK_LIST_ 500	//最大发送任务队列
#define _THREAD_EXT_ 0x00
#define _THREAD_RUN_ 0x01

#define _UDP_TYPE_PSP_ 	0x00	//psp sendid //server
#define _UDP_TYPE_P2P_ 	0x01	//p2p sendid
#define _UDP_TYPE_PGP_ 	0x02	//发送给sendId 组		//server
#define _UDP_TYPE_ADDR_ 0x03	//数据转发,并返回某个用户的外网地址


#define _TASK_LOGIN_ 1 //登录任务
#define _TASK_LOGIN_SUCCESS_ 2
#define _TASK_LOGIN_FAIL_ 3 //登录失败
#define _TASK_QUITE_ 4 //退出登录
#define _TASK_BACK_QUITE_ 5 //退出登录
#define _TASK_GET_ADDR_ 6 //获取用户addr
#define _TASK_BACK_ADDR_ 7 //返回用户addr
#define _TASK_GET_PACK_ 8	//获取数据包,用于返回缺少那些包
#define _TASK_EXEIT_PACK_ 9  //包不存在
#define _TASK_END_ 11 //结束数据包
#define _TASK_USER_EXITE_ 12  //用户未登录
#define _TASK_EXIT_READ_THEAD_ 13 //结束线程
#define _TASK_HEARTBEAT_ 14  //心跳包

#define _TASK_TEXT_ 10	//文本

typedef struct udp_pack {
	uint8 version;		//版本号
	uint8 type;
	uint8 task;		//任务编号
	uint32 send_id;		//发送给谁的id
	uint32 from_id;		//来自谁的id
	uint16 max_size;	//数据大小
	uint16 sequence;	//包序
	uint16 unique;
	uint64 time;
	uint8 data[_UDP_DATA_SIZE_];
} UDP_PACK, *PUDP_PACK;


typedef struct extranet_addr {
	uint8 type;		// 通信类型: p2p,psp
	uint32 port;	// 通信端口 //当客户端确认p2p 时，将绑定的端口发送给服务器保存
	int8 IPdotdec[20];	//通信的ip,根据客户端的数据包 解析客户端的外网ip 地址
	uint64 last_time;	//上次登录日期
	struct sockaddr_in addr;	//服务端保存客户端连接服务端的外网地址信息
} EXTRANET_ADDR, *PEXTRANET_ADDR;





#endif










