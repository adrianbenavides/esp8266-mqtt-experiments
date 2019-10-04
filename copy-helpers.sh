#!/usr/bin/env bash

dirs=(
    "mqtt-0-config-devices"
    "mqtt-1-two-devices"
    "mqtt-2-last-will"
    "mqtt-full-example"
)

for dir in ${dirs[@]}
do
    cp -arf "./arduino-helpers/lib" "./${dir}/"
    cp -arf "./arduino-helpers/src/mqtt.cpp" "./${dir}/src/"
    cp -arf "./arduino-helpers/src/nodemcu.cpp" "./${dir}/src/"
    cp -arf "./arduino-helpers/include/mqtt.h" "./${dir}/include/"
    cp -arf "./arduino-helpers/include/nodemcu.h" "./${dir}/include/"
    cp -arf "./arduino-helpers/platformio.ini" "./${dir}/"
    cp -arf "./arduino-helpers/.editorconfig" "./${dir}/"
done
