# Swarm Eval Kit custom component for ESPHome

[Swarm](https://swarm.space/) VHF provides low-bandwidth satellite connectivity for IoT devices using ultra-small satellites in a low orbit at 450-550 km altitude. They are spread out like strings of pearls into a series of distributed sun-synchronous orbital planes, allowing them to provide global network coverage. The IoT devices can transmit data messages of 192 bytes to the satellites using a simple 22cm ¼-wave antenna operating in the VHF band. The data is relayed to the Swarm cloud which can push the data through public internet to any site, via various API methods.

The [Swarm Eval Kit](https://swarm.space/product/swarm-eval-kit/) is a device based on ESP32-S2 (FeatherS2) microcontroller connected to the Swarm M138 Modem handling the communications with the satellite network. The device contains various peripherals like display, addressable LEDs, energy sensors which are all supported by ESPHome.
It comes with a [pre-installed, open sourced firmware](https://github.com/Swarm-Technologies/Getting-Started) and it was designed to [test the functionality](https://swarm.space/swarm-eval-kit-quickstart-guide/) of the service.

The ESPHome configuration and custom component for the M138 Modem is an alternative to the original firmware, aimed to extend the functionality of it:

 - Bi-directional communication (Swarm API -> Satellites -> IoT device)
 - Integration with a local automation system (eg. Home Assistant) to broaden the possibility of gathering data to be transmitted, or to trigger local actuators based on commands coming from remote locations
 - Various automations can be implemented, at ESPHome (firmware) or Home Assistant (system) level - they can rely on each other.

Most of the functionality of the original firmware has been re-implemented in ESPHome:

 - Background noise and WiFi RSSI indicator LEDs
 - OLED Display and Buttons, local energy sensors
 - GPS Pinger functionality (compatible with Swarm's Bumblebee Hive)
 - Email Web App functionality (usable through ESPHome API / Home Assistant service call)
 - Direct Modem commands (+automatic checksum calculation)

Some of the functionality is a bit different due to the nature of ESPHome and Home Assistant implementation:

 - Disabling WiFi and the LEDs, changing the GPS pinger interval or turning it off will not reboot the board, they happen instantly. Setting these parameters is also possible throuh Home Assistant user interface. There are switches to turn them on or off, and there's an number input to adjust the GPS ping send interval. ESPHome periodically saves these settings to the flash, if you reset the board before an autosave cycle, the settings will not be kept.
 - The Email Web App is not accessible through ESPHome's web interface. The Web UI shows configured sensors, buttons, switches and a log window. Use a [service call in Home Assistant](https://www.home-assistant.io/docs/scripts/service-calls/) to send the message:
 
    ```yaml
    service: esphome.swarm_1_send_email
    data:
      from_email: your@email.address
      to_email: another@email.address
      subject: Hi from Swarm 1
      message: This is a test message from Swarm 1
    ```
    The contents of `message` will be truncated so that the text fits into the maximum message size supported by Swarm.
    
    Note: For a graphical user interface to call services, in Home Assistant go to _Developer Tools_ > _Services_.
 
 - In AP mode, the ESPHome Web UI only allows providing WiFi credentials to connect to an existing network.
 - ESPHome itself cannot be commanded via Telnet, however, the [Stream server external component](https://github.com/oxan/esphome-stream-server) allows bidirectional forwarding of all the serial communication of the modem to a TCP client on your local network - but it excludes local usage of the modem.
 - Sending direct modem commands is easier through a service call from Home Assistant, because the checksum will be automatically calculated, so it's not needed to manually add it:
    ```yaml
    service: esphome.swarm_1_modem_command
    data:
      nmea_sentence: $FV
    ``` 
    The result can be observed in the log window of ESPHome's web interface or the Dashboard if [UART logging](https://esphome.io/components/uart.html#debugging) remains enabled in the configuration.
 
Additional functionality:

 - With ESPHome connected to Home Assistant (either via its native API or MQTT), various states of the kit can be displayed, monitored and logged in a convenient way.
 - There's also a switch to trun off the screen in order to prevent OLED burn-in (display contents are cleared). It can also be toggled by double-clikcing the button A on the board.
 - To send some arbitrary data from Home Assistant to yourself through Swarm, with an Application ID of your choice, there's another service call available:
    ```yaml
    service: esphome.swarm_1_send_data
    data:
      appID: 9999
      data: WhateverData
    ```
    Note that you have to take care of the proper encoding format and size of `WhateverData` according to Swarm requirements.
 - To receive data through the Swarm infrastructure:
    - Make sure you configure `message_notifications_switch`, `unsolicited_message_appid` and `unsolicited_message_data` options in the `swarm_m138` component.
    - Turn on the switch
    - Send a message using POST to `/hive/api/v1/messages` according to the [API Docs](https://bumblebee.hive.swarm.space/apiDocs)
    - Wait for the message application ID and the data to arrive in Home Assistant and you can [do anything you'd like](https://www.home-assistant.io/docs/automation/trigger/#state-trigger) with it.

## Installation

Prepare your ESPHome firmware binary following the steps of the [documentation](https://esphome.io/). For a start, use the [configuration file](https://github.com/nagyrobi/esphome_swarm_m138/blob/main/swarm_eval_kit.yaml) containing the functionality described above. Make sure to also download the `custom` folder along with the yaml configuration. You can customize it to your environment (use your own names, passwords, Wi-Fi SSIDs etc). 

If you're using the web based Dashboard, click on the config entry's 3-dots menu, choose _Install_ > _Manual Download_ > _Modern Format_, wait for the compile to finish and save the binary on your computer.

Open up your Eval Kit and remove the FeatherS2 module from the mainboard, also disconnect the screen wing from it. Connect it to your computer via an USB-C cable, while holding down the BOOT button, to enter in bootloader mode.

Using [esptool](https://github.com/espressif/esptool), make a backup of the original firmware from the FeatherS2 module:
```
./esptool --port /dev/ttyUSB0 read_flash 0 ALL swarm_original_firmware_backup.bin
```

Make sure the process ends successufully. In case not, use an external USB-TTL adapter connected to the RX/TX pins of the module (might take longer).

To flash your ESPHome binary, power-cycle the module with BOOT button held down, and erase the flash memory:
```
./esptool --port /dev/ttyUSB0 erase_flash
```
Following a successful erasure, power-cycle the module again with BOOT button held down, and program the new firmware binary into the flash memory:

```
./esptool --port /dev/ttyUSB0 --baud 460800 write_flash 0x0 swarm-1-esphome-factory.bin
```

After successful flashing, power-cycle the module normally and wait for ESPHome to boot. If successful, you can put back the module and the display on the mainboard and close the box. Subsequent firmware updates to ESPHome are done Over The Air.
