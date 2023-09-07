/*
 * MQTTRouterTwin.cpp
 *
 *  Created on: 23 Nov 2021
 *      Author: jondurrant
 */

#include "RouterTwin.h"

#include <logging_levels.h>
#define LIBRARY_LOG_NAME "ROUTER_TWIN"
#define LIBRARY_LOG_LEVEL LOG_INFO
#define SDK_LOG(X) printf X
#include <logging_stack.h>

RouterTwin::RouterTwin() {
  // TODO Auto-generated constructor stub
}

RouterTwin::~RouterTwin() {
  if (pSetTopic != NULL) {
    vPortFree(pSetTopic);
    pSetTopic = NULL;
  }
  if (pGetTopic != NULL) {
    vPortFree(pGetTopic);
    pGetTopic = NULL;
  }
}

/***
 * Initialise the object give the MQTT Interface
 * @param mi = MQTT Interface
 */
RouterTwin::RouterTwin(MQTTInterface *mi) { init(mi); }

/***
 * Initialise the object give the Id and CommInterface
 * @param comm_interface = CommInterface
 */
void RouterTwin::InitializeTopics(CommInterface *comm_interface) {
  if (set_topic_ == nullptr) {
    set_topic_ =
        (char *)pvPortMalloc(MQTTTopicHelper::lenThingSet(mi->getId()));
    if (pSetTopic != NULL) {
      MQTTTopicHelper::getThingSet(pSetTopic, mi->getId());
    } else {
      LogError(("Unable to allocate topic"));
    }
  }

  if (pGetTopic == NULL) {
    pGetTopic = (char *)pvPortMalloc(MQTTTopicHelper::lenThingGet(mi->getId()));
    if (pGetTopic != NULL) {
      MQTTTopicHelper::getThingGet(pGetTopic, mi->getId());
    } else {
      LogError(("Unable to allocate topic"));
    }
  }
}

/***
 * Use the interface to setup all the subscriptions
 * @param comm_interface
 */
void RouterTwin::Subscribe(CommInterface *comm_interface) {
  if (get_topic_ == nullptr) {
    panic("get_topic_ not set");
  }

  if (set_topic_ == nullptr) {
    panic("set_topic_ not set");
  }

  comm_interface->SubToTopic(set_topic_, 1);
  comm_interface->SubToTopic(get_topic_, 1);
}

/***
 * Route the message the appropriate part of the application
 * @param topic
 * @param topic_len
 * @param payload
 * @param payload_len
 * @param comm_interface
 */
void RouterTwin::Route(const char *topic, size_t topic_len, const void *payload,
                       size_t payload_len, CommInterface *comm_interface) {
  if (get_topic_ == nullptr) {
    panic("get_topic_ not set");
  }

  if (set_topic_ == nullptr) {
    panic("set_topic_ not set");
  }

  if (twin_ == NULL) {
    panic("twin_ not set");
  }

  if (strlen(set_topic_) == topic_len) {
    if (memcmp(topic, set_topic_, topic_len) == 0) {
      LogDebug(("Routing STATE SET"));
      twin_->AddMessage((char *)payload, payload_len);
    }
  }
  if (strlen(get_topic_) == topic_len) {
    if (memcmp(topic, get_topic_, topic_len) == 0) {
      LogDebug(("Routing STATE GET"));
      twin_->AddGetMessage();
    }
  }
}

/***
 * set the TwinTask
 * @param state
 */
void RouterTwin::SetTwin(TwinTask *twin) { twin_ = twin; }
