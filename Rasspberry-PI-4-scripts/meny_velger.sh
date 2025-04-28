#!/bin/bash

# (PROWLTECH25 - CA).
# Dette er menyen for å velge de forskjellige menyen for can0, motorkontroller og kontroller operasjoner.

# Setter opp CAN-grensesnitt og bitrate.
#================================================================
CAN_INTERFACE="can0"
BITRATE=125000
$VESC_ID=10
#================================================================

# Sti til undermenyer.
canbus_kontrollmeny=./canbus_meny.sh
motorkontroller_kontrollmeny=./motorkontroller_meny.sh
kontroller_meny=./kontroller_meny.sh

# Funksjonen viser en enkel "laste-animasjon" med roterende tegn.
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

# Funksjonen som viser CAN status aktiv og innaktiv på toppen av menyen.
get_can_status() {
    local state=$(ip link show $CAN_INTERFACE | grep -o "state [A-Z]*" | awk '{print $2}')
    if [[ $state == "UP" ]]; then
        echo "(can0 status: AKTIV)"
    else
        echo "(can0 status: INAKTIV)"
    fi
}

# === HOVEDMENYEN ===
while true; do
    clear
    loading_animation
    clear
    # Viser statuslinje for can0, VESC og motor.
    echo "===================================="
    echo "$(get_can_status)"
    echo "===================================="
    echo ""
    echo ""
    echo "===================================="
    echo "===Velg Kontrollmeny ==="
    echo "1. CAN-bus kontrollmeny"
    echo ""
    echo "2. Xbox Kontroller meny"
    echo ""
    echo "x. Avslutt"
    echo "===================================="
    echo -n "Velg operasjon: "
    read -n 1 valg
    echo

    case $valg in
        # Alternativ 1: Åpner CAN-bus kontrollmeny.
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
        # Alternativ 2: Åpner kontroller-meny
        2)
            clear
            echo "Åpner opp: "
            echo "kontroller meny"
            sleep 0.2
            loading_animation
            if [[ -x $kontroller_meny ]]; then
                $kontroller_meny
            else
                echo "Feil: Fant ikke $kontroller_meny"
                sleep 0.4
                loading_animation
            fi
            ;;
        # Alternativ x: Avslutt programmet
        x)
            echo "Avslutter."
            loading_animation
            clear
            exit 0
            ;;
        # Default: Ugyldig valg
        *)
            clear
            echo "Ugyldig valg. Prøv igjen."
            loading_animation
            ;;
    esac
done
