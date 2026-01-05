/*
 * EVBox Component for ESPHome
 * Author: A.J. van de Werken
 * GitHub: ajvdw
 */

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/gpio.h"

namespace esphome {
namespace evbox {

static const uint8_t MESSAGE_START = 0x02;
static const uint8_t MESSAGE_END = 0x03;
static const uint8_t MESSAGE_MIN_LENGTH = 8;
static const uint8_t EXPECTED_MESSAGE_LENGTH = 56;
static const size_t MAX_MESSAGE_LENGTH = 255;

class EVBox : public uart::UARTDevice, public Component {
 public:
  void setup() override;
  void loop() override;
  
  // Set charging current in amps
  void set_current(float amp);
  
  // Set flow control pin for RS485
  void set_flow_control_pin(GPIOPin *pin) { this->flow_control_pin_ = pin; }
  
  // Sensor setters
  void set_l1_current_sensor(sensor::Sensor *sensor) { this->l1_current_sensor_ = sensor; }
  void set_l2_current_sensor(sensor::Sensor *sensor) { this->l2_current_sensor_ = sensor; }
  void set_l3_current_sensor(sensor::Sensor *sensor) { this->l3_current_sensor_ = sensor; }
  void set_total_energy_sensor(sensor::Sensor *sensor) { this->total_energy_sensor_ = sensor; }
  
 protected:
  void process_message_(char *msg);
  
  GPIOPin *flow_control_pin_{nullptr};
  sensor::Sensor *l1_current_sensor_{nullptr};
  sensor::Sensor *l2_current_sensor_{nullptr};
  sensor::Sensor *l3_current_sensor_{nullptr};
  sensor::Sensor *total_energy_sensor_{nullptr};
  
  // Message reception state
  bool receiving_{false};
  uint8_t received_len_{0};
  uint8_t received_data_[MAX_MESSAGE_LENGTH + 1];
  
  // Hex lookup table
  static constexpr char HEX_CHARS[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                          '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
};

}  // namespace evbox
}  // namespace esphome
