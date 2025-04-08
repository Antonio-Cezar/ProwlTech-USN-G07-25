import pexpect
import re
import time

def scan_bluetooth_devices(name_filter=""):
    """
    Søker etter Bluetooth-enheter i pairing-modus (synlige enheter).
    Returnerer dict: {adresse: navn}
    """

    devices = {}

    try:
        # Start bluetoothctl
        child = pexpect.spawn("bluetoothctl", echo=False)
        child.expect("#", timeout=5)  # Økt timeout for første respons

        child.sendline("scan on")
        time.sleep(1)  # Gi scanningen tid til å starte

        start_time = time.time()
        scan_duration = 10  # Økt til 10 sekunder

        while time.time() - start_time < scan_duration:
            try:
                # Vent på en linje med enhetsdata
                child.expect(r"Device ([0-9A-F:]{17}) (.+)", timeout=3)
                addr, name = child.match.groups()
                addr = addr.decode()
                name = name.decode()

                if name_filter.lower() in name.lower() or name_filter == "":
                    devices[addr] = name

            except pexpect.exceptions.TIMEOUT:
                # Fortsett å vente på neste enhet
                continue

        # Stopp scanning
        child.sendline("scan off")
        child.sendline("quit")
        child.close()

    except Exception as e:
        print("Bluetooth-feil:", e)

    return devices


