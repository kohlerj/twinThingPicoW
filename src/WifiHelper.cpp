/*
 * WifiAgent.cpp
 *
 *  Created on: 24 Nov 2021
 *      Author: jondurrant
 */

#include "WifiHelper.h"
#include "MQTTConfig.h"

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"

#include "FreeRTOS.h"
#include "task.h"


WifiHelper::WifiHelper() {
	// TODO Auto-generated constructor stub

}

WifiHelper::~WifiHelper() {
	// TODO Auto-generated destructor stub
}



/***
 * Get IP address of unit
 * @param ip - output uint8_t[4]
 */
bool WifiHelper::getIPAddress(uint8_t *ip) {
	memcpy(ip, netif_ip4_addr(&cyw43_state.netif[0]), 4);
}

/***
 * Get IP address of unit
 * @param ips - output char * up to 16 chars
 * @return - true if IP addres assigned
 */
bool WifiHelper::getIPAddressStr(char *ips) {
	char *s = ipaddr_ntoa(netif_ip4_addr(&cyw43_state.netif[0]));

	strcpy(ips, s);


	return true;

}


bool WifiHelper::getMACAddressStr(char *macStr) {
	uint8_t mac[6];
	int r =  cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA,  mac);

	if (r == 0) {
		for (uint8_t i = 0; i < 6; i++) {
			if (mac[i] < 16){
				sprintf(&macStr[i*2], "0%X", mac[i]);
			} else {
				sprintf(&macStr[i*2], "%X", mac[i]);
			}
		}
		macStr[13] = 0;
		return true;
	}
	return false;
}

bool WifiHelper::syncRTCwithSNTP() {

}


bool WifiHelper::isJoined(){
	int res = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
	return (res >= 0);
}


