#!/bin/bash

CAN_INTERFACE="can0"
BITRATE=125000

loading_animation() {
    spinner="/|\\-"
    duration=1.5
    interval=0.1
    end_time=$(echo "$duration / $interval" | bc)

    echo -n "Laster "

    for ((i=0; i<$end_time; i++)); do
        index=$((i % 4))
        printf "\b${spinner:$index:1}"
        sleep $interval
    done

    printf "\b Ferdig!\n"
}

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

# meny
while true; do
    clear
    echo "===================================="
    echo "=== (can0) CAN-bus Kontrollmeny ==="
    echo "1. Prøv å starte CAN-bus på nytt"
    echo "2. Manuelt: Skru AV can0"
    echo "3. Manuelt: Skru PÅ can0"
    echo "4. Vis CAN-status"
    echo "5. Vis CAN-dump (BUS info)"
    echo "x. Avslutt"
    echo "===================================="
    echo -n "Velg et alternativ [1-5 / x]: "
    echo ""
    echo ""
    read valg

    case $valg in
        1)
            start_canbus
            show_status
            loading_animation
            ;;
        2)
            echo "Manuelt: Skru AV $CAN_INTERFACE ..."
            sudo ip link set $CAN_INTERFACE down
            show_status
            loading_animation
            ;;
        3)
            echo "Manuelt: Skru PÅ $CAN_INTERFACE ..."
            sudo ip link set $CAN_INTERFACE type can bitrate $BITRATE
            sudo ip link set $CAN_INTERFACE up
            show_status
            loading_animation
            ;;
        4)
            show_status
            loading_animation
            ;;
        5)
            echo "Starter CAN-dump... Trykk x + Enter for å avslutte."
            loading_animation
            candump $CAN_INTERFACE &
            CANDUMP_PID=$!

            read -n 1 -s input
            if [[ $input == "x" || $input == "X" ]]; then
                echo "Avslutter candump..."
                kill $CANDUMP_PID
                wait $CANDUMP_PID 2>/dev/null
                echo "Tilbake til meny."
            fi
            ;;
        x)
            echo "Avslutter."
            loading_animation
            exit 0
            ;;
        *)
            echo "Ugyldig valg. Prøv igjen."
            loading_animation
            ;;
    esac
done
