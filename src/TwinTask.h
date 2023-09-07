/*
 * StateTask.h
 * Task to manage state updates for publishing and processing of MQTT
 *
 *  Created on: 22 Nov 2021
 *      Author: jondurrant
 *  Modified on: 4 Sept 2023
 *      Author: julienkohler
 */

#ifndef TWIN_TASK_H_
#define TWIN_TASK_H_

#include "State.h"
#include "StateObserver.h"
#include <FreeRTOS.h>
#include <message_buffer.h>
#include <task.h>
#include "queue.hpp"
#include <stdlib.h>
#include "tiny-json.h"
#include "Agent.h"
#include "CommInterface.h"

#include "TwinTopicHelper.h"

static constexpr uint8_t kMaxStateMessageLength = 255;
static constexpr uint8_t kMaxNumberOfJsonObjects = 25;

static constexpr char *kTwinDeltaKey = "delta";
static constexpr char *kTwinStateKey = "state";
static constexpr char *kTwinGetKey = "get";
static constexpr char *kTwinGetPayload = "{\"get\":1}";
static constexpr uint8_t kTwinGetPayloadLength = strlen(kTwinGetPayload);

class TwinTask : public Agent, StateObserver {
 public:
  /***
   * Constructor
   */
  TwinTask(CommInterface *comm_interface);

  /***
   * Destructor
   */
  virtual ~TwinTask();

  /***
   * @brief Set the state object to be used
   * @param state
   */
  void SetStateObject(State *state);

  /***
   * @brief Get the state object
   * @return
   */
  State *GetStateObject();

  /***
   * @brief Add message to the message buffer
   * @param msg
   * @param msg_len
   * @return
   */
  bool AddMessage(const char *msg, size_t msg_len);

  /***
   * @brief Add get message to the message buffer
   * @return
   */
  bool AddGetMessage() {
    return AddMessage(kTwinGetPayload, kTwinGetPayloadLength);
  }

  /***
   * Notification of a change of a state item with the State object.
   * @param dirty_code - Representation of item changed within state. Used to
   * pull back delta
   */
  virtual void NotifyState(uint16_t dirty_code);

 protected:
  /***
   * Internal function to run the task from within the object
   */
  void Run();

 private:
  /***
   * @brief Set the communication interface to be used
   * @param comm_interface
   */
  void SetCommunicationInterface(CommInterface *comm_interface);

  /***
   * Process a json message received
   * @param str
   */
  virtual void ProcessMessage(char *str);

  MessageBufferHandle_t x_message_buffer;
  Queue *notify_dirty_queue;

  CommInterface *comm_interface;

  State *p_state = nullptr;

  char message[kMaxStateMessageLength];
  char *update_topic = nullptr;

  json_t json_buffer[kMaxNumberOfJsonObjects];
  uint8_t json_buffer_length = kMaxNumberOfJsonObjects;

  TwinTopicHelper *topic_helper;
};

#endif /* TWIN_TASK_H_ */
