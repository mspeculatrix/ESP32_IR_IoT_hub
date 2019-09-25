# ESP32_IR_IoT_hub

ESP32-based IR receiver for IoT

This is an ESP32-based infrared receiver intended to be used with standard infrared remote controls. It uses the classic, cheap as chips 1838T infrared sensor/receiver.

The idea is to have a single receiver that can be used to control multiple devices in the home by sending out MQTT messages in response to codes sent by the remote control.

I may extend it at some time to also interact with a REST API running on my home intranet server.

Read the blog for more information on this project: https://mansfield-devine.com/speculatrix/category/projects/home-control/esp32-ir-hub/
