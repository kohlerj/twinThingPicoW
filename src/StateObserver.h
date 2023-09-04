/*
 * StateObserver.h
 *
 * Abstract class receiving state change notifications from State objects
 *
 *  Created on: 21 Sept 2021
 *      Author: jondurrant
 *
 * 	Modified on: 4 Sept 2023
 * 		Author: julienkohler
 */

#ifndef STATE_OBSERVER_H_
#define STATE_OBSERVER_H_
#include <stdint.h>

/***
 * Abstract class receiving state change notifications from State objects
 */
class StateObserver {
 public:
  StateObserver();
  virtual ~StateObserver();

  /***
   * Notification of a change of a state item with the State object.
   * @param dirty_code - Representation of item changed within state. Used
   * to pull back delta
   */
  virtual void NotifyState(uint16_t dirty_code) = 0;
};

#endif /* STATE_OBSERVER_H_ */
