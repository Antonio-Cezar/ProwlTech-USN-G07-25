import pexpect
import re
import time

def scan_bluetooth_devices(name_filter=""):
    """
    Søker etter Bluetooth-enheter i nærheten som er synlige (typisk i pairing-modus).
    Returnerer en dictionary med {adresse: navn}.
    """

    devices = {}

    try:
        # Start bluetoothctl i interaktivt shell
        child = pexpect.spawn("bluetoothctl", echo=False)
        child.expect("#", timeout=2)

        # Start scanning etter enheter
        child.sendline("scan on")
        scan_duration = 5  # sekunder
        end_time = time.time() + scan_duration

        while time.time() < end_time:
            try:
                # Matcher linjer som: Device XX:XX:XX:XX:XX:XX ControllerName
                child.expect(r'Device ([0-9A-F:]{17}) (.+)', timeout=1)
                addr, name = child.match.groups()
                addr = addr.decode()
                name = name.decode()

                # Hvis navnefilter er satt, sjekk det
                if name_filter.lower() in name.lower():
                    devices[addr] = name
                elif name_filter == "":
                    devices[addr] = name

            except pexpect.exceptions.TIMEOUT:
                continue  # Ingen enhet funnet akkurat nå, prøv videre

        # Avslutt scanning og bluetoothctl
        child.sendline("scan off")
        child.sendline("quit")
        child.close()

    except Exception as e:
        print("Feil under Bluetooth-søk:", e)

    return devices



