#include "udp_base.h"


void* send_thread_function(void* arg) {
	UdpBase *udp_base = (UdpBase*)arg;
	while (udp_base->send_thread_status == _THREAD_RUN_) {
		pthread_mutex_lock(&(udp_base->send_mutex));
		if (udp_base->wait_send_queue <= 0) {
			pthread_cond_wait(&(udp_base->send_cond), &(udp_base->send_mutex));
		}
		if (udp_base->send_thread_status != _THREAD_RUN_) {
			pthread_mutex_unlock(&(udp_base->send_mutex));
			pthread_exit(0);
			return NULL;
		}
		if (udp_base->wait_send_queue <= 0) {
			pthread_mutex_unlock(&(udp_base->send_mutex));
			continue;
		}
		struct udp_pack* send_pack = udp_base->send_list.front();
		if (send_pack == NULL) {
			pthread_mutex_unlock(&(udp_base->send_mutex));
			continue;
		}
		if (send_pack->type == _UDP_TYPE_P2P_) {	//	//p2p
			udp_base->p2p_send(send_pack, send_pack->send_id);

			udp_base->send_list.erase(udp_base->send_list.begin());
			udp_base->wait_send_queue--;
		} else {
			udp_base->psp_send(send_pack, udp_base->psp_server_addr);
			udp_base->send_list.erase(udp_base->send_list.begin());
			udp_base->wait_send_queue--;
		}
		pthread_mutex_unlock(&(udp_base->send_mutex));
	}

	return NULL;
}

void* read_thread_function(void* arg) {
	UdpBase *udp_base = (UdpBase*)arg;
	udp_base->recv_start();
	pthread_exit(0);
	return NULL;
}

UdpBase::UdpBase() {

	this->user_id = 0x00;
	this->task_queue = 0;
	this->p2p_socket_fd = 0;
	this->psp_socket_fd = 0;
	this->current_num = 0;
	this->verify_recv_log = false;
	this->heartbeat_timer = 0;
	pthread_cond_init(&(this->send_cond), NULL);
	pthread_mutex_init(&(this->read_mutex), NULL);

	pthread_mutex_init(&(this->send_mutex), NULL);
	pthread_mutex_init(&(this->recv_log_mutex), NULL);
	pthread_mutex_init(&(this->socket_addr_mutex), NULL);



}





UdpBase::~UdpBase() {

	this->send_thread_status = _THREAD_EXT_;
	this->read_thread_status = _THREAD_EXT_;

	pthread_cond_signal(&(this->send_cond));
	if (this->psp_socket_fd > 0) {
		close(this->psp_socket_fd);
	}
	if (this->p2p_socket_fd > 0) {
		close(this->p2p_socket_fd);
	}
	pthread_join(this->read_thread_id, NULL);
	pthread_join(this->send_thread_id, NULL);
	pthread_cond_destroy(&(this->send_cond));

	pthread_mutex_destroy(&(this->read_mutex));
	pthread_mutex_destroy(&(this->send_mutex));
	pthread_mutex_destroy(&(this->recv_log_mutex));
	pthread_mutex_destroy(&(this->socket_addr_mutex));
	/*释放发送数据的内存*/
	std::map < uint64, std::map<uint16, struct udp_pack*> >::iterator send_map_list_iter;
	for (send_map_list_iter = this->send_map_list.begin(); send_map_list_iter != this->send_map_list.end();) {
		std::map<uint16, struct udp_pack*> unique_map_list = send_map_list_iter->second;
		std::map<uint16, struct udp_pack*> ::iterator unique_map_list_iter;
		for (unique_map_list_iter = unique_map_list.begin(); unique_map_list_iter != unique_map_list.end();) {
			struct udp_pack** pack_data = &unique_map_list_iter->second;
			if (*pack_data != NULL) {
				free(*pack_data);
				*pack_data = NULL;
			}
			unique_map_list.erase(unique_map_list_iter++);
		}
		unique_map_list.clear();
		this->send_map_list.erase(send_map_list_iter++);
	}
	this->send_map_list.clear();
	/*释放接受数据时分配的内存*/
	std::map<uint32, std::map<uint64, uint8*> >::iterator recv_map_list_iter;
	for (recv_map_list_iter = this->recv_map_list.begin(); recv_map_list_iter != this->recv_map_list.end();) {
		std::map<uint64, uint8*> recv_data_map = recv_map_list_iter->second;
		std::map<uint64, uint8*>::iterator recv_data_map_iter;
		for (recv_data_map_iter = recv_data_map.begin(); recv_data_map_iter != recv_data_map.end();) {
			uint8** recv_data = &recv_data_map_iter->second;
			if (*recv_data != NULL) {
				free(*recv_data);
				*recv_data = NULL;
			}
			recv_data_map.erase(recv_data_map_iter++);
		}
		recv_data_map.clear();
		this->recv_map_list.erase(recv_map_list_iter++);
	}
	this->recv_map_list.clear();
	/*释放p2p 分配的客户端信息*/
	std::map<uint32, struct extranet_addr*>::iterator extranet_addr_map_iter;
	for (extranet_addr_map_iter = this->extranet_addr_map.begin(); extranet_addr_map_iter != this->extranet_addr_map.end();) {
		struct extranet_addr** extranet_data = &extranet_addr_map_iter->second;
		if (*extranet_data != NULL) {
			free(*extranet_data);
			*extranet_data = NULL;
		}
		this->extranet_addr_map.erase(extranet_addr_map_iter++);
	}
	this->extranet_addr_map.clear();
}


