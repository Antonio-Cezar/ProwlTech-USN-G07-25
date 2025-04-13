#!/bin/bash

CAN_INTERFACE="can0"
BITRATE=125000
VESC_ID=10  # Change this to match your VESC's CAN ID

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

send_start() {
    echo "Sender START-kommando til VESC (CAN ID: $VESC_ID)..."
    python3 vesc_cmd.py --can_id $VESC_ID --duty 0.10
}

send_stop() {
    echo "Sender STOP-kommando til VESC (CAN ID: $VESC_ID)..."
    python3 vesc_cmd.py --can_id $VESC_ID --duty 0.00
}

read_temp() {
    echo "Henter temperatur fra VESC (CAN ID: $VESC_ID)..."
    python3 vesc_cmd.py --can_id $VESC_ID --get_temp
    pause_for_return
}

get_motor_status() {
    python3 /home/prowltech/prowltech-script/vesc_cmd.py --can_id 10 --get_status >/dev/null 2>&1
    case $? in
        0)
            echo "(MOTORSTATUS: PÅ)"
            ;;
        3)
            echo "(MOTORSTATUS: AV)"
            ;;
        1)
            echo "(MOTORSTATUS: IKKE TILKOBLET)"
            ;;
        2)
            echo "(MOTORSTATUS: CAN FEIL)"
            ;;
        4)
            echo "(MOTORSTATUS: UKJENT SVAR)"
            ;;
        *)
            echo "(MOTORSTATUS: FEIL)"
            ;;
    esac
}

check_vesc_status() {
    python3 /home/prowltech/prowltech-script/vesc_cmd.py --can_id 10 --check_status >/dev/null 2>&1
    case $? in
        0)
            echo "(VESC status: TILKOBLET)"
            ;;
        1)
            echo "(VESC status: IKKE TILKOBLET)"
            ;;
        2)
            echo "(VESC status: FEIL I CAN-GRENSESNITT)"
            ;;
        *)
            echo "(VESC status: UKJENT)"
            ;;
    esac
}

# meny
while true; do
    clear
    loading_animation
    clear
    echo "===================================="
    echo "$(check_vesc_status)"
    echo "$(get_motor_status)"
    echo "===================================="
    echo ""
    echo ""
    echo "===================================="
    echo "=== (can0) CAN-bus Kontrollmeny ==="
    echo ""
    echo "1. Send PÅ-kommando til VESC"
    echo "2. Send AV-kommando til VESC"
    echo ""
    echo "3. Hent temperatur fra VESC"
    echo ""
    echo "x. Avslutt"
    echo "===================================="
    echo -n "Velg operasjon: "
    read -n 1 valg
    echo

    case $valg in
        1)
            clear
            send_start
            pause_for_return
            ;;
        2)
            clear
            send_stop
            pause_for_return
            ;;
        3)
            clear
            read_temp
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
