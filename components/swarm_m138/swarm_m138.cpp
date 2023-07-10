#include "swarm_m138.h"
#include "esphome/core/log.h"
#include <sstream>
#include <chrono>  // For std::chrono
#include <ctime>   // For std::tm
#include <algorithm> // For std::replace


namespace esphome {
namespace swarm_m138 {

static const char *TAG = "swarm_m138";

void SwarmModem::setup() {
  if (this->modem_has_booted_) {
    this->espinit();
  }
}

void SwarmModem::espinit() {
  ESP_LOGD(TAG, "Initial status query");
  this->write_str("$FV*10\n");      //Firmware Version Read
  this->write_str("$GS @*74\n");    //GPS Fix Quality
  this->write_str("$DT @*70\n");    //Date/Time Status
  this->write_str("$RT @*66\n");    //Receive Test (RSSI)
  this->write_str("$GN @*69\n");    //Geospatial Information
  this->write_str("$MT C=U*12\n");  //Count of unsent messages
  this->write_str("$GJ @*6d\n");    //GPS Jamming/Spoofing Indication
  this->write_str("$MM C=U*0b\n");  //Counting all unread messages
  this->write_str("$MM N=?*6C\n");  //Message notifications status
  this->write_str("$PW @*67\n");    //Power Status
}

void SwarmModem::update() {
  if (this->modem_has_booted_) {
    this->espupdate();
  }
}

void SwarmModem::espupdate() {
  ESP_LOGD(TAG, "Periodic update query");
  this->write_str("$MT C=U*12\n");  //Count of unsent messages
  this->write_str("$GJ @*6d\n");    //GPS Jamming/Spoofing Indication
  this->write_str("$MM C=U*0b\n");  //Counting all unread messages
  this->write_str("$PW @*67\n");    //Power Status
}

void SwarmModem::loop() {
  while (this->available()) {
    uint8_t c;
    this->read_byte(&c);
    this->handle_char_(c);
  }
}

void SwarmModem::handle_char_(uint8_t c) {
  if (c == '\r')
    return;
  if (c == '\n') {
    std::string s(this->rx_message_.begin(), this->rx_message_.end());

    if (!this->modem_has_booted_) {  // consider modem as booted after Modem Status or any other Unsolicited Message
      if ((s.substr(0, 18) == "$M138 BOOT,RUNNING") ||
        (s.substr(0, 3) == "$DT") ||
        (s.substr(0, 3) == "$GJ") ||
        (s.substr(0, 3) == "$GN") ||
        (s.substr(0, 3) == "$GS") ||
        (s.substr(0, 3) == "$PW") ||
        (s.substr(0, 3) == "$RD") ||
        (s.substr(0, 3) == "$RT") ||
        (s.substr(0, 3) == "$TD")) {
          ESP_LOGI(TAG, "Modem has booted OK");
          this->modem_has_booted_ = true;
          this->espinit();
      }
      this->rx_message_.clear();
      return;
    }

    if ((s.substr(0, 18) == "$M138 BOOT,RESTART") || 
      (s.substr(0, 6) == "$RS OK") || 
      (s.substr(0, 6) == "Swarm ") || 
      (s.substr(0, 18) == "$M138 BOOT,POWERON")) {
        ESP_LOGI(TAG, "Modem boot in progress");
        this->modem_has_booted_ = false;
        this->rx_message_.clear();
        return;
    }

    std::replace(s.begin(), s.end(), '*', ',');  // Replace '*' with ',' for easier parsing

    if (s.substr(0, 3) == "$GS") {   // eg. "$GS 159,165,7,0,G3*48\n"
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, ' ');  // Skip "$GS"
      std::getline(iss, token, ',');  // Skip value 1 (159)
      std::getline(iss, token, ',');  // Skip value 2 (165)
      std::getline(iss, token, ',');  // Skip value 3 (7)
      std::getline(iss, token, ',');  // Skip value 4 (0)
      std::getline(iss, token, ',');  // Get value 5 (G3)
      if (this->gps_ != nullptr && !token.empty())
        this->gps_->publish_state(token);  // Publish GPS Fix Quality
    }

    if (s.substr(0, 3) == "$PW") {   // eg. "$PW 3.28700,0.00000,0.00000,0.00000,28.0*3d\n"
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, ' ');  // Skip "$PW"
      std::getline(iss, token, ',');  // Get CPU voltage (3.28700)
      token.erase(std::remove(token.begin(), token.end(), ' '), token.end());  // Remove spaces if any
      if (this->cpuv_ != nullptr && !token.empty() && token != "0.00000")
        this->cpuv_->publish_state(parse_number<float>(token).value_or(0));  // Publish CPU voltage
      std::getline(iss, token, ',');  // Skip value 2 (0.00000)
      std::getline(iss, token, ',');  // Skip value 3 (0.00000)
      std::getline(iss, token, ',');  // Skip value 4 (0.00000)
      std::getline(iss, token, ',');  // Get CPU Temp (28.0)
      token.erase(std::remove(token.begin(), token.end(), ' '), token.end());  // Remove spaces if any
      if (this->cput_ != nullptr && !token.empty() && token != "0.0")
        this->cput_->publish_state(parse_number<float>(token).value_or(0));  // Publish CPU Temp
    }

