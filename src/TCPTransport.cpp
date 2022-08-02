/*
 * TCPTransport.cpp
 *
 *  Created on: 5 Mar 2022
 *      Author: jondurrant
 */

#include "TCPTransport.h"
#include <stdlib.h>
#include "pico/stdlib.h"
#include <errno.h>

#define LogError printf


TCPTransport::TCPTransport(){
	xHostDNSFound = xSemaphoreCreateBinary(  );
}


TCPTransport::~TCPTransport() {
	// TODO Auto-generated destructor stub
}


/***
 * Required by CoreMQTT returns time in ms
 * @return
 */
uint32_t TCPTransport::getCurrentTime(){
	return to_ms_since_boot(get_absolute_time ());
}

int32_t TCPTransport::transSend(NetworkContext_t * pNetworkContext, const void * pBuffer, size_t bytesToSend){
	uint32_t dataOut;

	dataOut = write(xSock,(uint8_t *)pBuffer, bytesToSend);
	if (dataOut != bytesToSend){
		printf("Send failed %d\n", dataOut);
	}
	return dataOut;
}


int32_t TCPTransport::transRead(NetworkContext_t * pNetworkContext, void * pBuffer, size_t bytesToRecv){
	int32_t dataIn=0;

	dataIn = read(xSock, (uint8_t *)pBuffer, bytesToRecv);

	if (dataIn < 0){
		if (errno == 0){
			dataIn = 0;
		}
	}

	//printf("Read(%d)=%d\n", bytesToRecv, dataIn);

	//printf("transRead(%d)=%d\n", bytesToRecv, dataIn);
	return dataIn;
}


int32_t TCPTransport::staticSend(NetworkContext_t * pNetworkContext, const void * pBuffer, size_t bytesToSend){
	TCPTransport *t = (TCPTransport *)pNetworkContext->tcpTransport;
	return t->transSend(pNetworkContext, pBuffer, bytesToSend);
}



int32_t TCPTransport::staticRead(NetworkContext_t * pNetworkContext, void * pBuffer, size_t bytesToRecv){
	TCPTransport *t = (TCPTransport *)pNetworkContext->tcpTransport;
	return t->transRead(pNetworkContext, pBuffer, bytesToRecv);
}


bool TCPTransport::transConnect(const char * host, uint16_t port){
	err_t res = dns_gethostbyname(host, &xHost, TCPTransport::dnsCB, this);

	strcpy(xHostName, host);
	xPort = port;

	if (xSemaphoreTake(xHostDNSFound, TCP_TRANSPORT_WAIT) != pdTRUE){
		LogError(("DNS Timeout on Connect"));
		return false;
	}

	return transConnect();
}

bool TCPTransport::transConnect(){
	struct sockaddr_in serv_addr;


	xSock = socket(AF_INET, SOCK_STREAM, 0);
	if (xSock < 0){
		LogError(("ERROR opening socket\n"));
		return false;
	}


	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(xPort);
	memcpy(&serv_addr.sin_addr.s_addr, &xHost, sizeof(xHost));

	printf("xhost %lx\n", xHost);
	printf("xHost %s\n", ipaddr_ntoa(&xHost));

	printf("Serv_addr: %x, %x, %lx\n",
			serv_addr.sin_family,
			serv_addr.sin_port,
			serv_addr.sin_addr.s_addr);


	int res = connect(xSock,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	if (res < 0){
		char *s = ipaddr_ntoa(&xHost);
		printf("ERROR connecting %d to %s port %d\n",res, s, xPort);
		printf("xhost %lx\n", xHost);
		return false;
	}

	int nonblock=1;
	ioctlsocket(xSock, FIONBIO, &nonblock);

	printf("Connect success\n");
	return true;
}

int TCPTransport::status(){
	int error = 0;
	socklen_t len = sizeof (error);
	int retval = getsockopt (xSock, SOL_SOCKET, SO_ERROR, &error, &len);
	return error;
}

bool TCPTransport::transClose(){
	closesocket(xSock);
	return true;
}

void TCPTransport::dnsCB(const char *name, const ip_addr_t *ipaddr, void *callback_arg){
	TCPTransport *self = (TCPTransport *) callback_arg;
	self->dnsFound(name, ipaddr, callback_arg);
}

void TCPTransport::dnsFound(const char *name, const ip_addr_t *ipaddr, void *callback_arg){
	memcpy(&xHost, ipaddr, sizeof(xHost));

	printf("DNS Found %s copied to xHost %s\n", ipaddr_ntoa(ipaddr), ipaddr_ntoa(&xHost));
	xSemaphoreGiveFromISR(xHostDNSFound, NULL );
}