void UdpBase::timer_clear_recv_log() { //清除日志
	Exception::save_info("execute_timer:timer_clear_recv_log");
	std::map<uint32, std::map<uint64, std::map<uint16, uint64> > >::iterator recv_map_log_iter;
	std::map<uint64, std::map<uint16, uint64> > unique_map_list;
	std::map<uint64, std::map<uint16, uint64> >::iterator unique_map_list_iter;
	std::map<uint16, uint64> sequence_list;
	std::map<uint16, uint64>::iterator sequence_list_iter;
	uint64 now_mstime = this->get_mstime();
	pthread_mutex_lock(&(this->recv_log_mutex));
	for (recv_map_log_iter = this->recv_map_log.begin(); recv_map_log_iter != this->recv_map_log.end();) {
		unique_map_list = recv_map_log_iter->second;

		for (unique_map_list_iter = unique_map_list.begin(); unique_map_list_iter != unique_map_list.end();) {
			sequence_list = unique_map_list_iter->second;

			for (sequence_list_iter = sequence_list.begin(); sequence_list_iter != sequence_list.end();) {
				if (now_mstime - sequence_list_iter->second > _SAVE_RECV_LOG_TIME_) {
					sequence_list.erase(sequence_list_iter++);
				} else {
					sequence_list_iter++;
				}
			}
			unique_map_list[unique_map_list_iter->first] = sequence_list;
			if (sequence_list.size() <= 0) {
				unique_map_list.erase(unique_map_list_iter++);
			} else {
				unique_map_list_iter++;
			}
		}

		if (unique_map_list.size() <= 0) {
			unique_map_list.clear();
			this->recv_map_log.erase(recv_map_log_iter++);
		} else {
			this->recv_map_log[recv_map_log_iter->first] =  unique_map_list;
			recv_map_log_iter++;
		}
	}
	if (this->recv_map_log.size() <= 0) {
		this->recv_map_log.clear();
	}
	pthread_mutex_unlock(&(this->recv_log_mutex));

}
void UdpBase::timer_verify_send_pack() {
	std::map<uint64, std::map<uint16, struct udp_pack*> >::iterator send_map_list_iter;
	std::map<uint16, struct udp_pack*> unique_pack_list;
	std::map<uint16, struct udp_pack*>::iterator unique_pack_list_iter;
	uint64 now_mstime = this->get_mstime();
	pthread_mutex_lock(&(this->send_mutex));
	for (send_map_list_iter = this->send_map_list.begin(); send_map_list_iter != this->send_map_list.end();) {
		unique_pack_list = send_map_list_iter->second;
		for (unique_pack_list_iter = unique_pack_list.begin(); unique_pack_list_iter != unique_pack_list.end();) {
			struct udp_pack** pack_data = &unique_pack_list_iter->second;
			if ( now_mstime - (*pack_data)->time > _OVER_SEND_TIME_) {
				this->send_timeout_call((*pack_data)->unique);
				free(*pack_data);
				*pack_data = NULL;
				unique_pack_list.erase(unique_pack_list_iter++);
			} else {
				unique_pack_list_iter++;
			}
		}
		if (unique_pack_list.size() <= 0) {
			this->send_map_list.erase(send_map_list_iter++);
			unique_pack_list.clear();
		} else {
			this->send_map_list[send_map_list_iter->first] = unique_pack_list;
			send_map_list_iter++;
		}
	}
	if (this->send_map_list.size() <= 0) {
		this->send_map_list.clear();
	}
	pthread_mutex_unlock(&(this->send_mutex));
	Exception::save_info("execute_timer:timer_verify_send_pack");
}

