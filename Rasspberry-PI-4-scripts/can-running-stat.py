#!/usr/bin/env python3

import os
import subprocess
import sys

# Funksjon for å kjøre en terminalkommando og hente ut resultatet
def kjør_kommando(kommando):
    try:
        output = subprocess.check_output(kommando, shell=True, text=True)
        return output.strip()
    except subprocess.CalledProcessError:
        return None

# Sjekker om SPI-enheten er aktiv og tilgjengelig
def sjekk_spi():
    print("Sjekker om SPI er aktivert...")
    if os.path.exists("/dev/spidev0.0"):
        print("SPI-enhet funnet: /dev/spidev0.0")
        return True
    else:
        print("SPI-enhet ikke funnet. SPI er sannsynligvis ikke aktivert.")
        return False

# Sjekker om nettverksgrensesnittet 'can0' finnes
def sjekk_can_interface():
    print("Sjekker om 'can0'-grensesnittet eksisterer...")
    output = kjør_kommando("ip link show can0")
    if output:
        print("Fant 'can0'-grensesnitt.")
        return True
    else:
        print("Fant ikke 'can0'.")
        print("Tips: Sjekk at MCP2515 er riktig konfigurert i /boot/firmware/config.txt.")
        return False

# Prøver å aktivere CAN-grensesnittet med en standard bitrate
def aktiver_can0():
    print("Prøver å aktivere 'can0' med bitrate 500000...")
    result = kjør_kommando("sudo ip link set can0 up type can bitrate 500000")
    if result is None:
        print("'can0' ble aktivert.")
    else:
        print("Klarte ikke å aktivere 'can0'.")
        print("Tips: Det kan være en feil med drivere, maskinvare eller konfigurasjon.")

# Viser nåværende status for CAN-grensesnittet
def vis_can_status():
    print("Henter status for 'can0'...")
    output = kjør_kommando("ifconfig can0")
    if output:
        print("'can0'-status:\n")
        print(output)
    else:
        print("Fant ikke aktivt 'can0'-grensesnitt.")

# Søker i systemloggen etter MCP2515-relaterte meldinger
def sjekk_dmesg_mcp():
    print("Søker i systemloggen etter MCP2515-meldinger...")
    output = kjør_kommando("dmesg | grep -i mcp")
    if output:
        print("MCP2515 ble funnet i systemloggen:")
        print(output)
    else:
        print("Ingen MCP2515-meldinger funnet.")
        print("Tips: Sjekk ledninger, strømtilførsel og /boot/firmware/config.txt.")

# Hovedfunksjonen som styrer hele sjekken
def hoved():
    print("\n=== CAN-oppsett – Statuskontroll ===\n")
    spi_ok = sjekk_spi()
    can_ok = sjekk_can_interface()

    if can_ok:
        aktiver_can0()
        vis_can_status()
    else:
        print("Kan ikke aktivere 'can0' fordi grensesnittet mangler.")
        print("Rydd opp i feilene først.\n")

    sjekk_dmesg_mcp()
    print("\nFerdig.")
    print("Se gjennom meldingene over for å finne ut hva som fungerer – og hva som eventuelt må rettes.")

# Sørg for at skriptet kjøres som root
if __name__ == "__main__":
    if os.geteuid() != 0:
        print("Dette skriptet må kjøres som root (administrator).")
        print("Prøv å kjøre: sudo python3 sjekk_can_status.py")
        sys.exit(1)
    hoved()
