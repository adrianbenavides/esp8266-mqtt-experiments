#!/usr/bin/env bash

dirs=(
    "mqtt-0-config-devices"
    "mqtt-1-two-devices"
    "mqtt-2-last-will"
)

for dir in ${dirs[@]}
do
    cp -arf "./arduino-helpers/lib" "./${dir}/"
    cp -arf "./arduino-helpers/src" "./${dir}/"
    cp -arf "./arduino-helpers/include" "./${dir}/"
    cp -arf "./arduino-helpers/platformio.ini" "./${dir}/"
    cp -arf "./arduino-helpers/.editorconfig" "./${dir}/"
done
