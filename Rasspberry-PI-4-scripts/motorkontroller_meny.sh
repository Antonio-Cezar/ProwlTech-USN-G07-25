#!/bin/bash

# (PROWLTECH25 - CA).
# Dette er menyen for å velge de forskjellige menyen for motorkontroller operasjoner.

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

# Funksjonen pauser og venter til bruker trykker x etter at en annen funkjson har blitt kalt på for å vise operasjonen så lenge bruker ønsker.
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

# Funksjonen som sender kommando til VESC for å starte/slå på motoren motorenkontrollerne.
send_start() {
    echo "Sender START-kommando til VESC (CAN ID: $VESC_ID)..."
    python3 vesc_cmd.py --can_id $VESC_ID --duty 0.10
}

# Funksjonen som sender kommando for å stoppe/slå av motorenkontrollerne.
send_stop() {
    echo "Sender STOP-kommando til VESC (CAN ID: $VESC_ID)..."
    python3 vesc_cmd.py --can_id $VESC_ID --duty 0.00
}

# Leser av temperatur fra VESC og viser det til brukeren
read_temp() {
    echo "Henter temperatur fra VESC (CAN ID: $VESC_ID)..."
    python3 vesc_cmd.py --can_id $VESC_ID --get_temp
    pause_for_return
}

# Funksjonen bruker Python-skript for å hente status på motorkontrolleren (via CAN)
get_motor_status() {
    python3 /home/prowltech/prowltech-script/vesc_cmd.py --can_id $VESC_ID --get_status >/dev/null 2>&1
    case $? in
        0)
            echo "(MOTORSTATUS: PÅ)";;
        3)
            echo "(MOTORSTATUS: AV)";;
        1)
            echo "(MOTORSTATUS: IKKE TILKOBLET)";;
        2)
            echo "(MOTORSTATUS: CAN FEIL)";;
        4)
            echo "(MOTORSTATUS: UKJENT SVAR)";;
        *)
            echo "(MOTORSTATUS: FEIL)";;
    esac
}

# Funksjonen sjekker om VESC (motorstyring komponenten) er koblet til.
check_vesc_status() {
    python3 /home/prowltech/prowltech-script/vesc_cmd.py --can_id $VESC_ID --check_status >/dev/null 2>&1
    case $? in
        0)
            echo "(VESC status: TILKOBLET)";;
        1)
            echo "(VESC status: IKKE TILKOBLET)";;
        2)
            echo "(VESC status: FEIL I CAN-GRENSESNITT)";;
        *)
            echo "(VESC status: UKJENT)";;
    esac
}

# === HOVEDMENYEN ===
while true; do
    clear
    loading_animation
    clear
     # Viser statuslinje for VESC og motor.
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
        # Alternativ 1: Sender PÅ-kommando til VESC.
        1)
            clear
            send_start
            pause_for_return
            ;;
        # Alternativ 2: Sender AV-kommando til VESC.
        2)
            clear
            send_stop
            pause_for_return
            ;;
        # Alternativ 3: Henter temperatur fra VESC.
        3)
            clear
            read_temp
            ;;
        # Alternativ x: Avslutt programmet.
        x)
            echo "Avslutter."
            loading_animation
            clear
            exit 0
            ;;
        # Default: Ugyldig valg.
        *)
            clear
            echo "Ugyldig valg. Prøv igjen."
            loading_animation
            pause_for_return
            ;;
    esac
done
