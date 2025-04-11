#!/bin/bash

CAN_INTERFACE="can0"
BITRATE=125000

canbus_kontrollmeny=./canbus_meny.sh
motorkontroller_kontrollmeny=./motorkontroller_kontrollmeny.sh

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

get_can_status() {
    local state=$(ip link show $CAN_INTERFACE | grep -o "state [A-Z]*" | awk '{print $2}')
    if [[ $state == "UP" ]]; then
        echo "(can0 status: AKTIV)"
    else
        echo "(can0 status: INAKTIV)"
    fi
}

# meny
while true; do
    clear
    loading_animation
    clear
    echo "===================================="
    echo "$(get_can_status)"
    echo ""
    echo "MOTORKONTROLLER STATUS (KOMMER)"
    echo "===================================="
    echo "===Velg Kontrollmeny ==="
    echo "1. CAN-bus kontrollmeny"
    echo ""
    echo "2. Motorkontrollere kontrollmeny"
    echo ""
    echo "x. Avslutt"
    echo "===================================="
    echo -n "Velg operasjon: "
    read -n 1 valg
    echo

    case $valg in
        1)
            clear
            echo "Åpner opp: "
            echo "CAN-bus kontrollmeny"
            sleep 0.2
            loading_animation
            if [[ -x $canbus_kontrollmeny ]]; then
                $canbus_kontrollmeny
            else
                echo "Feil: Fant ikke $canbus_kontrollmeny"
                sleep 0.4
                loading_animation
            fi
            ;;
        2)
            clear
            echo "Åpner opp: "
            echo "Motorkontrollere kontrollmeny"
            sleep 0.2
            loading_animation
            if [[ -x $motorkontroller_kontrollmeny ]]; then
                $motorkontroller_kontrollmeny
            else
                echo "Feil: Fant ikke $motorkontroller_kontrollmeny"
                sleep 0.4
                loading_animation
            fi
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
            ;;
    esac
done
