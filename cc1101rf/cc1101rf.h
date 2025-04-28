#pragma once
#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "ELECHOUSE_CC1101_SRC_DRV.h"

namespace esphome {
namespace cc1101rf {

class CC1101RFComponent : public Component {
 public:
  void setup() override {
    ELECHOUSE_cc1101.setSpiPin(6, 4, 5, 7); // SCK, MOSI, MISO, CSN
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(433.92);
    pinMode(17, INPUT); // GDO0
    pinMode(16, INPUT); // GDO2
    ESP_LOGI("cc1101", "CC1101 initialized (SCK=6, MOSI=4, MISO=5, CSN=7, GDO0=17, GDO2=16)");
  }

  void loop() override {
    if (ELECHOUSE_cc1101.CheckRxFifo()) {
      byte buffer[64] = {0};
      byte len = ELECHOUSE_cc1101.ReceiveData(buffer);
      if (len > 0) {
        char data_str[200] = {0};
        for (int i = 0; i < len; i++) {
          sprintf(data_str + strlen(data_str), "%02X ", buffer[i]);
        }
        ESP_LOGI("cc1101", "Received RF: %s", data_str);
        if (this->rf_code_sensor_ != nullptr)
          this->rf_code_sensor_->publish_state(data_str);
      }
    }
  }

  void set_rf_code_sensor(text_sensor::TextSensor *sensor) { rf_code_sensor_ = sensor; }

 protected:
  text_sensor::TextSensor *rf_code_sensor_{nullptr};
};

}  // namespace cc1101rf
}  // namespace esphome
