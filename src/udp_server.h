#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_


#include <iostream>

#include "types.h"
#include "exception.h"
#include "udp_base.h"

class UdpServer : public UdpBase {

  public:
	UdpServer(int port);
	~UdpServer();
	void recved_pack(struct udp_pack, struct sockaddr_in addr);
};

#endif