/*
 * WifiAgent.h
 *
 *  Created on: 24 Nov 2021
 *      Author: jondurrant
 */

#ifndef SRC_WIFIHELPER_H_
#define SRC_WIFIHELPER_H_

#include "MQTTConfig.h"
#include <stdlib.h>


class WifiHelper {
public:
	WifiHelper();

	virtual ~WifiHelper();



	/***
	 * Get IP address of unit
	 * @param ip - output uint8_t[4]
	 * @return - true if IP addres assigned
	 */
	static bool getIPAddress(uint8_t *ip);

	/***
	 * Get IP address of unit
	 * @param ips - output char * up to 16 chars
	 * @return - true if IP addres assigned
	 */
	static bool getIPAddressStr(char *ips);

	static bool getMACAddressStr(char *macStr);

	static bool syncRTCwithSNTP();

	static bool isJoined();

private:


};

#endif /* SRC_WIFIHELPER_H_ */

