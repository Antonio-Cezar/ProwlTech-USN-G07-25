#!/bin/bash

CAN_INTERFACE="can0"
BITRATE=125000

loading_animation() {
    spinner="/|\\-"
    duration=0.6
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
    echo "===================================="
    echo "=== CAN-status ==="
    echo "===================================="
    ip -details link show $CAN_INTERFACE | grep -A 5 "$CAN_INTERFACE"
    echo "===================================="
    echo ""
}

get_can_status() {
    local state=$(ip link show $CAN_INTERFACE | grep -o "state [A-Z]*" | awk '{print $2}')
    if [[ $state == "UP" ]]; then
        echo "(can0 status: AKTIV)"
    else
        echo "(can0 status: INAKTIV)"
    fi
}

pause_for_return() {
    echo ""
    echo "Trykk x for å gå tilbake til menyen..."
    while true; do
        read -n 1 -s input
        if [[ $input == "x" || $input == "X" ]]; then
            break
        fi
    done
}

# meny
while true; do
    clear
    loading_animation
    clear
    echo "===================================="
    echo "$(get_can_status)"
    echo "===================================="
    echo "=== (can0) CAN-bus Kontrollmeny ==="
    echo "1. Prøv å starte CAN-bus på nytt"
    echo ""
    echo "2. Manuelt: Skru AV can0"
    echo "3. Manuelt: Skru PÅ can0"
    echo ""
    echo "4. Vis CAN-status"
    echo "5. Vis CAN-dump (BUS info)"
    echo ""
    echo ""
    echo "x. Avslutt"
    echo "===================================="
    echo -n "Velg operasjon: "
    read -n 1 valg
    echo

    case $valg in
        1)
            clear
            start_canbus
            show_status
            loading_animation
            pause_for_return
            ;;
        2)
            clear
            echo "Manuelt: Skru AV $CAN_INTERFACE ..."
            sudo ip link set $CAN_INTERFACE down
            show_status
            sleep 0.2
            loading_animation
            ;;
        3)
            clear
            echo "Manuelt: Skru PÅ $CAN_INTERFACE ..."
            sudo ip link set $CAN_INTERFACE type can bitrate $BITRATE
            sudo ip link set $CAN_INTERFACE up
            show_status
            sleep 0.2
            loading_animation
            ;;
        4)
            clear
            show_status
            loading_animation
            pause_for_return
            ;;
        5)
            clear
            echo "Starter CAN-dump... Trykk x for å avslutte."
            loading_animation
            candump $CAN_INTERFACE &
            CANDUMP_PID=$!

            while true; do
                read -n 1 -s input
                if [[ $input == "x" || $input == "X" ]]; then
                    echo "Avslutter candump..."
                    kill $CANDUMP_PID
                    wait $CANDUMP_PID 2>/dev/null
                    break
                fi
            done
            ;;
        x)
            echo "Avslutter."
            loading_animation
            clear
            exit 0
            ;;
        *)
            clear
            echo "Ugyldig valg. Prøv igjen."
            loading_animation
            pause_for_return
            ;;
    esac
done