    if (s.substr(0, 3) == "$RD") {   // eg. "$RD AI=6078,RSSI=-84,SNR=12,FDEV=9,596f7527766520676f74206d61696c21*6e\n"
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, '=');  // Skip "$RD AI="
      std::getline(iss, token, ',');  // Get value 1 (6078)
      if (this->msuapid_ != nullptr && !token.empty())
        this->msuapid_->publish_state(token);  // Publish Unsolicited Message Application ID
      std::getline(iss, token, ',');  // Skip value 2 (RSSI=-84)
      std::getline(iss, token, ',');  // Skip value 3 (SNR=12)
      std::getline(iss, token, ',');  // Skip value 4 (FDEV=9)
      std::getline(iss, token, ',');  // Get value 5 (596f7527766520676f74206d61696c21)
      if (this->msutext_ != nullptr && !token.empty())
        this->msutext_->publish_state(this->hex_to_ascii(token));  // Publish Unsolicited Message Data

    }

    if (s.substr(0, 3) == "$FV") {   // eg. "$FV 2022-10-18T22:38:36,v3.0.1*08\n"
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, ' ');  // Skip "$FV"
      std::getline(iss, token, ',');  // Skip value 1 (2022-10-18T22:38:36)
      std::getline(iss, token, ',');  // Get value 2 (v3.0.1)
      if (this->ver_ != nullptr && !token.empty())
        this->ver_->publish_state(token);  // Publish modem firmware version
    }

    if (s.substr(0, 3) == "$GN") {   // eg. $GN 35.2389,9.8623,456,261,0*15\n
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, ' ');  // Skip "$GN"
      std::getline(iss, token, ',');  // Get value 1 (35.2389)
      if (this->lat_ != nullptr && !token.empty())
        this->lat_->publish_state(token);  // Publish latitude

      std::getline(iss, token, ',');  // Get value 2 (9.8623)
      if (this->lon_ != nullptr && !token.empty())
        this->lon_->publish_state(token);  // Publish longitude

      std::getline(iss, token, ',');  // Get value 3 (456)
      if (this->alt_ != nullptr && !token.empty())
        this->alt_->publish_state(token);  // Publish altitude

      std::getline(iss, token, ',');  // Get value 4 (261)
      if (this->cou_ != nullptr && !token.empty())
        this->cou_->publish_state(token);  // Publish course

      std::getline(iss, token, ',');  // Get value 5 (0)
      if (this->spe_ != nullptr && !token.empty())
        this->spe_->publish_state(token);  // Publish speed
    }

    if (s.substr(0, 3) == "$GJ") {   // eg. $GJ 1,23*31\n
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, ' ');  // Skip "$GJ"
      std::getline(iss, token, ',');  // Get value 1 (1)
      if (this->spf_ != nullptr && !token.empty()) { // Publish spoof_state
        if (token == "0") {
          this->spf_->publish_state("Inactive");  
        } else if (token == "1") {
          this->spf_->publish_state("No spoofing");  
        } else if (token == "2") {
          this->spf_->publish_state("Spoofing ");  
        } else if (token == "3") {
          this->spf_->publish_state("Multi spoofing");  
        }
      }
      std::getline(iss, token, ',');  // Get value 2 (23)
      token.erase(std::remove(token.begin(), token.end(), ' '), token.end());  // Remove spaces if any
      if (this->jam_ != nullptr && !token.empty() && token != "0")
        this->jam_->publish_state(parse_number<float>(token).value_or(0));  // Publish jamming_level
    }

    if (s.substr(0, 8) == "$RT RSSI") {  //eg. "$RT RSSI=-104*18\n"
      std::istringstream iss(s);
      std::string token;
      std::string tokrssi;
      std::string tokrsnr;
      std::getline(iss, token, '=');  // Skip "$RT RSSI="
      std::getline(iss, tokrssi, ',');  // Get RSSI value
      std::getline(iss, tokrsnr, '=');  // to see if this is a background, or a satellite RSSI
      tokrssi.erase(std::remove(tokrssi.begin(), tokrssi.end(), ' '), tokrssi.end());  // Remove spaces if any
      if (tokrsnr == "SNR") {
        if (this->rssi_sat_ != nullptr && !tokrssi.empty() && tokrssi != "0")
          this->rssi_sat_->publish_state(parse_number<float>(tokrssi).value_or(0));  // Publish rssi_sat
      } else {
        if (this->rssi_bak_ != nullptr && !tokrssi.empty() && tokrssi != "0")
          this->rssi_bak_->publish_state(parse_number<float>(tokrssi).value_or(0));  // Publish rssi_bkgnd
      }
    }

    if (s.substr(0, 3) == "$MT") {  //eg. "$MT 0*09\n"
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, ' ');  // Skip "$MT"
      std::getline(iss, token, ',');  // Number of unsent messages
      token.erase(std::remove(token.begin(), token.end(), ' '), token.end());  // Remove spaces if any
      if (this->msgt_ != nullptr && !token.empty())
        this->msgt_->publish_state(parse_number<float>(token).value_or(0));  // Publish number of unsent messages
    }

    if (s.substr(0, 7) == "$MM AI=") {  //eg. "$MM AI=0,68692066726f6d20737761726d,4428826476689,1605639598*65\n"
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, '=');  // Skip "MM AI="
      std::getline(iss, token, ',');  // Get appID
      if (this->msgapid_ != nullptr && !token.empty())
        this->msgapid_->publish_state(token);  // Publish application ID
      std::getline(iss, token, ',');  // Get Text data
      if (this->msgtext_ != nullptr)
        this->msgtext_->publish_state(this->hex_to_ascii(token));  // Publish text

    } else if (s.substr(0, 6) == "$MM N=") {  //eg. "$MM N=E*16\n"
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, '=');  // Skip "$MM N="
      std::getline(iss, token, ',');  // Message notifications status, D=disabled, E=enabled
      if (this->msg_noti_switch_ != nullptr && !token.empty()) { // Publish notification switch state
        if (token == "E") {
          this->msg_noti_switch_->publish_state(true);  
        } else if (token == "D") {
          this->msg_noti_switch_->publish_state(false);  
        } 
      }

    } else if (s.substr(0, 3) == "$MM") {  //eg. "$MM 3*13\n"
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, ' ');  // Skip "$MM"
      std::getline(iss, token, ',');  // Number of unread messages
      token.erase(std::remove(token.begin(), token.end(), ' '), token.end());  // Remove spaces if any
      if (this->msgu_ != nullptr && !token.empty())
        this->msgu_->publish_state(parse_number<float>(token).value_or(0));  // Publish number of unread messages
    }

    if (s.substr(0, 3) == "$DT") {  // eg. "$DT 20230608174442,V*47\n"
      std::istringstream iss(s);
      std::string token;
      std::getline(iss, token, ' ');  // Skip "$DT"
      std::getline(iss, token, ',');  // Get date and time value
      std::string flag;
      std::getline(iss, flag, ',');   // Get flag value

      if (flag == "V") {
        // Only with Valid flag, extract date and time components
        int year, month, day, hour, minute, second;
        year = std::stoi(token.substr(0, 4));
        month = std::stoi(token.substr(4, 2));
        day = std::stoi(token.substr(6, 2));
        hour = std::stoi(token.substr(8, 2));
        minute = std::stoi(token.substr(10, 2));
        second = std::stoi(token.substr(12, 2));

        if (!iss.fail()) {
          // Construct std::tm object
          std::tm tm = {};
          tm.tm_year = year - 1900;  // Years since 1900
          tm.tm_mon = month - 1;     // Months since January (0-based)
          tm.tm_mday = day;
          tm.tm_hour = hour;
          tm.tm_min = minute;
          tm.tm_sec = second;

          // Convert std::tm to Unix epoch
          auto time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm));
          auto epoch = std::chrono::duration_cast<std::chrono::seconds>(time_point.time_since_epoch()).count();

          if (this->tme_ != nullptr)
            this->tme_->publish_state(static_cast<float>(epoch));  // Publish time_epoch
        }
      }
    }

    if (s.substr(0, 6) == "$TD OK") {
      ESP_LOGI(TAG, "Message accepted");
    }
    if (s.substr(0, 8) == "$TD SENT") {
      ESP_LOGI(TAG, "Message sent");
    }

    this->rx_message_.clear();
    return;
  }
  this->rx_message_.push_back(c);
}

