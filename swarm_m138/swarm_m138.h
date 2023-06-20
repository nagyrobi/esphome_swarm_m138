#pragma once

#include "esphome/core/component.h"
#include "esphome/components/button/button.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

#include <sstream>
#include <algorithm> // For std::replace


namespace esphome {
namespace swarm_m138 {

class SwarmModem : public PollingComponent,  public uart::UARTDevice {
 public:
  float get_setup_priority() const override { return setup_priority::LATE; }
  void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;

  void set_gps(text_sensor::TextSensor *text_sensor) { gps_ = text_sensor; }
  void set_lat(text_sensor::TextSensor *text_sensor) { lat_ = text_sensor; }
  void set_lon(text_sensor::TextSensor *text_sensor) { lon_ = text_sensor; }
  void set_alt(text_sensor::TextSensor *text_sensor) { alt_ = text_sensor; }
  void set_cou(text_sensor::TextSensor *text_sensor) { cou_ = text_sensor; }
  void set_spe(text_sensor::TextSensor *text_sensor) { spe_ = text_sensor; }
  void set_ver(text_sensor::TextSensor *text_sensor) { ver_ = text_sensor; }
  void set_spf(text_sensor::TextSensor *text_sensor) { spf_ = text_sensor; }
  void set_msgapid(text_sensor::TextSensor *text_sensor) { msgapid_ = text_sensor; }
  void set_msgtext(text_sensor::TextSensor *text_sensor) { msgtext_ = text_sensor; }
  void set_msuapid(text_sensor::TextSensor *text_sensor) { msuapid_ = text_sensor; }
  void set_msutext(text_sensor::TextSensor *text_sensor) { msutext_ = text_sensor; }
  void set_rssi(sensor::Sensor *sensor) { rssi_ = sensor; }
  void set_tme(sensor::Sensor *sensor) { tme_ = sensor; }
  void set_msgt(sensor::Sensor *sensor) { msgt_ = sensor; }
  void set_jam(sensor::Sensor *sensor) { jam_ = sensor; }
  void set_msgu(sensor::Sensor *sensor) { msgu_ = sensor; }
  void set_cpuv(sensor::Sensor *sensor) { cpuv_ = sensor; }
  void set_cput(sensor::Sensor *sensor) { cput_ = sensor; }
  void set_msg_noti_switch(switch_::Switch *sw) { this->msg_noti_switch_ = sw; }
  
  void write_binary(bool value);
  void write_msgnotif_sw(bool value);
  void espinit();
  void espupdate();
  void delunsentmsg();
  void delrcvdtmsg();
  void readnewstmsg();
  void restartmodem();

  std::string appennd_checksum_newline(const std::string& sentence);
  std::string hex_to_ascii(const std::string& token);

 protected:
  bool modem_has_booted_{false};

  text_sensor::TextSensor *gps_{nullptr};
  text_sensor::TextSensor *lat_{nullptr};
  text_sensor::TextSensor *lon_{nullptr};
  text_sensor::TextSensor *alt_{nullptr};
  text_sensor::TextSensor *cou_{nullptr};
  text_sensor::TextSensor *spe_{nullptr};
  text_sensor::TextSensor *ver_{nullptr};
  text_sensor::TextSensor *spf_{nullptr};
  text_sensor::TextSensor *msgapid_{nullptr};
  text_sensor::TextSensor *msgtext_{nullptr};
  text_sensor::TextSensor *msuapid_{nullptr};
  text_sensor::TextSensor *msutext_{nullptr};
  sensor::Sensor *rssi_{nullptr};
  sensor::Sensor *tme_{nullptr};
  sensor::Sensor *msgt_{nullptr};
  sensor::Sensor *jam_{nullptr};
  sensor::Sensor *msgu_{nullptr};
  sensor::Sensor *cpuv_{nullptr};
  sensor::Sensor *cput_{nullptr};
  switch_::Switch *msg_noti_switch_{nullptr};

  void handle_char_(uint8_t c);
  std::vector<uint8_t> rx_message_;
};

class SwarmModemDelUnsentMsg : public Component, public button::Button {
 public:
  void dump_config() override;
  void set_parent(SwarmModem *parent) { this->parent_ = parent; }
 protected:
  void press_action() override;
  SwarmModem *parent_;
};

class SwarmModemDelReceivedMsg : public Component, public button::Button {
 public:
  void dump_config() override;
  void set_parent(SwarmModem *parent) { this->parent_ = parent; }
 protected:
  void press_action() override;
  SwarmModem *parent_;
};

class SwarmModemReadNewestMsg : public Component, public button::Button {
 public:
  void dump_config() override;
  void set_parent(SwarmModem *parent) { this->parent_ = parent; }
 protected:
  void press_action() override;
  SwarmModem *parent_;
};

class SwarmModemRestart : public Component, public button::Button {
 public:
  void dump_config() override;
  void set_parent(SwarmModem *parent) { this->parent_ = parent; }
 protected:
  void press_action() override;
  SwarmModem *parent_;
};

class SwarmModemMsgNotifSw : public Component, public switch_::Switch {
 public:
  void dump_config() override;
  void set_parent(SwarmModem *parent) { this->parent_ = parent; }
 protected:
  void write_state(bool state) override;
  SwarmModem *parent_;
};

}  // namespace swarm_m138
}  // namespace esphome

