from pydbus import SystemBus
import time
import subprocess

# Koble til systembus
bus = SystemBus()
mngr = bus.get("org.bluez", "/")
adapter = bus.get("org.bluez", "/org/bluez/hci0")

found_devices = {}

# Start Bluetooth-skanning via BlueZ
print("Starter søket...")
try:
    adapter.StartDiscovery()
    print("Søker...")
    time.sleep(10)
    adapter.StopDiscovery()
except Exception as e:
    print(f"Feil ved skanning: {e}")

print("Skanning avsluttet.\n")

# Fase 1 – Hent enheter via D-Bus ObjectManager
print("Enheter funnet via D-Bus:")
managed_objects = mngr.GetManagedObjects()

for path, interfaces in managed_objects.items():
    if "org.bluez.Device1" in interfaces:
        dev = interfaces["org.bluez.Device1"]
        address = dev.get("Address", "ukjent")
        name = dev.get("Name")
        if not name:
            continue
        found_devices[address] = name
        print(f"🔹 {name} ({address})")

def get_devices():
    return found_devices

def get_device(name):
    for addr, dev_name in found_devices.items():
        if dev_name == name:
            return addr
    return None

'''

# Fase 2 – Fallback: bluetoothctl devices
print("\n🔧 Fallback-sjekk via bluetoothctl:")
output = subprocess.run(["bluetoothctl", "devices"], capture_output=True, text=True)
for line in output.stdout.strip().split("\n"):
    if line.startswith("Device"):
        _, addr, name = line.split(" ", 2)
        if addr not in found_devices:
            found_devices[addr] = name
            print(f"🔸 {name} ({addr})")

# Resultat
print("\n🎯 Totalt funnet enheter:")
if found_devices:
    for addr, name in found_devices.items():
        print(f" - {name} ({addr})")
else:
    print("🚫 Ingen enheter funnet.")
'''
