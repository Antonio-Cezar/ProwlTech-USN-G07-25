#!/bin/bash

# (PROWLTECH25 - CA).
# Dette er menyen for å velge de forskjellige opperasjonene for can0 (CAN-Bus kommunikasjon).

# Setter opp CAN-grensesnitt og bitrate.
#================================================================
CAN_INTERFACE="can0"
BITRATE=125000
VESC_ID=10
#================================================================

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

# Funksjonen pauser og venter til bruker trykker x etter at en annen funkjson har blitt kalt på for å vise operasjonen så lenge bruker ønsker 
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

# Funksjon for å starter opp CAN-bussen med spesifisert bitrate når den blir valgt i menyen.
start_canbus() {
    echo "Starter $CAN_INTERFACE med bitrate $BITRATE ..."
    sudo ip link set $CAN_INTERFACE down
    sudo ip link set $CAN_INTERFACE type can bitrate $BITRATE
    sudo ip link set $CAN_INTERFACE up
    sleep 1
}
# Funksjonen viser CAN status når den blir valgt i menyen.
show_status() {
    echo ""
    echo "===================================="
    echo "=== CAN-status ==="
    echo "===================================="
    ip -details link show $CAN_INTERFACE | grep -A 5 "$CAN_INTERFACE" #(-A 5) -> Dette filtrerer ut deler av teksten som vises.
    echo "===================================="
    echo ""
}

# Funksjonen som viser CAN status aktiv og innaktiv på toppen av menyen.
get_can_status() {
    local state=$(ip link show $CAN_INTERFACE | grep -o "state [A-Z]*" | awk '{print $2}') #-o betyr "bare vis det som matcher mønsteret", Det ser etter SATE UP/DOWN og da tar denlinja og viser.
    if [[ $state == "UP" ]]; then
        echo "(can0 status: AKTIV)"
    else
        echo "(can0 status: INAKTIV)"
    fi
}

# === HOVEDMENY ===
while true; do
    clear
    loading_animation
    clear
    # Viser statuslinje for can0.
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
        # Alternativ 1: Start CAN-bus på nytt
        1) 
            clear
            start_canbus
            show_status
            loading_animation
            pause_for_return
            ;;
        # Alternativ 2: Skru av can0 manuelt
        2)
            clear
            echo "Manuelt: Skru AV $CAN_INTERFACE ..."
            sudo ip link set $CAN_INTERFACE down
            show_status
            sleep 0.2
            loading_animation
            ;;
        # Alternativ 3: Skru på can0 manuelt
        3)
            clear
            echo "Manuelt: Skru PÅ $CAN_INTERFACE ..."
            sudo ip link set $CAN_INTERFACE type can bitrate $BITRATE
            sudo ip link set $CAN_INTERFACE up
            show_status
            sleep 0.2
            loading_animation
            ;;
        # Alternativ 4: Vis CAN-status
        4)
            clear
            show_status
            loading_animation
            pause_for_return
            ;;
        # Alternativ 5: Kjør `candump` og vis CAN-trafikk
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
            pause_for_return
            ;;
    esac
done
