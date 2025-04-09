from pydbus import SystemBus
import time

bus = SystemBus()
adapter = bus.get("org.bluez", "/org/bluez/hci0")

found_devices = {}

def device_found(sender, object, iface, signal, params):
    interface, changed, _ = params
    if interface == "org.bluez.Device1":
        address = object.split("/")[-1].replace("_", ":")
        name = changed.get("Name")
        if name and name not in found_devices.values():
            found_devices[address] = name
            print(f"🔹 Fant enhet: {name} ({address})")

# Lytt etter Bluetooth-hendelser
bus.subscribe(
    iface="org.freedesktop.DBus.Properties",
    signal="PropertiesChanged",
    signal_fired=device_found
)

# Start skanning
print("Starter skanning etter Bluetooth-enheter...")
adapter.StartDiscovery()
time.sleep(10)  # Skann i 10 sekunder
adapter.StopDiscovery()
print("Skanning ferdig.")