/*发送心跳包*/
void UdpBase::timer_heartbeat() {
	Exception::save_info("execute_timer:timer_heartbeat");

	struct udp_pack send_pack = {0x00};
	send_pack.task = _TASK_HEARTBEAT_;
	send_pack.version = _VERSION_;
	send_pack.from_id = this->user_id;
	send_pack.time = this->get_mstime();
	pthread_mutex_lock(&(this->send_mutex));
	this->psp_send(&send_pack, this->psp_server_addr);
	pthread_mutex_unlock(&(this->send_mutex));
}
/*设置心跳包的周长*/
void UdpBase::set_timer_heartbeat(uint32 time) {	//向服务器心跳包
	this->heartbeat_timer = time;
}
void UdpBase::execute_timer() {
	this->current_num++;
	uint32 max_timer = _TIMER_CLEAER_RECV_LOG_;
	if (this->current_num % _TIMER_CLEAER_RECV_LOG_ == 0) {
		this->timer_clear_recv_log();
	}
	if (this->current_num % _TIMER_VERIFY_SEND_PACK_ == 0) {
		this->timer_verify_send_pack();
	}
	if (this->heartbeat_timer > 0 && this->current_num % this->heartbeat_timer == 0) {
		this->timer_heartbeat();
	}
	if (max_timer < _TIMER_VERIFY_SEND_PACK_) {
		max_timer = _TIMER_VERIFY_SEND_PACK_;
	}
	if (max_timer < this->heartbeat_timer) {
		max_timer = this->heartbeat_timer;
	}
	if (this->current_num > max_timer) {
		this->current_num = 0;	//将定时计算器 最大取余,防止长期累加超出数据最大大小
	}

}

void UdpBase::free_send_map(uint64 unique) {	//结束释放某个任务的数据 :time+task_queue
	pthread_mutex_lock(&(this->send_mutex));
	std::map < uint64, std::map<uint16, struct udp_pack*> >::iterator send_map_list_iter;
	send_map_list_iter = this->send_map_list.find(unique);
	if (send_map_list_iter != this->send_map_list.end()) {
		std::map<uint16, struct udp_pack*> unique_map_list = send_map_list_iter->second;
		std::map<uint16, struct udp_pack*> ::iterator unique_map_list_iter;
		for (unique_map_list_iter = unique_map_list.begin(); unique_map_list_iter != unique_map_list.end();) {
			struct udp_pack** pack_data = &unique_map_list_iter->second;
			free(*pack_data);
			*pack_data = NULL;

			unique_map_list.erase(unique_map_list_iter++);
		}
		unique_map_list.clear();
		this->send_map_list.erase(send_map_list_iter);
	}
	pthread_mutex_unlock(&(this->send_mutex));
}

