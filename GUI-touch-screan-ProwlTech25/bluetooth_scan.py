import subprocess   # Kunne kjøre kommandoer i terminalen
import time     # Brukes til å vente
import re       # Kunne finne mønstre i tekst

def scan_bluetooth_devices(scan_time=5, name_filter=""):      # Søk etter enheter med navn som matcher filter

    # Start bluetoothctl
    process = subprocess.Popen(
        ['bluetoothctl'], 
        stdin=subprocess.PIPE,  # Sende kommandoer
        stdout=subprocess.PIPE, # Lese ut data
        stderr=subprocess.PIPE, 
        text=True       # Får tekst istedenfor bytes
    )

    # Aktiver agent og scanning
    process.stdin.write('agent on\n')
    process.stdin.write('scan on\n')
    process.stdin.flush()   # Sender kommandoer

    # Vent litt mens den skanner
    time.sleep(scan_time)

    # Slå av scanning
    process.stdin.write('scan off\n')
    process.stdin.write('quit\n')
    process.stdin.flush()

    output, _ = process.communicate()   # Laser output fra prosessen

    devices = {}    # Tom liste for enheter

    # Går gjennom hver linje og ser etter linjer som matcher: Device <MAC> <Navn>
    for line in output.split('\n'):
        match = re.search(r'Device ([0-9A-F:]+) (.+)', line)
        if match:
            address, name = match.groups()

            # Filter
            if name_filter.lower() in name.lower():
                devices[address] = name     # Legg til i resultater

    return devices

# Test
if __name__ == "__main__":
    print("Skanner etter enheter...")
    found_devices = scan_bluetooth_devices()
    for addr, name in found_devices.items():
        print(f"{name} ({addr})")
