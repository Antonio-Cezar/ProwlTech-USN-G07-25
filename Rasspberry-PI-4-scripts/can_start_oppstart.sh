#!/bin/bash

CAN_INTERFACE="can0"
BITRATE=125000

start_canbus() {
    echo "Starter $CAN_INTERFACE med bitrate $BITRATE ..."
    sudo ip link set $CAN_INTERFACE down
    sudo ip link set $CAN_INTERFACE type can bitrate $BITRATE
    sudo ip link set $CAN_INTERFACE up
    sleep 1
}

# Første oppstart
start_canbus
show_status