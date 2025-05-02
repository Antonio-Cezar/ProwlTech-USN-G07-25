#!/bin/bash

# (PROWLTECH25 - CA).
# Dette er menyen for å velge de forskjellige menyen for can0, motorkontroller og kontroller operasjoner.

# Setter opp CAN-grensesnitt og bitrate.
#================================================================
CAN_INTERFACE="can0"
BITRATE=125000
#================================================================

# Sti til undermenyer.
canbus_kontrollmeny=./canbus_meny.sh
motorkontroller_kontrollmeny=./motorkontroller_meny.sh
kontroller_meny=./kontroller_meny.sh

# Funksjonen som viser CAN status aktiv og innaktiv på toppen av menyen.
get_can_status() {
    # grep -o = "only matching". Denne skriver bare ut selve matchen og ikke hele linjen
    # awk '{print $2}' henter ord nummer 2 = selve statusen ("UP", "DOWN", osv)
    local state=$(ip link show $CAN_INTERFACE | grep -o "state [A-Z]*" | awk '{print $2}')

    # Sjekk om grensesnittet er aktivt
    if [[ $state == "UP" ]]; then
        echo "(can0 status: AKTIV)"
    else
        echo "(can0 status: INAKTIV)"
    fi
}

# === HOVEDMENYEN ===
while true; do
    clear
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

            # Kjører hvis det finnes og er kjørbart (-x)
            if [[ -x $canbus_kontrollmeny ]]; then
                $canbus_kontrollmeny
            else
                echo "Feil: Fant ikke $canbus_kontrollmeny"
                sleep 0.4
            fi
            ;;
        # Alternativ 2: Åpner kontroller-meny
        2)
            clear
            echo "Åpner opp: "
            echo "kontroller meny"
            sleep 0.2
            if [[ -x $kontroller_meny ]]; then
                $kontroller_meny
            else
                echo "Feil: Fant ikke $kontroller_meny"
                sleep 0.4
            fi
            ;;
        # Alternativ x: Avslutt programmet
        x)
            echo "Avslutter."
            sleep 0.4
            clear
            exit 0
            ;;
        # Default: Ugyldig valg
        # Håndterer alle andre input
        *) 
            clear
            echo "Ugyldig valg. Prøv igjen."
            sleep 0.4 
            ;;
    esac
done
