import pexpect
import time

def live_scan(timeout=10):
    devices = {}

    # Start bluetoothctl som en interaktiv sesjon
    child = pexpect.spawn("bluetoothctl", encoding="utf-8", timeout=5)
    child.expect("#")  # Vent til bluetoothctl er klar

    child.sendline("agent on")
    child.expect("#")

    child.sendline("scan on")

    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            # Fang linjer som viser nye enheter
            child.expect(r"Device ([0-9A-F:]{17}) (.+)", timeout=2)
            mac, name = child.match.groups()
            label = f"{name} ({mac})"
            if label not in devices:
                devices[label] = mac
        except pexpect.exceptions.TIMEOUT:
            continue  # Fortsett å vente hvis ingen ny linje

    child.sendline("scan off")
    child.sendline("exit")
    return devices

# Testskanning
if __name__ == "__main__":
    print("Skanner etter Bluetooth-enheter...\n")
    found_devices = live_scan(10)
    if found_devices:
        print("Enheter funnet:")
        for label, mac in found_devices.items():
            print(f"  - {label}")
    else:
        print("Fant ingen enheter.")
