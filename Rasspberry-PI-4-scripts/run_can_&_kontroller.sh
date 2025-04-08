#!/bin/bash

# Filer som skal kjøres
CAN_SCRIPT="can-running.stat.py"
XBOX_SCRIPT="v.5_xbox_one_kontroller.py"
LOADING_SCRIPT="./loading_animation.sh"  # Sørg for at denne peker til riktig sti

# Funksjon for å sjekke at en fil finnes
sjekk_fil() {
    if [ ! -f "$1" ]; then
        echo "Feil: Fant ikke filen '$1'"
        exit 1
    fi
}

# Sjekk at Python er tilgjengelig
if ! command -v python3 &> /dev/null; then
    echo "Python3 er ikke installert eller ikke i PATH."
    exit 1
fi

# Sjekk at alle nødvendige filer eksisterer
sjekk_fil "$CAN_SCRIPT"
sjekk_fil "$XBOX_SCRIPT"
sjekk_fil "$LOADING_SCRIPT"

# Start første skript
echo "Starter CAN-status: $CAN_SCRIPT"
python3 "$CAN_SCRIPT"
echo "CAN-status fullført."

# Kjør ekstern loading-animasjon
bash "$LOADING_SCRIPT"

# Start neste skript
echo "Starter Xbox-kontroller: $XBOX_SCRIPT"
python3 "$XBOX_SCRIPT"
echo "Xbox-kontroller avsluttet."
