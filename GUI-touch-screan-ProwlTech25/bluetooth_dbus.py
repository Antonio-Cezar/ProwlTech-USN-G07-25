from pydbus import SystemBus
import time

# Koble til systembussen og BlueZ-adapteren
bus = SystemBus()
adapter = bus.get("org.bluez", "/org/bluez/hci0")

# Lagrer funnede enheter
found_devices = {}

# Kalles når et nytt Bluetooth-signal dukker opp
def device_found(sender, object, iface, signal, params):
    print("Mottok signal fra D-Bus")
    print(f"   ↳ object path: {object}")
    interface, changed, _ = params
    print(f"   ↳ Interface: {interface}")
    print(f"   ↳ Changed: {changed}")

    # Hvis signalet gjelder en enhet
    if interface == "org.bluez.Device1":
        address = object.split("/")[-1].replace("_", ":")
        name = changed.get("Name", "(ukjent navn)")
        if address not in found_devices:
            found_devices[address] = name
            print(f"🔹 Ny enhet: {name} ({address})")

# Abonner på signaler fra BlueZ
bus.subscribe(
    iface="org.freedesktop.DBus.Properties",
    signal="PropertiesChanged",
    signal_fired=device_found
)

# Start scanning
print("🔍 Starter Bluetooth-skanning med pydbus...")
try:
    adapter.StartDiscovery()
except Exception as e:
    print(f"Klarte ikke starte skanning: {e}")
    exit(1)

# Vent og logg
try:
    time.sleep(30)
finally:
    adapter.StopDiscovery()
    print("Skanning avsluttet.\n")

# Skriv ut resultat
if found_devices:
    print("Enheter funnet:")
    for addr, name in found_devices.items():
        print(f" - {name} ({addr})")
else:
    print("Fant ingen enheter.")

