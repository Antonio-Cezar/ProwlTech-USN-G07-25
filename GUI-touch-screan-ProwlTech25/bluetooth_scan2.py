import pexpect
import time

def live_scan(timeout=10):
    devices = {}

    # Start bluetoothctl som interaktiv sesjon
    child = pexpect.spawn("bluetoothctl", encoding="utf-8", timeout=5)
    child.expect("#")  # Vent til klar prompt

    child.sendline("agent on")
    child.expect("#")

    child.sendline("scan on")

    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            # Fang bare nye enheter mens scanning er aktiv
            child.expect(r"Device ([0-9A-F:]{17}) (.+)", timeout=2)
            mac, name = child.match.groups()

            # Hvis enheten er ny (basert på MAC), legg den til
            if mac not in devices:
                devices[mac] = name
        except pexpect.exceptions.TIMEOUT:
            continue  # Hvis ingen ny linje – prøv igjen

    child.sendline("scan off")
    child.sendline("exit")
    return devices

# Testskanning
if __name__ == "__main__":
    print("🔍 Skanner etter Bluetooth-enheter...\n")
    found_devices = live_scan(10)
    if found_devices:
        print("🎮 Enheter funnet:")
        for addr, name in found_devices.items():
            print(f"  - {name} ({addr})")
    else:
        print("🚫 Fant ingen enheter.")