void SwarmModem::dump_config() {
  LOG_TEXT_SENSOR("", "Text sensor", this->gps_);
  LOG_TEXT_SENSOR("", "Text sensor", this->lat_);
  LOG_TEXT_SENSOR("", "Text sensor", this->lon_);
  LOG_TEXT_SENSOR("", "Text sensor", this->alt_);
  LOG_TEXT_SENSOR("", "Text sensor", this->cou_);
  LOG_TEXT_SENSOR("", "Text sensor", this->spe_);
  LOG_TEXT_SENSOR("", "Text sensor", this->ver_);
  LOG_TEXT_SENSOR("", "Text sensor", this->spf_);
  LOG_TEXT_SENSOR("", "Text sensor", this->msgapid_);
  LOG_TEXT_SENSOR("", "Text sensor", this->msgtext_);
  LOG_TEXT_SENSOR("", "Text sensor", this->msuapid_);
  LOG_TEXT_SENSOR("", "Text sensor", this->msutext_);
  LOG_SENSOR("", "Sensor", this->rssi_bak_);
  LOG_SENSOR("", "Sensor", this->rssi_sat_);
  LOG_SENSOR("", "Sensor", this->tme_);
  LOG_SENSOR("", "Sensor", this->msgt_);
  LOG_SENSOR("", "Sensor", this->jam_);
  LOG_SENSOR("", "Sensor", this->msgu_);
  LOG_SENSOR("", "Sensor", this->cpuv_);
  LOG_SENSOR("", "Sensor", this->cput_);
  LOG_SWITCH("", "Switch", this->msg_noti_switch_);
  LOG_BUTTON("", "Button", this->del_unsent_button_);
  LOG_BUTTON("", "Button", this->del_received_button_);
  LOG_BUTTON("", "Button", this->read_newest_button_);
  LOG_BUTTON("", "Button", this->restart_modem_button_);
}