void UdpBase::free_recved_data(uint32 userid, uint64 unique) {
	uint8 **recv_data = NULL;
	pthread_mutex_lock(&(this->read_mutex));
	std::map<uint32, std::map<uint64, uint8*> >::iterator recv_map_list_iter;
	std::map<uint64, uint8*> user_recv_map_list;
	std::map<uint64, uint8*>::iterator user_recv_map_list_iter;
	recv_map_list_iter = this->recv_map_list.find(userid);
	if (recv_map_list_iter != this->recv_map_list.end()) {
		user_recv_map_list = recv_map_list_iter->second;
		user_recv_map_list_iter = user_recv_map_list.find(unique);
		if (user_recv_map_list_iter != user_recv_map_list.end()) {
			/*----*/
			recv_data = &user_recv_map_list_iter->second;
			/*----*/
			free(*recv_data);
			*recv_data = NULL;
			user_recv_map_list.erase(user_recv_map_list_iter);
			user_recv_map_list.clear();
		}
		this->recv_map_list.erase(recv_map_list_iter);
	}
	pthread_mutex_unlock(&(this->read_mutex));
}


void UdpBase::set_verify_recv_log(int type) {
	this->verify_recv_log = type;
}

uint32 UdpBase::clear_recved_log(uint32 userid, uint32 unique) {	//清除日志
	if (this->verify_recv_log == true) {
		pthread_mutex_lock(&(this->recv_log_mutex));
		std::map<uint32, std::map<uint64, std::map<uint16, uint64> > >::iterator recv_map_log_iter;
		std::map<uint64, std::map<uint16, uint64> > user_map_log;
		std::map<uint64, std::map<uint16, uint64> >::iterator user_map_log_iter;
		std::map<uint16, uint64> pack_map_log;
		std::map<uint16, uint64>::iterator pack_map_log_iter;
		recv_map_log_iter = this->recv_map_log.find(userid);
		if (recv_map_log_iter != this->recv_map_log.end()) {
			user_map_log = recv_map_log_iter->second;
			user_map_log_iter = user_map_log.find(unique);
			if (user_map_log_iter != user_map_log.end()) {
				pack_map_log = user_map_log_iter->second;
				pack_map_log.clear();
			}
		}
		pthread_mutex_unlock(&(this->recv_log_mutex));
	}

	return true;
}

