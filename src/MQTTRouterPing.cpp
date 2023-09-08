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

#include <logging_levels.h>
#define LIBRARY_LOG_NAME "MQTT_ROUTER_PING"
#define LIBRARY_LOG_LEVEL LOG_INFO
#define SdkLog(X) printf X
#include <logging_stack.h>

/***
 * Constructor
 */
MQTTRouterPing::MQTTRouterPing() {
}

/***
 * Constructor providing Id for the Thing and MQTT Interface
 * @param mi - MQTT Interface
 */
MQTTRouterPing::MQTTRouterPing(CommInterface *comm_interface) {
  init(comm_interface);
}

/***
 * Initialise the object give the Id and MQTT Interface
 * @param id = string ID of the Thing
 * @param mi = MQTT Interface
 */
void MQTTRouterPing::init(CommInterface *comm_interface) {
        if (pingTopic == NULL) {
          pingTopic = (char *)pvPortMalloc(MQTTTopicHelper::lenThingTopic(
              comm_interface->GetLocalAddress(), PING));
          if (pingTopic != NULL) {
            MQTTTopicHelper::genThingTopic(
                pingTopic, comm_interface->GetLocalAddress(), PING);
          } else {
            LogError(("Unable to allocate PING topic"));
          }
        }

        if (pongTopic == NULL) {
          pongTopic = (char *)pvPortMalloc(MQTTTopicHelper::lenThingTopic(
              comm_interface->GetLocalAddress(), PONG));
          if (pongTopic != NULL) {
            MQTTTopicHelper::genThingTopic(
                pongTopic, comm_interface->GetLocalAddress(), PONG);
          } else {
            LogError(("Unable to allocate PONG topic"));
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
void MQTTRouterPing::Route(const char *topic, size_t topic_len,
                           const void *payload, size_t payload_len,
                           MQTTInterface *interface) {
        LogInfo( ("MQTTRouterPing(%.*s[%d]: %.*s[%d])\n",topicLen,
			topic, topicLen, payloadLen, (char *)payload, payloadLen) );

	if (allPingTopic == NULL){
		init(interface);
	}

	if (pingTask == NULL){
		return;
	}
        if (strlen(pingTopic) == topic_len) {
                if (memcmp(topic, pingTopic, topic_len) == 0) {
                        pingTask->addPing(payload, payload_len);
                }
        }
        if (strlen(allPingTopic) == topic_len) {
                if (memcmp(topic, allPingTopic, topic_len) == 0) {
                        pingTask->addPing(payload, payload_len);
                }
        }
}

/***
 * Use the interface to setup all the subscriptions
 * @param interface
 */
void MQTTRouterPing::Subscribe(CommInterface *comm_interface) {
        if (allPingTopic == NULL) {
                init(comm_interface);
        }
        comm_interface->SubToTopic(pingTopic, 1);
        comm_interface->SubToTopic(allPingTopic, 1);
}

/***
 * Set Task to use for action
 * @param p
 */
void MQTTRouterPing::setPingTask(MQTTPingTask *p,
                                 MQTTInterface *CommInterface) {
        pingTask = p;
	init(mi);
	pingTask->setPongTopic(pongTopic);
	pingTask->setInterface(mi);
}
