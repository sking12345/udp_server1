#include "socket_base.h"


socket_base::socket_base()
{

}
socket_base::~socket_base()
{

}

//tcp 发送数据

uint32 socket_base::tcp_send(uint32 socket_fd, uint8 *data,uint32 size)
{
	return send(socket_fd,data,size,0);
}	
//udp 发送数据
uint32 socket_base::udp_send(uint32 socekt_fd, uint8 *data,uint32 size, struct sockaddr_in addr)
{
	
	return sendto(socekt_fd,data,size,0,(struct sockaddr *)&addr,sizeof(struct sockaddr_in));
}

//创建读线程
uint32 socket_base::create_read_thread(void *(*call_function)(void *),void *arg){

	pthread_cond_init(&(this->read_cond), NULL);
	pthread_mutex_init(&(this->read_mutex), NULL);

	if ((pthread_create(&this->read_thread_fd, NULL, call_function, (void*)arg)) == -1)
    {
       	printf("create read thread error!\n");
      
     	return -1;
 	}
 	return 1;
}	
//创建写线程
uint32 socket_base::create_send_thread(void *(*call_function)(void *),void *arg){

	pthread_cond_init(&(this->send_cond), NULL);
	pthread_mutex_init(&(this->send_mutex), NULL);
	if ((pthread_create(&this->read_thread_fd, NULL, call_function, (void*)arg)) == -1)
	{
       	printf("create send thread error!\n");
       
     	return -1;
 	}
 	return 1;
}

//关闭读线程
uint32 socket_base::close_read_thread(){
	this->read_thread_status = 0;
	pthread_cond_signal(&this->read_cond);
	return 1;
}

//关闭写线程
uint32 socket_base::close_send_thread(){
	this->send_thread_status = 0;
	pthread_cond_signal(&this->send_cond);
	return 1;
}	