void UdpBase::recv_start() {
	uint32 recv_socket_fd = this->psp_socket_fd;
	if (this->p2p_socket_fd > 0) {	//如果是采用了p2p 方式,则系统只接受p2p 的数据
		recv_socket_fd = this->p2p_socket_fd;
	}
	socklen_t len = sizeof(struct sockaddr_in);
	int recv_size = sizeof(struct udp_pack);
	this->read_thread_status = _THREAD_RUN_;
	int recv_num = 0;
	while (1) {
		struct sockaddr_in client_addr = {0x00};
		struct udp_pack recv_upd_pack = {0x00};
		if (this->psp_socket_fd <= 0 && this->p2p_socket_fd <= 0) {
			return;
		}
		if (this->p2p_socket_fd > 0) {
			recv_num = recvfrom(this->p2p_socket_fd, &recv_upd_pack, recv_size, 0, (struct sockaddr *)&client_addr, (socklen_t *)&len);
		} else {
			recv_num = recvfrom(this->psp_socket_fd, &recv_upd_pack, recv_size, 0, (struct sockaddr *)&client_addr, (socklen_t *)&len);

		}
		if (recv_num <= 0) {
			if (this->read_thread_status == _THREAD_EXT_) {
				Exception::save_error("quit recv");

				return;
			}
			Exception::save_error("recvfrom error");
			continue;
		}

		if (this->read_thread_status == _THREAD_EXT_) { //结束该线程
			return;
		}
		if (this->verify_recv_log == true) {
			pthread_mutex_lock(&(this->recv_log_mutex));
			uint64 unique = recv_upd_pack.time * 10000 + recv_upd_pack.unique;
			std::map<uint32, std::map<uint64, std::map<uint16, uint64> > >::iterator recv_map_log_iter;
			std::map<uint64, std::map<uint16, uint64> > user_map_log;
			std::map<uint64, std::map<uint16, uint64> >::iterator user_map_log_iter;
			std::map<uint16, uint64> pack_map_log;
			std::map<uint16, uint64>::iterator pack_map_log_iter;

			recv_map_log_iter = this->recv_map_log.find(recv_upd_pack.from_id);
			if (recv_map_log_iter != this->recv_map_log.end()) {
				user_map_log = recv_map_log_iter->second;
				user_map_log_iter = user_map_log.find(unique);
				if (user_map_log_iter != user_map_log.end()) {
					pack_map_log = user_map_log_iter->second;
					if (pack_map_log.find(recv_upd_pack.sequence) != pack_map_log.end()) {
						pthread_mutex_unlock(&(this->recv_log_mutex));
						return;
					}
				}
				pack_map_log.insert(std::pair<uint16, uint64>(recv_upd_pack.sequence, this->get_mstime()));
				this->recv_map_log[recv_upd_pack.from_id][unique] = pack_map_log;
			} else {
				pack_map_log.insert(std::pair<uint16, uint64>(recv_upd_pack.sequence, recv_upd_pack.sequence));
				user_map_log.insert(std::pair<uint64, std::map<uint16, uint64> >(unique, pack_map_log));
				this->recv_map_log.insert(std::pair<uint32, std::map<uint64, std::map<uint16, uint64> > >(recv_upd_pack.from_id, user_map_log));
			}
			pthread_mutex_unlock(&(this->recv_log_mutex));
		}
		this->recved_pack(recv_upd_pack, client_addr);
	}
	return;
}

void UdpBase::send_timeout_call(uint16 unique) {	//发送数据包的数据回调
	//
}

