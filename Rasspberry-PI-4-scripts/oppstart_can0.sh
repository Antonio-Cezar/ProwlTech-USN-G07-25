#!/bin/bash
CAN_INTERFACE="can0"
BITRATE=125000

ip link set $CAN_INTERFACE down
ip link set $CAN_INTERFACE type can bitrate $BITRATE
ip link set $CAN_INTERFACE up
