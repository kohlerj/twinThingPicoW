/*
 * TCPTransport.h
 *
 *  Created on: 5 Mar 2022
 *      Author: jondurrant
 */

#ifndef TCPTRANSPORT_H_
#define TCPTRANSPORT_H_

#define TCP_TRANSPORT_WAIT 10000

#include "MQTTConfig.h"
#include "core_mqtt.h"
#include "core_mqtt_agent.h"

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "lwip/ip4_addr.h"
#include "lwip/ip_addr.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"

}


class TCPTransport {
public:
	TCPTransport();
	virtual ~TCPTransport();


	bool transConnect(const char * host, uint16_t port);
	//bool transConnect(ip_addr_t * ip, uint16_t port);

	int status();

	bool transClose();

	int32_t transSend(NetworkContext_t * pNetworkContext, const void * pBuffer, size_t bytesToSend);
	int32_t transRead(NetworkContext_t * pNetworkContext, void * pBuffer, size_t bytesToRecv);


	static uint32_t getCurrentTime();
	static int32_t staticSend(NetworkContext_t * pNetworkContext, const void * pBuffer, size_t bytesToSend);
	static int32_t staticRead(NetworkContext_t * pNetworkContext, void * pBuffer, size_t bytesToRecv);


private:
	bool transConnect();
	static void dnsCB(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
	void dnsFound(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

	int xSock = 0;

	uint16_t xPort=80;
	ip_addr_t xHost;
	char xHostName[80];

	SemaphoreHandle_t xHostDNSFound;


};

#endif /* TCPTRANSPORT_H_ */