uint32 UdpBase::get_pack_num(uint32 data_size) {	//数据大小可以拆分成多少个数据包
	int pack_num = 0;
	if (data_size > _UDP_DATA_SIZE_) {
		if (data_size % _UDP_DATA_SIZE_ > 0) {
			pack_num = (data_size - (data_size % _UDP_DATA_SIZE_)) / _UDP_DATA_SIZE_ + 1;
		} else {
			pack_num = data_size / _UDP_DATA_SIZE_;
		}
	} else {
		pack_num = 1;
	}
	return pack_num;
}
uint64 UdpBase::get_mstime() {	//获取系统当前毫秒
	struct timeval tv;
	gettimeofday(&tv, NULL);   //该函数在sys/time.h头文件中
	return  tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
uint32 UdpBase::send_data(void* send_data, int data_size, uint32 send_userid, uint16 task, uint8 send_type) { //拆包发送数据

	if (this->psp_socket_fd <= 0 && this->p2p_socket_fd <= 0) {
		Exception::save_error("please socket server");	//未进行服务器绑定
		return -1;
	}
	if (this->wait_send_queue > _MAX_TASK_LIST_) {
		Exception::save_error("wait_send_queue > max task list");
		return -2;
	}
	if (send_type == _UDP_TYPE_P2P_) {
		if (this->get_already_addrs(send_userid) == NULL) {
			this->get_socket_addrs(send_userid);
		}
	}
	int pack_num = this->get_pack_num(data_size);
	if (this->task_queue >= 65500) {
		this->task_queue = 0;
	}
	this->task_queue++;
	std::map<uint16, struct udp_pack*> send_user_pack_map_save;
	time_t time = this->get_mstime();
	for (int i = 0; i < pack_num; ++i) {
		struct udp_pack *pack_data = (struct udp_pack *)malloc(sizeof(struct udp_pack));
		memset(pack_data, 0x00, sizeof(struct udp_pack));
		pack_data->version = _VERSION_;
		pack_data->max_size = data_size;
		pack_data->type = send_type;
		pack_data->sequence = i;
		pack_data->from_id = this->user_id;
		pack_data->send_id = send_userid;
		pack_data->task = task;
		pack_data->unique = this->task_queue;
		pack_data->time = time;
		if (data_size > _UDP_DATA_SIZE_) {
			if (i < pack_num - 1) {
				memcpy(pack_data->data, (uint8*)send_data + (i * _UDP_DATA_SIZE_), _UDP_DATA_SIZE_);
			} else {
				memcpy(pack_data->data, (uint8*)send_data + (i * _UDP_DATA_SIZE_), data_size % _UDP_DATA_SIZE_);
			}
		} else {
			memcpy(pack_data->data, send_data, data_size % _UDP_DATA_SIZE_);
		}
		send_user_pack_map_save.insert(std::pair<uint16, struct udp_pack*>(i, pack_data));
		if (this->send_thread_status == _THREAD_RUN_) {
			pthread_mutex_lock(&(this->send_mutex));
			this->send_list.push_back(pack_data);
			this->wait_send_queue++;
			pthread_mutex_unlock(&(this->send_mutex));
			pthread_cond_signal(&(this->send_cond));
		} else if (send_type == _UDP_TYPE_PSP_ || send_type == _UDP_TYPE_PGP_) {
			this->psp_send(pack_data, this->psp_server_addr);
		} else {
			this->p2p_send(pack_data, send_userid);
		}
	}
	uint64 unique = time * 10000 + this->task_queue;
	pthread_mutex_lock(&(this->send_mutex));
	this->send_map_list.insert(std::pair < uint64, std::map<uint16, struct udp_pack*> >(unique, send_user_pack_map_save));
	pthread_mutex_unlock(&(this->send_mutex));
	return unique;


}

//向服务器转发数据
uint32 UdpBase::psp_send(struct udp_pack *pack_data, struct sockaddr_in addr) {
	if (this->psp_socket_fd <= 0) {
		Exception::save_error("please socket server");	//未进行服务器绑定
		return -1;
	}
	int len = sizeof(struct sockaddr_in);
	int send_num = sendto(this->psp_socket_fd, pack_data, sizeof(struct udp_pack), 0, (struct sockaddr *)&addr, len);
	if (send_num < sizeof(struct udp_pack)) {
		char err_info[120] = {0x00};
		sprintf(err_info, "psp send faild,error:%d\n", errno);
		Exception::save_error(err_info);
	}
	return send_num;
}
uint32 UdpBase::p2p_send(struct udp_pack* send_pack, struct sockaddr_in addr) {	//p2p 发送数据包给uid,如果uid 没有采用p2p 则psp 发送转发
	uint32 send_socket = this->psp_socket_fd;
	if (this->p2p_socket_fd > 0) {	//rug
		send_socket = this->p2p_socket_fd;
	}
	if (send_socket <= 0) {
		Exception::save_error("please socket server");	//未进行服务器绑定
		return -1;
	}
	int len = sizeof(struct sockaddr_in);
	int send_num = sendto(send_socket, send_pack, sizeof(struct udp_pack), 0, (struct sockaddr *)&addr, len);
	if (send_num < sizeof(struct udp_pack)) {
		char err_info[120] = {0x00};
		sprintf(err_info, "psp send faild,error:%d\n", errno);
		Exception::save_error(err_info);
	}
	return send_num;
}
/*向某个客户端发送数据*/
uint32 UdpBase::p2p_send(struct udp_pack* send_pack, uint32 uid) {

	uint32 send_socket = this->psp_socket_fd;
	if (this->p2p_socket_fd > 0) {	//rug
		send_socket = this->p2p_socket_fd;
	}
	if (send_socket <= 0) {
		Exception::save_error("please socket server");	//未进行服务器绑定
		return -1;
	}
	int len = sizeof(struct sockaddr_in);
	struct extranet_addr *uid_addr = this->get_already_addrs(uid);
	int send_num = 0;
	if (uid_addr == NULL || uid_addr->type == _UDP_TYPE_PSP_) {	//如果uid 的外网地址不存在,则通过服务器转发并返回addr
		send_num = sendto(send_socket, send_pack, sizeof(struct udp_pack), 0, (struct sockaddr *)&this->psp_server_addr, len);
		if (send_num != sizeof(struct udp_pack)) {
			char err_info[120] = {0x00};
			sprintf(err_info, "p2p send faild,error:%d\n", errno);
			Exception::save_error(err_info);
		}
	} else {
		printf("%s\n", "xxx");
		struct sockaddr_in send_addr = {0};
		send_addr.sin_family = AF_INET;
		send_addr.sin_port = htons(uid_addr->port);
		send_addr.sin_addr.s_addr = inet_addr((char*)uid_addr->IPdotdec);
		send_num = sendto(send_socket, send_pack, sizeof(struct udp_pack), 0, (struct sockaddr *)&send_addr, len);
		if (send_num != sizeof(struct udp_pack)) {
			char err_info[120] = {0x00};
			sprintf(err_info, "p2p send faild,error:%d\n", errno);
			Exception::save_error(err_info);
		}
	}
	return send_num;
}


struct extranet_addr* UdpBase::get_already_addrs(uint32 uid) { //获取外网信息
	std::map<uint32, struct extranet_addr*>::iterator extranet_addr_map_iter;
	pthread_mutex_lock(&(this->socket_addr_mutex));
	extranet_addr_map_iter = this->extranet_addr_map.find(uid);
	if (extranet_addr_map_iter != this->extranet_addr_map.end()) {	//存在
		pthread_mutex_unlock(&(this->socket_addr_mutex));
		return  extranet_addr_map_iter->second;
	} else {
		pthread_mutex_unlock(&(this->socket_addr_mutex));
		return NULL;
	}
}

struct extranet_addr* UdpBase::save_addr(uint32 uid, struct sockaddr_in client_addr, uint8 type) {	//保存外网信息
	std::map<uint32, struct extranet_addr*>::iterator extranet_addr_map_iter;
	struct extranet_addr *addr_info = NULL;

	pthread_mutex_lock(&(this->socket_addr_mutex));
	extranet_addr_map_iter = this->extranet_addr_map.find(uid);
	if (extranet_addr_map_iter != this->extranet_addr_map.end()) {	//存在
		addr_info = extranet_addr_map_iter->second;
		addr_info->type = type;
		addr_info->last_time = this->get_mstime();
		addr_info->port = client_addr.sin_port;
		char IPdotdec[20] = {0};
		inet_ntop(AF_INET, &client_addr.sin_addr, IPdotdec, 20);
		memcpy(addr_info->IPdotdec, IPdotdec, 20);
		memcpy(&addr_info->addr, &client_addr, sizeof(struct sockaddr_in));
	} else {
		addr_info = (struct extranet_addr *)malloc(sizeof(struct extranet_addr));
		memset(addr_info, 0x00, sizeof(sizeof(struct extranet_addr)));
		addr_info->type = type;
		addr_info->last_time = this->get_mstime();
		addr_info->port = client_addr.sin_port;
		char IPdotdec[20] = {0x00};
		inet_ntop(AF_INET, &client_addr.sin_addr, IPdotdec, 20);
		memcpy(addr_info->IPdotdec, IPdotdec, 20);
		memcpy(&addr_info->addr, &client_addr, sizeof(struct sockaddr_in));
		this->extranet_addr_map.insert(std::pair<uint32, struct extranet_addr*>(uid, addr_info));
	}
	pthread_mutex_unlock(&(this->socket_addr_mutex));
	return addr_info;
}

uint32 UdpBase::remove_addr(uint32 uid) {	//移除用户外网信息
	pthread_mutex_lock(&(this->socket_addr_mutex));
	std::map<uint32, struct extranet_addr*>::iterator extranet_addr_map_iter;
	extranet_addr_map_iter = this->extranet_addr_map.find(uid);
	if (extranet_addr_map_iter != this->extranet_addr_map.end()) {
		struct extranet_addr* addr_info = extranet_addr_map_iter->second;
		if (addr_info != NULL) {
			free(addr_info);
			addr_info = NULL;
		}
		this->extranet_addr_map.erase(extranet_addr_map_iter);
	}
	pthread_mutex_lock(&(this->socket_addr_mutex));
	return true;
}


uint32 UdpBase::get_socket_addrs(uint32 uid) { //向服务器获取外网信息
	this->task_queue++;
	struct udp_pack command_pack = {0x00};
	command_pack.version = _VERSION_;
	command_pack.type = _UDP_TYPE_PSP_;
	command_pack.task = _TASK_GET_ADDR_;
	command_pack.from_id = this->user_id;
	command_pack.send_id = uid;
	command_pack.time = this->get_mstime();
	command_pack.unique = this->task_queue;
	pthread_mutex_unlock(&(this->send_mutex));
	this->psp_send(&command_pack, this->psp_server_addr);
	pthread_mutex_unlock(&(this->send_mutex));
	return true;
}

uint32 UdpBase::send_server_command(uint8 task, uint32 uid) { //向服务器发送指令

	this->task_queue++;
	struct udp_pack command_pack = {0x00};
	command_pack.version = _VERSION_;
	command_pack.type = _UDP_TYPE_PSP_;
	command_pack.task = task;
	command_pack.from_id = this->user_id;
	command_pack.send_id = uid;
	command_pack.time = this->get_mstime();
	command_pack.unique = this->task_queue;
	pthread_mutex_unlock(&(this->send_mutex));
	this->psp_send(&command_pack, this->psp_server_addr);
	pthread_mutex_unlock(&(this->send_mutex));
	return true;
}


uint32 UdpBase::send_client_command(uint8 task, uint32 uid) { //向p2p 另一段发送指令
	this->task_queue++;
	struct udp_pack command_pack = {0x00};
	command_pack.version = _VERSION_;
	command_pack.type = _UDP_TYPE_P2P_;
	command_pack.task = task;
	command_pack.from_id = this->user_id;
	command_pack.send_id = uid;
	command_pack.time = this->get_mstime();
	command_pack.unique = this->task_queue;
	pthread_mutex_unlock(&(this->send_mutex));
	this->p2p_send(&command_pack, uid);
	pthread_mutex_unlock(&(this->send_mutex));
	return true;
}

struct udp_pack* UdpBase::get_missing_pack(struct udp_pack miss_pack) {	//获取确实的包,用于补包

	uint64 unique = miss_pack.time * 10000 + miss_pack.unique;
	std::map <uint64, std::map<uint16, struct udp_pack*> >::iterator send_map_list_iter;

	std::map<uint16, struct udp_pack*> send_map_list_data;
	send_map_list_iter = this->send_map_list.find(unique);

	if (send_map_list_iter != this->send_map_list.end()) {
		send_map_list_data = send_map_list_iter->second;
		std::map<uint16, struct udp_pack*>::iterator unique_pack_list_iter;
		unique_pack_list_iter = send_map_list_data.find(miss_pack.sequence);
		if (unique_pack_list_iter != send_map_list_data.end()) {
			return unique_pack_list_iter->second;
		}
	}
	return NULL;

}
void UdpBase::create_read_thread() {	//传教读线程
	this->read_thread_status = _THREAD_RUN_ ;	//读线程状态
	pthread_create(&(this->read_thread_id), NULL, read_thread_function, (void *)this);
	return;
}
void UdpBase::close_read_thread() { //关闭读数据线程
	this->read_thread_status = _THREAD_EXT_;
	if (this->psp_socket_fd > 0) {
		close(this->psp_socket_fd);
		this->psp_socket_fd = 0;
	}
	pthread_join(this->read_thread_id, NULL);

}
void UdpBase::create_send_thread() {	//创建写线程
	this->send_thread_status = _THREAD_RUN_ ;	//写线程状态
	pthread_create(&(this->send_thread_id), NULL, send_thread_function, (void *)this);
	return;
}






















