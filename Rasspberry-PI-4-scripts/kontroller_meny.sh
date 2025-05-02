#!/bin/bash

# (PROWLTECH25 - CA).
# Dette er menyen for å starte opp kontroller scriptet manuelt

# Setter opp CAN-grensesnitt og bitrate.
#================================================================
CAN_INTERFACE="can0"
BITRATE=125000
#================================================================

xbox_one="python3 v.5_xbox_one_kon_debug.py"

# meny
while true; do
    clear
    clear
    echo "===================================="
    echo "===Kontroller meny==="
    echo "1. Start xbox one kontorlelr sending"
    echo ""
    echo "(Flere tilkoblingsmuligehter kan utvikles)"
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

            # Kjører hvis det finnes og er kjørbart (-x)
            if [[ -x $xbox_one ]]; then
                $xbox_one
            else
                echo "Feil: Fant ikke $xbox_one"
                sleep 0.4
            fi
            ;;
        x)
            echo "Avslutter."
            sleep 0.4
            clear
            exit 0
            ;;
        *)
            clear
            echo "Ugyldig valg. Prøv igjen."
            sleep 0.4
            ;;
    esac
done
