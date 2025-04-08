def scan_bluetooth_devices(name_filter=""):
    """
    Returnerer tidligere parrede Bluetooth-enheter som matcher et valgfritt navn-filter.
    """
    import subprocess
    import re

    process = subprocess.Popen(
        ['bluetoothctl'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    process.stdin.write('paired-devices\n')
    process.stdin.write('quit\n')
    process.stdin.flush()

    output, _ = process.communicate()

    devices = {}
    for line in output.split('\n'):
        match = re.search(r'Device ([0-9A-F:]+) (.+)', line)
        if match:
            address, name = match.groups()
            if name_filter.lower() in name.lower():  # tomt filter = vis alt
                devices[address] = name

    return devices

