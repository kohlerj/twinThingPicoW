/*
 * MQTTRouterPing.cpp
 *
 *  Created on: 31 Oct 2021
 *      Author: jondurrant
 */

#include <stdio.h>
#include <cstring>

#include "MQTTRouterPing.h"
#include "MQTTTopicHelper.h"


#define PING "PING"
#define PONG "PONG"
#define ALL  "ALL"


/***
 * Constructor
 */
MQTTRouterPing::MQTTRouterPing() {
}

/***
 * Constructor providing Id for the Thing and MQTT Interface
 * @param mi - MQTT Interface
 */
MQTTRouterPing::MQTTRouterPing(MQTTInterface *mi) {
	init(mi);
}

/***
 * Initialise the object give the Id and MQTT Interface
 * @param id = string ID of the Thing
 * @param mi = MQTT Interface
 */
void MQTTRouterPing::init(MQTTInterface *mi) {
	if (pingTopic == NULL){
		pingTopic = (char *)pvPortMalloc(
				MQTTTopicHelper::lenThingTopic(mi->getId(), PING)
				);
		if (pingTopic != NULL){
			MQTTTopicHelper::genThingTopic(pingTopic, mi->getId(), PING);
		} else {
			LogError( ("Unable to allocate PING topic") );
		}
	}

	if (pongTopic == NULL){
		pongTopic = (char *)pvPortMalloc(
				MQTTTopicHelper::lenThingTopic(mi->getId(), PONG)
				);
		if (pongTopic != NULL){
			MQTTTopicHelper::genThingTopic(pongTopic, mi->getId(), PONG);
		} else {
			LogError( ("Unable to allocate PONG topic") );
		}
	}

	if (allPingTopic == NULL){
		allPingTopic = (char *)pvPortMalloc(
				MQTTTopicHelper::lenGroupTopic(ALL, PING)
				);
		if (allPingTopic != NULL){
			MQTTTopicHelper::genGroupTopic(allPingTopic, ALL, PING);
		} else {
			LogError( ("Unable to allocate ALL topic") );
		}
	}

}

/***
 * Destructor
 */
MQTTRouterPing::~MQTTRouterPing() {
	if (pingTopic != NULL){
		vPortFree(pingTopic);
		pingTopic = NULL;
	}
	if (pongTopic != NULL){
		vPortFree(pongTopic);
		pongTopic = NULL;
	}
}


/***
 * Route the message the appropriate part of the application
 * @param topic
 * @param topicLen
 * @param payload
 * @param payloadLen
 * @param interface
 */
void MQTTRouterPing::route(const char *topic, size_t topicLen, const void * payload, size_t payloadLen, MQTTInterface *interface){

	LogInfo( ("MQTTRouterPing(%.*s[%d]: %.*s[%d])\n",topicLen,
			topic, topicLen, payloadLen, (char *)payload, payloadLen) );

	if (allPingTopic == NULL){
		init(interface);
	}

	if (pingTask == NULL){
		return;
	}
	if (strlen(pingTopic) == topicLen){
		if (memcmp(topic, pingTopic, topicLen)==0){
			pingTask->addPing(payload, payloadLen);
		}
	}
	if (strlen(allPingTopic) == topicLen){
		if (memcmp(topic, allPingTopic, topicLen)==0){
			pingTask->addPing(payload, payloadLen);
		}
	}
}

/***
 * Use the interface to setup all the subscriptions
 * @param interface
 */
void MQTTRouterPing::subscribe(MQTTInterface *interface){
	if (allPingTopic == NULL){
		init(interface);
	}
	interface->subToTopic(pingTopic, 1);
	interface->subToTopic(allPingTopic, 1);
}

/***
 * Set Task to use for action
 * @param p
 */
void MQTTRouterPing::setPingTask(MQTTPingTask *p, MQTTInterface *mi){
	pingTask = p;
	init(mi);
	pingTask->setPongTopic(pongTopic);
	pingTask->setInterface(mi);
}


