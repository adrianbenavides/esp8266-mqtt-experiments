#!/usr/bin/env bash

dirs=(
    "mqtt-0-config-devices"
    "mqtt-1-two-devices"
)

for dir in ${dirs[@]}
do
    cp -arf "./shared/lib" "./${dir}/"
    cp -arf "./shared/src" "./${dir}/"
    cp -arf "./shared/include" "./${dir}/"
    cp -arf "./shared/platformio.ini" "./${dir}/"
    cp -arf "./shared/.editorconfig" "./${dir}/"
done
