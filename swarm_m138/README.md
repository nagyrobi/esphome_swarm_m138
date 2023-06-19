# A demo component to interface with Swarm Eval Kit containing an M138 modem

See https://swarm.space/product/swarm-eval-kit/ for more details on the hardware.

A configured uart is required.

Example:
```yaml
swarm_m138:
  - uart_id: swarm_uart
    latitude:
      name: "Latitude"
      id: swarm_lat
    longitude:
      name: "Longitude"
      id: swarm_lon
    altitude:
      name: "Altitude"
      id: swarm_alt
    course:
      name: "Course"
      id: swarm_cou
    speed:
      name: "Speed"
      id: swarm_spe
    time_epoch:
      name: "Epoch Time"
      id: epochtime
    gps_fix:
      name: "GPS Fix Quality"
      id: gps_fix
    gps_spoof_state:
      name: "Spoof state"
    gps_jamming_level:
      name: "Jamming level"
    modem_cpu_voltage:
      name: "CPU Voltage"
    modem_cpu_temperature:
      name: "CPU Temperature"
    modem_fw_ver:
      name: "Modem firmware"
    delete_unsent_button:
      name: "Delete unsent messages"
    delete_all_received_button:
      name: "Delete received messages"
    read_newest_message_button:
      name: "Read newest message"
    message_notifications_switch:
      name: "Message notifications"
      icon: mdi:message-fast
    unsent_messages:
      name: "Unsent messages"
    received_unread_messages:
      name: "Unread messages"
    received_message_appid:
      name: "Message AppID"
    received_message_data:
      name: "Message Text"
    unsolicited_message_appid:
      name: "Unsolicited AppID"
    unsolicited_message_data:
      name: "Unsolicited Text"
    restart_modem_button:
      name: "Modem Restart"
    rssi:
      name: "RSSI"
      id: swarm_rssi
```

