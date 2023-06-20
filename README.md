# ESPHome configuration for Swarm Eval Kit

[Swarm](https://swarm.space/) provides low-bandwidth satellite connectivity using ultra-small satellites in a low orbit. Swarm satellites cover every point on Earth, enabling IoT devices to operate in any location. Swarm’s satellites orbit at 450-550 km altitude, spread out like strings of pearls into a series of distributed sun-synchronous orbital planes. This configuration allows the satellites to provide global network coverage. The IoT devices can transmit data messages to the satellites, which relay them through public internet to your site, via various API methods.

The [Swarm Eval Kit](https://swarm.space/product/swarm-eval-kit/) is a device based on ESP32-S2 (FeatherS2) microcontroller connected to the Swarm M138 Modem handling the communications with the satellite network. The kit contains various peripherals like display, addressable LEDs, energy sensors which are all supported by ESPHome.
The kit comes with a [pre-installed, open sourced firmware](https://github.com/Swarm-Technologies/Getting-Started) which allows [testing the functionality](https://swarm.space/swarm-eval-kit-quickstart-guide/) of the service.

The ESPHome configuration and custom component for the M138 Modem is aimed to extend the functionality of the original firmware:

 - bi-directional communication (Swarm API -> Satellites -> IoT device)
 - integration with a local automation system (eg. Home Assistant) to broaden the possibility of gathering data to be transmitted, or to trigger local actuators based on commands coming from remote locations

Most of the functionality of the original firmware has been re-implemented in ESPHome:

 - background noise and WiFi RSSI indicator LEDs
 - OLED Display and Buttons
 - GPS Pinger functionality (compatible with Swarm's Bumblebee Hive)
 - Email Web App functionality (usable through ESPHome API / Home Assistant service call)
 - Direct Modem commands (with automatic checksum calculation (through ESPHome API / Home Assistant service call)

Some of the functionality is a bit different due to the nature of ESPHome and Home Assistant implementation:

 - Disabling WiFi is not possible using the "A" button of the OLED display. The button turns off the screen and the LEDs.
 - The Email Web App is not accessible through ESPHome's web interface. The Web UI shows configured sensors, buttons, switches and a log window.
 - ESPHome itself cannot be commanded via Telnet, however, the [Stream server external component](https://github.com/oxan/esphome-stream-server) allows bidirectional forwarding of all the serial communication of the modem to a TCP client on your local network - but it excludes local usage of the modem.

Additional functionality:

 - To receive data through the Swarm infrastrucure, make sure you configure `message_notifications_switch`, `unsolicited_message_appid` and `unsolicited_message_data` options in the `swarm_m138` component.
 - Send a message using POST to `/hive/api/v1/messages` according to the [API Docs](https://bumblebee.hive.swarm.space/apiDocs)
 - Wait for the message to arrive in Home Assistant and do anything you'd like with it.

## Installation

Prepare your ESPHome firmware binary following the steps of the [documentation](https://esphome.io/). If you're using the Dashboard, click on the config entry's 3-dots menu, choose _Install_ > _Manual Download_ > _Modern Format_, wait for the compile to finish and save the binary on your computer.

Open up your Eval Kit and remove the FeatherS2 module from the mainboard, also disconnect the screen wing from it. Connect it to your compute via an USB-C cable, while holding down the BOOT button, to enter in bootloader mode.

Make a backup of the original firmware from the FeatherS2 module:
```
./esptool --port /dev/ttyUSB0 read_flash 0 ALL swarm_original_firmware_backup.bin
```

Make sure the process ends successufully. In case not, use an external USB-TTL adapter connected to the RX/TX pins of the module.

To flash your ESPHome binary, power-cycle the module with BOOT button held down, and erase the flash memory:
```
./esptool --port /dev/ttyUSB0 erase_flash
```
Afters successful erase, power-cycle the module again with BOOT button held down, and write the new firmware:

```
./esptool --port /dev/ttyUSB0 --baud 460800 write_flash 0x0 swarm-1-esphome-factory.bin
```

After successful flashing, power-cycle the module normally and wait for ESPHome to come up. If successful, you can put back the module and the display on the mainboard and close the box. Subsequent firmware updates to ESPHome are done Over The Air.