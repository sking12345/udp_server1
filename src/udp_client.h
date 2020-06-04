#ifndef _UDP_CLIENT_
#define _UDP_CLIENT_


#include <iostream>
#include "types.h"
#include "exception.h"
#include "udp_base.h"

class UdpClient : public UdpBase {
  public:
	uint32 login_status;
  public:
	UdpClient(const char* ip, int port);
	~UdpClient();
	void login(uint32 userid);
	void p2p_login(uint32 userid);
	void quit_login(uint32 userid);
	void p2p_start();	//p2p2 开始
	void recved_pack(struct udp_pack, struct sockaddr_in addr);
	void recved_data(uint8 * recv_data, uint32 data_size, uint16 task, uint32 userid, uint8 type, uint64 unique = 0x00 );

};
#endif