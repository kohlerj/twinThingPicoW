/*
 * State.cpp
 *
 * State superclass to hold the state of a Twin Thing object.
 *
 *  Created on: 19 Sept 2021
 *      Author: jondurrant
 */

#include "State.h"
#include "json-maker/json-maker.h"
#include <stdio.h>
#include "TwinProtocol.h"

#include <logging_levels.h>
#define LIBRARY_LOG_NAME "STATE"
#define LIBRARY_LOG_LEVEL LOG_INFO
#define SdkLog(X) printf X
#include <logging_stack.h>

static constexpr uint8_t kTransactionSlot = 0;

/***
 * Constructor
 */
State::State() {
  for (uint16_t i = 0; i < elements_; i++) {
    json_helpers_[i] = NULL;
  }
  elements_ = 1;
  json_helpers_[kTransactionSlot] = (StateFunc)&State::GetJsonTransactionNumber;
}

/***
 * Destructor
 */
State::~State() {
  // NOP
}

/***
 * Copy constuctor - no items so blank
 * @param other - to copy from
 */
State::State(const State &other) {
  // TODO Auto-generated constructor stub
}

/***
 * Pull back a delta description in JSON format.
 * @param buf - Buffer to write it to
 * @param len - Len of buffer
 * @return len of json. If 0 then has overwritten end.
 */
uint16_t State::GetDelta(char *buf, uint16_t len) {
  uint16_t len_out = GetDelta(buf, len, dirty_code_);
  return len_out;
}

/***
 *Pull back a delta of state in JSON format
 * @param buf - buffer to write JSON to
 * @param len - length of buffer
 * @param dirtyCode - code representing the items changed
 * @return length of json or 0 if we exceeded buffer
 */
uint16_t State::GetDelta(char *buf, uint16_t len, uint16_t dirty_code) {
  char *buffer_pointer = buf;
  size_t length = len;

  if (len < 10) {
    return 0;  // It will never fit
  }

  buffer_pointer = json_objOpen(buffer_pointer, NULL, &length);
  buffer_pointer = json_objOpen(buffer_pointer, TWINDELTA, &length);

  uint16_t mask = 0;

  for (uint16_t i = 0; i < elements_; i++) {
    mask = 1 << i;
    if (mask & dirty_code) {
      if (json_helpers_[i] != NULL) {
        buffer_pointer = (this->*json_helpers_[i])(
            buffer_pointer, len - (buffer_pointer - buf));
      }
      if (buffer_pointer > len + buf) {
        return 0;
      }
    }
  }
  buffer_pointer = json_objClose(buffer_pointer, &length);
  buffer_pointer = json_objClose(buffer_pointer, &length);
  buffer_pointer = json_end(buffer_pointer, &length);
  if (buffer_pointer > len + buf) {
    return 0;
  }
  return buffer_pointer - buf;
}

/***
 * Return state of object as json
 * @param buf - buffer to write json to
 * @param len - length of buffer
 * @return length of json or zero if we ran out of space
 */
uint16_t State::GetState(char *buf, uint16_t len) {
  char *buffer_pointer = buf;
  size_t length = len;

  if (len < 10) {
    return 0;  // It will never fit
  }
  buffer_pointer = json_objOpen(buffer_pointer, NULL, &length);
  buffer_pointer = json_objOpen(buffer_pointer, TWINSTATE, &length);

  for (uint16_t i = 0; i < elements_; i++) {
    if (json_helpers_[i] != NULL) {
      buffer_pointer = (this->*json_helpers_[i])(buffer_pointer,
                                                 len - (buffer_pointer - buf));
    }
    if (buffer_pointer > len + buf) {
      return 0;
    }
  }
  buffer_pointer = json_objClose(buffer_pointer, &length);
  buffer_pointer = json_objClose(buffer_pointer, &length);
  buffer_pointer = json_end(buffer_pointer, &length);

  if (buffer_pointer > len + buf) {
    return 0;
  }
  return buffer_pointer - buf;
}

/***
 * Set state as clean so no delta available
 */
void State::SetStateClean() { dirty_code_ = 0; }

/***
 * Is state dirty_code_ since last notification was sent.
 * Will only be true in a transaction
 * @return bool
 */
bool State::IsStateDirty() const { return (dirty_code_ != 0); }

/***
 * Set the given element to be dirty.
 * Only 8 slots available
 * @param element
 */
void State::SetStateDirty(uint8_t element) {
  if (element < kNumberOfStateElements) {
    uint16_t mask = 1 << element;
    dirty_code_ = dirty_code_ | mask;
    if (!in_transaction_) {
      NotifyObservers();
    }
  } else {
    LogWarn(("SetStateDirty element too high 0x%x", element));
  }
}

/***
 * Attach an observer which will be notified of changes
 * @param observer
 */
void State::Attach(StateObserver *observer) { observers_.push_back(observer); }

/***
 * Detach observer
 * @param observer
 */
void State::Detach(StateObserver *observer) { observers_.remove(observer); }

/***
 * Send notifications of change to all observers_
 */
void State::NotifyObservers() {
  auto iterator = observers_.begin();
  while (iterator != observers_.end()) {
    (*iterator)->NotifyState(dirty_code_);
    ++iterator;
  }
  SetStateClean();
}

/***
 * Start a transaction and supress notifications until transaction is complete
 */
void State::StartTransaction() { in_transaction_ = true; }

/***
 * Complete transactions and notify all observers
 */
void State::CommitTransaction() {
  in_transaction_ = false;
  if (IsStateDirty()) {
    NotifyObservers();
  } else {
    LogDebug(("Transaction completed with no change to state"));
  }
}

void State::UpdateFromJson(json_t const *json) {
  json_t const *json_pointer;

  json_pointer = json_getProperty(json, kJsonTransactionNumberKey);
  if (json_pointer != nullptr) {
    if (JSON_INTEGER == json_getType(json_pointer)) {
      int value = json_getInteger(json_pointer);
      if (value > 0) {
        SetTransactionNumber(static_cast<uint32_t>(value));
      }
    }
  }
}

/***
 * Set transaction number. Used by twin client on a host to confirm updates
 * have been processed by pico
 * @param newTRN
 */
void State::SetTransactionNumber(uint32_t new_transaction_number) {
  transaction_number_ = new_transaction_number;
  SetStateDirty(kTransactionSlot);
}

/***
 * get transaction code. Used by twin client on a host to confirm updates
 * have been processed by pico
 * @return trn
 */
uint32_t State::GetTransactionNumber() { return transaction_number_; }

/***
 * Retrieve transaction number in JSON format
 * @param buf
 * @param len
 * @return
 */
char *State::GetJsonTransactionNumber(char *buf, uint16_t len) {
  char *buffer_pointer = buf;
  size_t length = len;
  buffer_pointer = json_uint(buffer_pointer, kJsonTransactionNumberKey,
                             GetTransactionNumber(), &length);
  return buffer_pointer;
}
