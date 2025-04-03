#!/usr/bin/env python3

import os
import subprocess
import sys

def kjør_kommando(kommando):
    try:
        output = subprocess.check_output(kommando, shell=True, text=True)
        return output.strip()
    except subprocess.CalledProcessError:
        return None

def sjekk_spi():
    print("Sjekker om SPI er aktivert...")
    if os.path.exists("/dev/spidev0.0"):
        print("SPI-enhet funnet: /dev/spidev0.0")
        return True
    else:
        print("SPI-enhet ikke funnet. SPI er sannsynligvis ikke aktivert.")
        return False

def sjekk_can_interface():
    print("Sjekker om can0 finnes...")
    output = kjør_kommando("ip link show can0")
    if output:
        print("Fant can0-grensesnitt.")
        return True
    else:
        print("Fant ikke can0. Sjekk at MCP2515 er riktig konfigurert i /boot/firmware/config.txt.")
        return False

def aktiver_can0():
    print("Prøver å aktivere can0 med bitrate 500000...")
    result = kjør_kommando("sudo ip link set can0 up type can bitrate 500000")
    if result is None:
        print("can0 ble aktivert.")
    else:
        print("Klarte ikke å aktivere can0. Det kan være en feil med drivere eller konfigurasjon.")

def vis_can_status():
    print("Sjekker status for can0...")
    output = kjør_kommando("ifconfig can0")
    if output:
        print("can0-status:")
        print(output)
    else:
        print("Fant ikke aktivt can0-grensesnitt.")

def sjekk_dmesg_mcp():
    print("Sjekker systemlogg (dmesg) for MCP2515...")
    output = kjør_kommando("dmesg | grep -i mcp")
    if output:
        print("MCP2515 ble funnet i systemloggen:")
        print(output)
    else:
        print("Ingen MCP2515-logg funnet. Sjekk ledninger, strøm, og /boot/firmware/config.txt.")

def hoved():
    print("=== CAN-oppsett statuskontroll ===\n")
    spi_ok = sjekk_spi()
    can_ok = sjekk_can_interface()

    if can_ok:
        aktiver_can0()
        vis_can_status()
    else:
        print("Kan ikke aktivere can0. Feil må rettes først.\n")

    sjekk_dmesg_mcp()
    print("\nFerdig. Se meldingene over for å finne ut hva som fungerer og hva som mangler.")

if __name__ == "__main__":
    if os.geteuid() != 0:
        print("Dette skriptet må kjøres som root. Prøv: sudo python3 sjekk_can_status.py")
        sys.exit(1)
    hoved()
