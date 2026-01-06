/*
 * EVBox Component for ESPHome
 * Author: A.J. van de Werken
 * GitHub: ajvdw
 */

#include "evbox.h"
#include "esphome/core/log.h"
#include <cstring>
#include <cmath>

namespace esphome {
namespace evbox {

static const char *const TAG = "evbox";

void EVBox::setup() {
  ESP_LOGD(TAG, "evbox.setup()");

  ESP_LOGCONFIG(TAG, "Setting up EVBox...");
  
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
    this->flow_control_pin_->digital_write(false);  // Set to receive mode
    ESP_LOGCONFIG(TAG, "  Flow control pin configured");
  }
}

void EVBox::loop() {
  // Capture EVBox data 
  while (this->available()) {
    uint8_t c;
    this->read_byte(&c);
    
    if (c == MESSAGE_START) {
      // Message Start  
      this->receiving_ = true;
      this->received_len_ = 0;
    } else if (c == MESSAGE_END && this->received_len_ > MESSAGE_MIN_LENGTH) {
      // Message End 
      this->receiving_ = false;
      this->received_data_[this->received_len_] = 0;
      this->process_message_(reinterpret_cast<char *>(this->received_data_));
      this->received_len_ = 0;   
    } else if (this->receiving_ && c >= 48 && c <= 70) {
      // Capture message data (ASCII '0'-'F')
      if (this->received_len_ < MAX_MESSAGE_LENGTH) {
        this->received_data_[this->received_len_++] = c;
      }
    } else {
      // Invalid data
      this->received_len_ = 0;
      this->receiving_ = false;
    }
  }
}

void EVBox::process_message_(char *msg) {
  size_t msglen = strlen(msg);
  
  // Check length and header
  if (msglen != EXPECTED_MESSAGE_LENGTH || strncmp(msg, "A08069", 6) != 0) {
    return;
  }
  
  ESP_LOGD(TAG, "Processing received message");
  
  // Calculate checksums
  uint8_t checksum_sum = 0;
  uint8_t checksum_xor = 0;
  for (size_t i = 0; i < 52; i++) {
    checksum_sum = (checksum_sum + msg[i]) & 0xFF;
    checksum_xor ^= msg[i];
  }
  
  // Validate checksums
  if (HEX_CHARS[checksum_sum >> 4] != msg[52] || 
      HEX_CHARS[checksum_sum & 0x0F] != msg[53] ||
      HEX_CHARS[checksum_xor >> 4] != msg[54] || 
      HEX_CHARS[checksum_xor & 0x0F] != msg[55]) {
    ESP_LOGW(TAG, "Checksum validation failed");
    return;
  }
  
  // Read MID metered values from EVSE
  char meter[9];
  meter[8] = '\0';

  
  if (this->total_energy_sensor_ != nullptr) {
    strncpy(meter, &msg[44], 8);
    float total_energy = 0.001f * strtoul(meter, nullptr, 16);
    this->total_energy_sensor_->publish_state(total_energy);
  }
  
  // Read phase currents
  char current[5];
  current[4] = '\0';
  
  if (this->l1_current_sensor_ != nullptr) {
    strncpy(current, &msg[20], 4);
    float l1_current = 0.1f * strtoul(current, nullptr, 16);
    this->l1_current_sensor_->publish_state(l1_current);
  }
  
  if (this->l2_current_sensor_ != nullptr) {
    strncpy(current, &msg[24], 4);
    float l2_current = 0.1f * strtoul(current, nullptr, 16);
    this->l2_current_sensor_->publish_state(l2_current);
  }
  
  if (this->l3_current_sensor_ != nullptr) {
    strncpy(current, &msg[28], 4);
    float l3_current = 0.1f * strtoul(current, nullptr, 16);
    this->l3_current_sensor_->publish_state(l3_current);
  }
}

void EVBox::set_current(float amp) {
  // MaxChargingCurrent command, minimal current 6 A. To stop charging use 0 A.
  char buf[35] = "80A06900__00__00__003C003C003C003C";
  uint16_t current_value;

  if( amp != 0.0 && amp < 6.0 ) amp = 6.0; // minimum charging current
  
  current_value = static_cast<uint16_t>(std::round(amp * 10.0f));

  // Set current values (fill in the blanks)
  buf[8] = buf[12] = buf[16] = HEX_CHARS[(current_value >> 4) & 0x0F];
  buf[9] = buf[13] = buf[17] = HEX_CHARS[current_value & 0x0F];
  
  // Calculate checksums
  uint8_t checksum_sum = 0;
  uint8_t checksum_xor = 0;
  for (size_t i = 0; buf[i] != '\0'; i++) {
    checksum_sum = (checksum_sum + buf[i]) & 0xFF;
    checksum_xor ^= buf[i];
  }
  
  ESP_LOGD(TAG, "Setting charge current: %.1f A", amp);
  
  // Enable TX mode
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->digital_write(true);
  }
  
  // Start of message
  this->write_byte(MESSAGE_START);
  
  // Message body
  this->write_array(reinterpret_cast<const uint8_t *>(buf), 34);
  
  // Checksums
  this->write_byte(HEX_CHARS[checksum_sum >> 4]);
  this->write_byte(HEX_CHARS[checksum_sum & 0x0F]);
  this->write_byte(HEX_CHARS[checksum_xor >> 4]);
  this->write_byte(HEX_CHARS[checksum_xor & 0x0F]);
  
  // End of message
  this->write_byte(MESSAGE_END);
  this->flush();
  
  // Return to RX mode
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->digital_write(false);
  }
}

}  // namespace evbox
}  // namespace esphome
