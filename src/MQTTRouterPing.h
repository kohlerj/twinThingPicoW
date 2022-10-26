/*
 * MQTTRouterPing.h
 *
 * Example router responding to Ping message with a pong response
 *
 *  Created on: 31 Oct 2021
 *      Author: jondurrant
 */

#ifndef MQTTROUTERPING_H_
#define MQTTROUTERPING_H_

#include "MQTTConfig.h"
#include "MQTTRouter.h"
#include "MQTTPingTask.h"
#include "MQTTInterface.h"


class MQTTRouterPing : public MQTTRouter{
public:
	/***
	 * Constructor
	 */
	MQTTRouterPing();

	/***
	 * Constructor providing Id for the Thing and MQTT Interface
	 * @param mi - MQTT Interface
	 */
	MQTTRouterPing(MQTTInterface *mi);

	/***
	 * Destructor
	 */
	virtual ~MQTTRouterPing();

	/***
	 * Initialise the object give the Id and MQTT Interface
	 * @param mi = MQTT Interface
	 */
	virtual void init(MQTTInterface *mi);

	/***
	 * Set Task to use for action
	 * @param p = Ping Task
	 * @param mi = MQTT Interface
	 */
	virtual void setPingTask(MQTTPingTask *p, MQTTInterface *mi);

	/***
	 * Use the interface to setup all the subscriptions
	 * @param interface
	 */
	virtual void subscribe(MQTTInterface *interface);

	/***
	 * Route the message the appropriate part of the application
	 * @param topic
	 * @param topicLen
	 * @param payload
	 * @param payloadLen
	 * @param interface
	 */
	virtual void route(const char *topic, size_t topicLen, const void * payload, size_t payloadLen, MQTTInterface *interface);

private:

	MQTTPingTask *pingTask = NULL;

	char * pingTopic = NULL;
	char * pongTopic = NULL;
	char * allPingTopic = NULL;

};

#endif /* MQTTROUTEREXP_H_ */
