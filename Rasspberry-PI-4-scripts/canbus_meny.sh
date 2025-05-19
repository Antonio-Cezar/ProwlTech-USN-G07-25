#!/bin/bash
# (PROWLTECH25 - CA).
# Dette er menyen for å velge de forskjellige opperasjonene for can0 (CAN-Bus kommunikasjon).
# Setter opp CAN-grensesnitt og bitrate.
#================================================================
CAN_INTERFACE="can0"
BITRATE=125000
#================================================================
# Funksjonen pauser og venter til bruker trykker x etter at en annen funkjson har blitt kalt på for å vise operasjonen så lenge bruker ønsker 
pause_for_return(){
    echo ""
    echo "Trykk x for å gå tilbake til menyen..."
    while true; do
        read -n 1 -s input
        if [[ $input == "x" || $input == "X" ]]; then
            break
        fi
    done
}
# Funksjon for å starte opp CAN-bussen med spesifisert bitrate når den blir valgt i menyen.
start_canbus(){
    echo "Starter $CAN_INTERFACE med bitrate $BITRATE ..."
    sudo ip link set "$CAN_INTERFACE" down
    sudo ip link set "$CAN_INTERFACE" type can bitrate "$BITRATE"
    sudo ip link set "$CAN_INTERFACE" up
    sleep 1
}
# Funksjonen viser CAN status når den blir valgt i menyen.
show_status(){
    echo ""
    echo "===================================="
    echo "=== CAN-status ==="
    echo "===================================="
    ip -details link show "$CAN_INTERFACE" | grep -A 5 "$CAN_INTERFACE" 
    echo "===================================="
    echo ""
}
# Funksjonen som viser CAN status aktiv og innaktiv på toppen av menyen.
get_can_status(){
    #-o betyr "vis det er lik mønsteret", Den ser etter STATE UP/DOWN og da tar den linja og viser.
    # `awk '{print $2}'`(andre kolonne = "UP" eller "DOWN")
    local state=$(ip link show "$CAN_INTERFACE" | grep -o "state [A-Z]*" | awk '{print $2}') 

    if [[ $state == "UP" ]]; then
        echo "(can0 status: AKTIV)"
    else
        echo "(can0 status: INAKTIV)"
    fi
}
# === HOVEDMENY ===
while true; do
    clear
    echo "===================================="
    echo "$(get_can_status)"
    echo "===================================="
    echo ""
    echo ""
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
            pause_for_return
            ;;
        2)
            clear
            echo "Manuelt: Skru AV $CAN_INTERFACE ..."
            sudo ip link set "$CAN_INTERFACE" down
            show_status
            sleep 0.2
            ;;
        3)
            clear
            echo "Manuelt: Skru PÅ $CAN_INTERFACE ..."
            sudo ip link set "$CAN_INTERFACE" type can bitrate "$BITRATE"
            sudo ip link set "$CAN_INTERFACE" up
            show_status
            sleep 0.2
            ;;
        4)
            clear
            show_status
            sleep 0.2
            pause_for_return
            ;;
        5)
            clear
            echo "Starter CAN-dump... Trykk x for å avslutte."
            sleep 0.2
            candump "$CAN_INTERFACE" &
            CANDUMP_PID=$!
            while true; do
                read -n 1 -s input
                if [[ $input == "x" || $input == "X" ]]; then
                    echo "Avslutter candump..."
                    kill "$CANDUMP_PID"
                    wait "$CANDUMP_PID" 2>/dev/null
                    break
                fi
            done
            ;;
        x)
            echo "Avslutter."
            sleep 0.2
            clear
            exit 0
            ;;
        *)
            clear
            echo "Ugyldig valg. Prøv igjen."
            sleep 0.2
            pause_for_return
            ;;
    esac
done
