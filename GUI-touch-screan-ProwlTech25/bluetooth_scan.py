import subprocess
import time
import re

def scan_bluetooth_devices(scan_time=5):
    # Start bluetoothctl
    process = subprocess.Popen(['bluetoothctl'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    # Aktiver agent og scanning
    process.stdin.write('agent on\n')
    process.stdin.write('scan on\n')
    process.stdin.flush()

    # Vent litt mens den skanner
    time.sleep(scan_time)

    # Slå av scanning
    process.stdin.write('scan off\n')
    process.stdin.write('quit\n')
    process.stdin.flush()

    output, _ = process.communicate()

    # Finn navn og adresser i output
    devices = {}
    for line in output.split('\n'):
        match = re.search(r'Device ([0-9A-F:]+) (.+)', line)
        if match:
            address, name = match.groups()
            devices[address] = name

    return devices

# Test
if __name__ == "__main__":
    print("Skanner etter enheter...")
    found_devices = scan_bluetooth_devices()
    for addr, name in found_devices.items():
        print(f"{name} ({addr})")