void SwarmModemMsgNotifSw::dump_config() {
}

void SwarmModemMsgNotifSw::write_state(bool state) {
  this->parent_->write_msgnotif_sw(state);
  this->publish_state(state);
}

void SwarmModem::write_msgnotif_sw(bool state) {
  if (state) {
    this->write_str(this->appennd_checksum_newline("$MM N=E").c_str());  //enable Message notifications
  } else {
    this->write_str(this->appennd_checksum_newline("$MM N=D").c_str());  //disable Message notifications
  }
}


void SwarmModemDelUnsentMsg::dump_config() {
}

void SwarmModemDelUnsentMsg::press_action() {
  this->parent_->delunsentmsg();
}

void SwarmModem::delunsentmsg() {
  ESP_LOGI(TAG, "Deleting unsent messages");
  this->write_str("$MT D=U*15\n");  //delete
  this->write_str("$MT C=U*12\n");  //count msgs (should be 0) to update sensor
}


void SwarmModemDelReceivedMsg::dump_config() {
}

void SwarmModemDelReceivedMsg::press_action() {
  this->parent_->delrcvdtmsg();
}

void SwarmModem::delrcvdtmsg() {
  ESP_LOGI(TAG, "Deleting all received messages");
  this->write_str("$MM D=**73\n");  //delete
  this->write_str("$MM C=U*0b\n");  //count msgs (should be 0) to update sensor
}

void SwarmModemReadNewestMsg::dump_config() {
}

void SwarmModemReadNewestMsg::press_action() {
  this->parent_->readnewstmsg();
}

void SwarmModem::readnewstmsg() {
  ESP_LOGI(TAG, "Reading newest messages");
//  this->write_str("$MM R=N*01\n");  //delete
  this->write_str(this->appennd_checksum_newline("$MM R=N").c_str());  //delete
}

void SwarmModemRestart::dump_config() {
}

void SwarmModemRestart::press_action() {
  this->parent_->restartmodem();
}

void SwarmModem::restartmodem() {
  ESP_LOGI(TAG, "Restarting modem");
  this->write_str(this->appennd_checksum_newline("$RS").c_str());  //restart
}


std::string SwarmModem::appennd_checksum_newline(const std::string& sentence) {
  uint8_t checksum = 0;
  for (size_t i = 1; i < sentence.size(); ++i) {
    checksum ^= sentence[i];
  }
  char checksumStr[4];
  snprintf(checksumStr, sizeof(checksumStr), "%02X", checksum);
  return sentence + "*" + checksumStr + "\n";
}

std::string SwarmModem::hex_to_ascii(const std::string& token) {
    std::string asciiText;
    for (std::size_t i = 0; i < token.length(); i += 2) {
        std::string byteString = token.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
        asciiText.push_back(byte);
    };
  return asciiText;
}

}  // namespace swarm_m138
}  // namespace esphome

