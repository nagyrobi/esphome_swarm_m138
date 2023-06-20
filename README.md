# ESPHome configuration for Swarm Eval Kit

[Swarm](https://swarm.space/) provides low-bandwidth satellite connectivity using ultra-small satellites in a low orbit. Swarm satellites cover every point on Earth, enabling IoT devices to operate in any location. Swarm’s satellites orbit at 450-550 km altitude, spread out like strings of pearls into a series of distributed sun-synchronous orbital planes. This configuration allows the satellites to provide global network coverage. The IoT devices can transmit data messages to the satellites, which relay them through public internet to your site, via various API methods.

The [Swarm Eval Kit](https://swarm.space/product/swarm-eval-kit/) is a device based on ESP32-S2 microcontroller connected to the Swarm M138 Modem handling the communications with the satellite network. The kit contains various peripherals like display, addressable LEDs, energy sensors which are all supported by ESPHome.
The kit comes with a [pre-installed, open sourced firmware](https://github.com/Swarm-Technologies/Getting-Started) which allows [testing the functionality](https://swarm.space/swarm-eval-kit-quickstart-guide/) of the service.

The ESPHome configuration and custom component for the M138 Modem is aimed to extend the functionality of the original firmware:

 - bi-directional communication (Swarm API -> Satellites -> IoT device)
 - integration with a local automation system (eg. Home Assistant) to broaden the possibility of gathering data to be transmitted, or to trigger local actuators based on commands coming from remote locations

Most of the functionality of the original firmware has been re-implemented in ESPHome:

 - background noise and WiFi RSSI indicator LEDs
 - OLED Display and Buttons
 - GPS Pinger functionality (compatible with Swarm's Bumblebee Hive)
 - Email Web App functionality (usable through ESPHome API / Home Assistant service call)

Some of the functionality is a bit different due to the nature of ESPHome and Home Assistant implementation:

 - Disabling WiFi is not possible using the "A" button of the OLED display. The button turns off the screen and the LEDs.
 - The Email Web App is not accessible through ESPHome's web interface. The Web UI shows configured sensors, buttons, switches and a log window.
 - ESPHome itself cannot be commanded via Telnet, however, the [Stream server external component](https://github.com/oxan/esphome-stream-server) allows bidirectional forwarding of all the serial communication of the modem to a TCP client on your local network - but it excludes local usage of the modem.


