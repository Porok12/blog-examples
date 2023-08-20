# Raspberry Pico C: Remote Sensor

Go to `File | Settings | Build, Execution, Deployment | CMake` and set:

* CMake options `-DTCP_PORT=8008 -DTCP_SERVER_IP=192.168.1.17 -DWIFI_PASSWORD=... -DWIFI_SSID=...`
* Environment variables `PICO_BOARD=pico_w;PICO_SDK_PATH=/path/to/pico-sdk`
