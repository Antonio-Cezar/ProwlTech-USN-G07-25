#!/bin/bash

CAN_INTERFACE="can0"
BITRATE=125000

xbox_one=python v.5_xbox_one_kontroller.py

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

# meny
while true; do
    clear
    loading_animation
    clear
    echo "===================================="
    echo "===Kontroller meny==="
    echo "1. Start xbox one kontorlelr sending"
    echo ""
    echo "(Flere tilkoblingsmuligehter utvikles)"
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
            echo "Xbox one kontroller sending"
            sleep 0.2
            loading_animation
            if [[ -x $xbox_one ]]; then
                $xbox_one
            else
                echo "Feil: Fant ikke $xbox_one"
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
