/*
 * StateTemp.cpp
 *
 * Temperate state object. Reads Temp from PICO
 *
 *  Created on: 19 Sept 2021
 *      Author: jondurrant
 */

#include "StateTemperature.h"
#include "hardware/adc.h"
#include <stdio.h>
#include <string.h>
#include "json-maker/json-maker.h"

static constexpr uint8_t kTemperatureSlot = 1;
static constexpr uint8_t kBootSlot = 2;

static constexpr float kConversionFactor = 3.3f / (1 << 12);

/***
 * Constructor
 */
StateTemp::StateTemp() {
  // Initialise the temp censor for on chip
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  elements_ = 3;
  json_helpers_[kTemperatureSlot] =
      (StateFunc)&StateTemp::JsonTemperature;  // @suppress("Symbol is not
                                               // resolved")
  json_helpers_[kBootSlot] =
      (StateFunc)&StateTemp::JsonBoot;  // @suppress("Symbol is not
                                        // resolved")
}

/***
 * Destructor
 */
StateTemp::~StateTemp() {
  // TODO Auto-generated destructor stub
}

/***
 * Copy constructor
 * @param other
 */
StateTemp::StateTemp(const StateTemp &other) {
  temperature_ = other.GetTemperature();
}

/***
 * Update Temp from the PICO on chip sensor
 */
void StateTemp::UpdateTemperature() {
  adc_select_input(4);

  float temp = (float)adc_read() * kConversionFactor;
  float temperature = 27.0 - ((temp - 0.706) / 0.001721);
  if (temperature != GetTemperature()) {
    SetTemperature(temperature);
  }
}

/***
 * Set temperate as provided in param
 * @param f celcius
 */
void StateTemp::SetTemperature(float temperature) {
  temperature_ = temperature;
  SetStateDirty(kTemperatureSlot);
}

/***
 * Get temperateure
 * @return celcius
 */
float StateTemp::GetTemperature() const { return temperature_; }

/***
 * Get time since boot in seconds
 * @return
 */
uint32_t StateTemp::GetBootTime() {
  return to_ms_since_boot(get_absolute_time()) / 1000;
}

/***
 * Get temperate in JSON format
 * @param buf
 * @param len
 * @return
 */
char *StateTemp::JsonTemperature(char *buf, uint16_t len) const {
  char *buffer_pointer = buf;
  size_t length = len;
  buffer_pointer = json_double(buffer_pointer, kJsonTemperatureKey,
                               static_cast<double>(GetTemperature()), &length);
  return buffer_pointer;
}

/***
 * Get time since boot in json format
 * @param buf
 * @param len
 * @return
 */
char *StateTemp::JsonBoot(char *buf, uint16_t len) {
  char *buffer_pointer = buf;
  size_t length = len;
  buffer_pointer =
      json_int(buffer_pointer, kJsonBootKey, GetBootTime(), &length);
  return buffer_pointer;
}

/***
 * Update state data from a json structure
 * @param json
 */
void StateTemp::UpdateFromJson(json_t const *json) {
  State::UpdateFromJson(json);
}
