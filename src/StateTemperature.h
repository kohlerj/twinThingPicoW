/*
 * StateTemp.h
 *
 * Useful base building block State that has temperature of PICO
 *
 *  Created on: 19 Sept 2021
 *      Author: jondurrant
 */

#ifndef STATE_TEMPERATURE_H_
#define STATE_TEMPERATURE_H_

#include "State.h"
#include "pico/stdlib.h"

static constexpr char kJsonTemperatureKey[] = "temp";
static constexpr char kJsonBootKey[] = "boot";

/***
 * Temperature state class reading temp from PICO and holding time since boot
 * Note time since boot is only provide in state json and never in delta
 */
class StateTemp : public State {
 public:
  StateTemp();
  virtual ~StateTemp();
  StateTemp(const StateTemp &other);

  /***
   * Update temperature from PICO
   */
  void UpdateTemperature();
  /***
   * Get temperature in celcius
   * @return celcius
   */
  float GetTemperature() const;
  /***
   * Set temerature in celcius
   * @param
   */
  void SetTemperature(float);

  /***
   * Get time since boot in seconds
   * @return seconds
   */
  static uint32_t GetBootTime();

  /***
   * Update state data from a json structure
   * @param json
   */
  virtual void UpdateFromJson(json_t const *json);

 protected:
  /***
   * get temperature in json format
   * @param buf
   * @param len
   * @return
   */
  char *JsonTemperature(char *buf, uint16_t len) const;
  /***
   * get boot time in json format.
   * @param buf
   * @param len
   * @return
   */
  static char *JsonBoot(char *buf, uint16_t len);

 private:
  float temperature_ = 0.0;
};

#endif /* STATE_TEMPERATURE_H_ */
