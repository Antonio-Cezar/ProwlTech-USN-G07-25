#!/bin/bash

CAN_INTERFACE="can0"
BITRATE=125000
LOADING_SCRIPT="./loading_animation.sh"

start_canbus() {
    echo "Starter $CAN_INTERFACE med bitrate $BITRATE ..."
    sudo ip link set $CAN_INTERFACE down
    sudo ip link set $CAN_INTERFACE type can bitrate $BITRATE
    sudo ip link set $CAN_INTERFACE up
    sleep 1
}

show_status() {
    echo ""
    echo "=== CAN-status ==="
    ip -details link show $CAN_INTERFACE | grep -A 5 "$CAN_INTERFACE"
    echo ""
}

loading_run(){
    echo ""
    echo ""
    echo ""
    bash "$LOADING_SCRIPT"
    sleep (0.3)
    echo ""
    echo ""
}

# === Start meny uansett tilstand ===
while true; do
    echo ""
    echo ""
    echo ""
    echo ""
    echo ""
    echo "=== CAN-bus Kontrollmeny ==="
    echo "1. Prøv å starte CAN-bus på nytt"
    echo "2. Manuelt: Skru AV can0"
    echo "3. Manuelt: Skru PÅ can0"
    echo "4. Vis CAN-status"
    echo "x. Avslutt"
    echo "============================"
    echo -n "Velg et alternativ [1-4 / x]: "
    echo ""
    echo ""
    read valg
    echo ""

    case $valg in
        1)
            start_canbus
            bash "$LOADING_SCRIPT"
            show_status
            ;;
        2)
            echo "Manuelt: Skru AV $CAN_INTERFACE ..."
            sudo ip link set $CAN_INTERFACE down
            loading_run
            show_status
            ;;
        3)
            echo "Manuelt: Skru PÅ $CAN_INTERFACE ..."
            sudo ip link set $CAN_INTERFACE type can bitrate $BITRATE
            sudo ip link set $CAN_INTERFACE up
            loading_run
            show_status
            ;;
        4)
            show_status
            loading_run
            ;;
        x)
            echo "Avslutter."
            loading_run
            exit 0
            ;;
        *)
            echo "Ugyldig valg. Prøv igjen."
            ;;
    esac
done
