/*
 * StateTask.cpp
 *
 *  Created on: 22 Nov 2021
 *      Author: jondurrant
 */

#include "TwinTask.h"
#include "TwinTopicHelper.h"
#include "TwinProtocol.h"
#include <cstring>

#include <logging_levels.h>
#define LIBRARY_LOG_NAME "TWIN_TASK"
#define LIBRARY_LOG_LEVEL LOG_INFO
#define SdkLog(X) printf X
#include <logging_stack.h>

static constexpr uint8_t kDepthDirtyQueue = 10;

/***
 * Constructor
 */
TwinTask::TwinTask(CommInterface *comm_interface)
    : Agent("TwinTask", 512, 3, 0x03) {
  notify_dirty_queue_ = new Queue(kDepthDirtyQueue, sizeof(uint16_t));

  x_message_buffer_ = xMessageBufferCreate(kMaxStateMessageLength);
  if (x_message_buffer_ == nullptr) {
    LogError(("Create message buffer failed"));
    return;
  }

  SetCommunicationInterface(comm_interface);

  Start();
}

/***
 * Destructor
 */
TwinTask::~TwinTask() {
  if (update_topic_ != nullptr) {
    vPortFree(update_topic_);
    update_topic_ = nullptr;
  }

  delete notify_dirty_queue_;

  if (x_message_buffer_ != nullptr) {
    vMessageBufferDelete(x_message_buffer_);
    x_message_buffer_ = nullptr;
  }
}

/***
 * Set the state object to be used
 * @param state
 */
void TwinTask::SetStateObject(State *state) {
  p_state_ = state;
  p_state_->Attach(this);
}

/***
 * Get the state object
 * @return
 */
State *TwinTask::GetStateObject() { return p_state_; }

/***
 * Set the communication interface to be used
 * @param comm_interface
 */
void TwinTask::SetCommunicationInterface(CommInterface *comm_interface) {
  comm_interface_ = comm_interface_;
  if (update_topic_ == nullptr) {
    update_topic_ = (char *)pvPortMalloc(
        TwinTopicHelper::GetInstance()->GetStateUpdateTopicLength(
            comm_interface_->GetId()));
    if (update_topic_ != nullptr) {
      TwinTopicHelper::GetInstance()->GetStateUpdateTopic(
          update_topic_, comm_interface_->GetId());
    } else {
      LogError(("Update topic allocation failed"));
    }
  }
}

/***
 * Add a message to the message buffer
 * @param msg
 * @param msg_len
 * @return
 */
bool TwinTask::AddMessage(const char *msg, size_t msg_len) {
  if (x_message_buffer_ != nullptr) {
    size_t res = xMessageBufferSend(x_message_buffer_, msg, msg_len, 0);
    if (res != msg_len) {
      LogError(("AddMessage failed. Msg len:%d  Buf:%d Msg:%s\n", msg_len,
                xMessageBufferSpacesAvailable(x_message_buffer_), msg));
      return false;
    }
  } else {
    LogError(("No message buffer was created"));
    return false;
  }
  return true;
}

/***
 * Internal function to run the task from within the object
 */
void TwinTask::Run() {
  uint16_t dirty_code;

  while (true) {
    // Handle incoming requests
    if (xMessageBufferIsEmpty(x_message_buffer_) == pdTRUE) {
      taskYIELD();
    } else {
      Delay(20);
      size_t size = xMessageBufferReceive(x_message_buffer_, message_,
                                          kMaxStateMessageLength, 0);
      if (size > 0) {
        if (p_state_ != nullptr) {
          ProcessMessage(message_);
        }
      }
    }

    // Handle delta publishing
    if (notify_dirty_queue_->Dequeue((void *)&dirty_code, 0)) {
      if (comm_interface_ != nullptr) {
        uint16_t tmp;
        tmp = p_state_->GetDelta(message_, kMaxStateMessageLength, dirty_code);
        if (tmp == 0) {
          LogError(("Buffer overrun"));
        }

        comm_interface_->PubToTopic(update_topic_, message_, strlen(message_));
      }
    }
  }
}

/***
 * Process a json message received
 * @param str
 */
void TwinTask::ProcessMessage(char *str) {
  json_t const *json = json_create(str, json_buffer_, json_buffer_length_);
  if (json == nullptr) {
    LogError(("Error creating json buffer for %s", str));
    return;
  }

  // Delta processing
  json_t const *delta = json_getProperty(json, kTwinDeltaKey);

  // If no delta then check for state processing
  if (delta == nullptr) {
    delta = json_getProperty(json, kTwinStateKey);
  }

  // Handle delta or state
  if (delta != nullptr) {
    p_state_->StartTransaction();
    p_state_->UpdateFromJson(delta);
    p_state_->CommitTransaction();
  }

  // Get processing
  if (delta == nullptr) {
    delta = json_getProperty(json, kTwinGetKey);
    if (delta != nullptr) {
      LogDebug(("Handling Get"));
      p_state_->GetState(message_, kMaxStateMessageLength);
      comm_interface_->PubToTopic(update_topic_, message_, strlen(message_), 1);
    }
  }

  ProcessJsonMessage(json);

  if (p_state_ != nullptr) {
    if (p_state_->IsStateDirty()) {
      LogDebug(("State is dirty so sending delta"));
      p_state_->GetDelta(message_, kMaxStateMessageLength);
      comm_interface_->PubToTopic(update_topic_, message_, strlen(message_), 1);
    } else {
      LogDebug(("State clean after processing json"));
    }
  }
}

/***
 * Process a json message received
 * Extend this for subclass processing
 * @param json
 */
void TwinTask::ProcessJsonMessage(json_t const *json) {
  // NOP
}

/***
 * Notification of a change of a state item with the State object.
 * @param dirty_code - Representation of item changed within state. Used to pull
 * back delta
 */
void TwinTask::NotifyState(uint16_t dirty_code) {
  // Queue dirty code to be handled from within the task
  BaseType_t x_higher_priority_task_woken;
  notify_dirty_queue_->EnqueueFromISR((void *)&dirty_code,
                                      &x_higher_priority_task_woken);
}
