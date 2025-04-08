import subprocess
import re

def scan_bluetooth_devices(name_filter="Controller"):
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
    process.stdin.write('devices\n')
    process.stdin.write('quit\n')
    process.stdin.flush()

    output, _ = process.communicate()

    devices = {}
    seen = set()  # For å unngå duplikater
    addresses = []

    for line in output.split('\n'):
        match = re.search(r'Device ([0-9A-F:]+) (.+)', line)
        if match:
            address, name = match.groups()

            if name_filter.lower() in name.lower() and address not in seen:
                addresses.append((address, name))
                seen.add(address)

    # Filtrer kun tilgjengelige enheter
    for addr, name in addresses:
        info = subprocess.run(
            ['bluetoothctl', 'info', addr],
            capture_output=True,
            text=True
        ).stdout

        if "Connected: yes" in info or "Connected: no" in info:
            devices[addr] = name

    return devices


