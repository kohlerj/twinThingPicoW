/*
 * MQTTAgentObserver.cpp
 *
 *  Created on: 23 Nov 2021
 *      Author: jondurrant
 */

#include "MQTTAgentObserver.h"
#include "MQTTConfig.h"

#include <logging_levels.h>
#define LIBRARY_LOG_NAME "MQTT_AGENT_OBSERVER"
#define LIBRARY_LOG_LEVEL LOG_INFO
#define SDK_LOG(X) printf X
#include <logging_stack.h>

MQTTAgentObserver::MQTTAgentObserver() {
	// NOP

}

MQTTAgentObserver::~MQTTAgentObserver() {
	// NOP
}


 void MQTTAgentObserver::MQTTOffline(){
	 LogInfo(("MQTT Offline"));
 }

 void MQTTAgentObserver::MQTTOnline(){
	 LogInfo(("MQTT Online"));
 }

 void MQTTAgentObserver::MQTTSend(){
	 LogInfo(("MQTT Send"));
 }

 void MQTTAgentObserver::MQTTRecv(){
	 LogInfo(("MQTT Receive"));
 }
