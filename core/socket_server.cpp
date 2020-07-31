#include "socket_server.h"



void *read_udp_thread(void * arg)
{
	socket_server *socket_obj = (socket_server*)arg;
	int udp_soccket_fd = socket_obj->udp_soccket_fd;
	int udp_size = sizeof(struct udp_pack);
	socklen_t socket_size = sizeof(struct sockaddr_in);
	while(1)
	{
		struct udp_pack udp_pack = {0};
		struct sockaddr_in clienn_address = {0};
		
		int recv_size = recvfrom(udp_soccket_fd,&udp_pack,udp_size,0, (struct sockaddr *)&clienn_address,&socket_size);
		if(recv_size<=0)
		{
			socket_obj->udp_read_abnormal(&clienn_address);	//udp 读取到数据异常处理
			return NULL;
		}
		socket_obj->udp_readed_data(&udp_pack,&clienn_address);
	}
	return NULL;
}


void *read_tcp_thread(void *arg)
{
	socket_server *socket_obj = (socket_server*)arg;
	int udp_soccket_fd = socket_obj->tcp_socket_fd;

	struct sockaddr_in client_addr;
    socklen_t peerlen = sizeof(struct sockaddr_in);
	
	#ifdef _WIN32
		
	#elif __APPLE__
		
	    // android
	#elif __linux__
		socket_obj->epollfd = epoll_create1(EPOLL_CLOEXEC); // 用于设置改描述符的close-on-exec(FD_CLOEXEC)标志
	    socket_obj->event.data.fd = udp_soccket_fd;
	    socket_obj->event.events = EPOLLIN | EPOLLET;
	    epoll_ctl(socket_obj->epollfd, EPOLL_CTL_ADD, udp_soccket_fd, &event);
	    while(1){
	    	int nready = epoll_wait(socket_obj->epollfd,socket_obj->events,socket_obj->tcp_max_size, -1);
	    	if(nready == -1) {
	            if(errno == EINTR) {
	                continue;
	            }
        	}
        	if((size_t)nready == socket_obj->events.size()) {
        		socket_obj->tcp_max_size = socket_obj->events.size() * 2;	
            	events.resize(socket_obj->events.size() * 2);	//如果收到的tcp事件数量 >= 计划的数量,加大监听数量
        	}
        	for(i = 0; i < nready; ++i) {
        		 if(socket_obj->events[i].data.fd == socket_obj->udp_soccket_fd) {	//新连接
        		 	socket_obj->tcp_new_fd(socket_obj->events[i].data.fd);
        		 }else if(socket_obj->events[i].events & EPOLLIN) {	//可读
        		 	socket_obj->tcp_new_data(socket_obj->events[i].data.fd);
        		 	
        		 }else{

        		 }
        	}
	    }
	#else
	#error "Unknown compiler"
	#endif
	return NULL;
}

socket_server::socket_server()
{

}
socket_server::~socket_server()
{

}


//接受到的数据包
void socket_server::recved_data(struct recve_data* recve_ptr, struct sockaddr_in addr)
{

}	
uint32 socket_server::create_tcp(uint32 port,uint32 fd_size)
{	
	this->tcp_max_size = fd_size;	//tcp 链接的最大数
	if ( (this->tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}

	#ifdef _WIN32	//windows 采用

	#elif __APPLE__	//苹果系统采用 kequeue 模式
		   
	#elif __linux__	//linux 采用epoll 模式
		this->epfd=epoll_create(fd_size);
		this->event.data.fd=this->tcp_socket_fd;
  		this->event.events=EPOLLIN|EPOLLET;
		epoll_ctl(this->epfd,EPOLL_CTL_ADD,this->tcp_socket_fd,&this->event);
	#endif

	memset(&this->tcp_server_addr, 0, sizeof(this->tcp_server_addr));
	this->tcp_server_addr.sin_family = AF_INET;
	this->tcp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	this->tcp_server_addr.sin_port = htons(port);
	int reuse = 1;
	//使其可以重复绑定wait 下的端口
    if(setsockopt(this->tcp_socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("setsockopt fail!");
        return 0;
    }
	if ( bind(this->tcp_socket_fd, (struct sockaddr*)&this->tcp_server_addr, sizeof(this->tcp_server_addr)) == -1) {
		printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	if ( listen(this->tcp_socket_fd, 10) == -1) {
		printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	this->create_read_thread(read_tcp_thread,(void*)this);	//创建读线程
	return this->tcp_socket_fd;
}

uint32 socket_server::create_udp(uint32 port)
{
	if ((this->udp_soccket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	memset(&(this->udp_server_addr), 0, sizeof(struct sockaddr_in));
	this->udp_server_addr.sin_family = AF_INET;
	this->udp_server_addr.sin_port = htons(port);
	this->udp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->udp_soccket_fd, (struct sockaddr *)&this->udp_server_addr, sizeof(struct sockaddr_in)) < 0) {
		printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
		return 0;
	}
	this->create_read_thread(read_udp_thread,(void*)this); //创建读线程
	return this->udp_soccket_fd;
}

//tcp 读取数据异常
uint32 socket_server::tcp_read_abnormal(uint32 client_fd){

	return 1;
}	
//udp 读取数据异常
uint32 socket_server::udp_read_abnormal(struct sockaddr_in *client_addr)
{
	printf("%s\n", "udp read abnormal");
	return 1;
}
//udp 读取到的数据,根据实际业务继承重写
uint32 socket_server::udp_readed_data(struct udp_pack*pack,struct sockaddr_in *addr)
{
	printf("%s\n", "udp_readed_data");
	return 1;
}
//tcp 读取到的数据,根据实际业务继承重写
uint32 socket_server::tcp_readed_data(struct tcp_pack*pack,uint32 socket_fd)
{
	printf("%s\n", "tcp_readed_data");
	return 1;
}
//新的tcp 链接
uint32 socket_server::tcp_new_fd(int32 new_fd)	
{
	printf("%s,%d\n", "tcp_new_fd",new_fd);
	return 1;
}
//tcp 连接的新数据
uint32 socket_server::tcp_new_data(int32 cfd,struct tcp_pack *pack)	
{
	printf("%s,%d\n", "tcp_new_data",cfd);
	return 1;
}
//tcp 连接断开
uint32 socket_server::tcp_close_fd(uint32 cfd) 
{
	printf("%s:%d\n", "tcp_close_fd",cfd);
	return 1;
}












