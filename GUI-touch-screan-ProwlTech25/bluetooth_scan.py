import subprocess
import re
import time

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

    process.stdin.write('scan on\n')
    process.stdin.flush()

    time.sleep(5)

    process.stdin.write('scan off\n')
    process.stdin.write('quit\n')
    process.stdin.flush()

    output, _ = process.communicate()

    devices = {}

    for line in output.split('\n'):
        match = re.search(r'Device ([0-9A-F:]+) (.+)', line)
        if match:
            address, name = match.groups()

            if name_filter.lower() in name.lower():
                devices[address] = name


    return devices


