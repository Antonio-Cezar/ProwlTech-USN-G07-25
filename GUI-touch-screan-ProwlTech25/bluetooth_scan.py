import subprocess
import re

def scan_bluetooth_devices(name_filter=""):
    """
    Henter både parrede og kjente enheter (fra bluetoothctl).
    Filtrerer med navn hvis angitt.
    """

    process = subprocess.Popen(
        ['bluetoothctl'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    # Kjører begge kommandoene: devices og paired-devices
    process.stdin.write('devices\n')           # Alle kjente enheter
    process.stdin.write('paired-devices\n')    # Tidligere parrede
    process.stdin.write('quit\n')
    process.stdin.flush()

    output, _ = process.communicate()

    devices = {}
    seen = set()  # For å unngå duplikater

    for line in output.split('\n'):
        match = re.search(r'Device ([0-9A-F:]+) (.+)', line)
        if match:
            address, name = match.groups()

            if name_filter.lower() in name.lower() and address not in seen:
                devices[address] = name
                seen.add(address)

    return devices


