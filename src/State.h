/*
 * State.h
 *
 * State superclass to hold the state of a Twin Thing object.
 *
 *  Created on: 19 Sept 2021
 *      Author: jondurrant
 */

#ifndef STATE_H_
#define STATE_H_

#include <stdbool.h>
#include <list>

#include "tiny-json.h"
#include "MQTTConfig.h"
#include "StateObserver.h"

static constexpr uint8_t kNumberOfStateElements = 16;

static constexpr char kJsonTransactionNumberKey[] = "trn";

// Declaration for State function that will be used as array of member functions
// in class
class State;
typedef char *(State::*StateFunc)(char *buffer, uint32_t len);

/***
 * Abstract state function to manage twin communication from PICO to a host
 * device Note class is limited to max of 8 state items!
 */
class State {
 public:
  /***
   * Set transaction number. Used by twin client on a host to confirm updates
   * have been processed by pico
   * @param new_transaction_number
   */
  virtual void SetTransactionNumber(uint32_t new_transaction_number);

  /***
   * get transaction code. Used by twin client on a host to confirm updates
   * have been processed by pico
   * @return trn
   */
  virtual uint32_t GetTransactionNumber();

  /***
   * Retrieve delta of changes to state in JSON format
   * @param buf - buffer to write to
   * @param len - length of buffer
   * @return length of json or zero if we ran out of space
   */
  virtual uint16_t GetDelta(char *buf, uint16_t len);

  /***
   * Retrieve delta of changes to state in JSON format
   * @param buf - buffer to write to
   * @param len - length of buffer
   * @param dirty_code - dirtyCode which shows which elements changed.
   * @return
   */
  virtual uint16_t GetDelta(char *buf, uint16_t len, uint16_t dirty_code);

  /***
   * Retrieve state of object in JSON format
   * @param buf - buffer to write to
   * @param len - length of buffer
   * @return length of json or zero if we ran out of space
   */
  virtual uint16_t GetState(char *buf, uint16_t len);

  /***
   * Update state data from a json structure
   * @param json
   */
  virtual void UpdateFromJson(json_t const *json);

  /***
   * Start a transaction and supress notifications of change until transaction
   * is complete
   */
  virtual void StartTransaction();

  /***
   * End transaction and notify all observers
   */
  virtual void CommitTransaction();

  /***
   * Set state to clean for delta
   */
  virtual void SetStateClean();

  /***
   * Check if delta is currently available. Should be false except in transation
   * @return bool
   */
  virtual bool IsStateDirty() const;

  /***
   * Attach an observer
   * @param observer
   */
  void Attach(StateObserver *observer);
  /***
   * Detach observer
   * @param observer
   */
  void Detach(StateObserver *observer);

 protected:
  /***
   * Protect constructor as virtual
   */
  State();
  /***
   * Destructor
   */
  virtual ~State();
  /***
   * Copy constructor
   * @param other
   */
  State(const State &other);

  // List of the 16 helpder functions that will serialise the 16 elements to
  // JSON
  StateFunc json_helpers_[kNumberOfStateElements];
  // Actual number of elements, must be bellow 16
  uint8_t elements_ = 0;

  /***
   * Set specific element (0 to 15) to be dirty
   * @param element
   */
  void SetStateDirty(uint8_t element);

  /***
   * Notify all observers
   */
  void NotifyObservers();

  /***
   * Retrieve transaction number in JSON format
   * @param buf
   * @param len
   * @return
   */
  char *GetJsonTransactionNumber(char *buf, uint16_t len);

 private:
  // bit associated with the state element is set when changed
  uint16_t dirty_code_ = 0;

  // Observer list who will be notified of change
  std::list<StateObserver *> observers_;

  // If we are in transaction don't notify until commit
  bool in_transaction_ = false;

  // Current transaction number
  uint32_t transaction_number_ = 0;
#define STATETRN "trn"
};

#endif /* STATE_H_ */
