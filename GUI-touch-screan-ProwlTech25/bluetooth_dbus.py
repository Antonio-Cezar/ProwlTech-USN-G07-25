import platform
import time

# Mock-versjon for å teste på Windows
if platform.system() != "Linux":
    print("Kjører i Windows")

    # Falsk data
    found_devices = {"00:11:22:33:44:55": "TestController A", "66:77:88:99:AA:BB": "TestController B"}

    def scan_devices():
        print("Mock: Simulerer skanning...")
        time.sleep(2)   # Tid til å vise progressbaren 

    def get_devices():
        return found_devices
    
    def get_device(name):
        for addr, dev_name in found_devices.items():
            if dev_name == name:
                return addr
        return None
    
    def connect_to_device(name):
        print(f"Mock: Koblet til {name}")
        return True
    
    def disconnect_from_device(name):
        print(f"Mock: Koblet fra {name}")
        return True

    
# Ekte Linux versjon
else:
    from pydbus import SystemBus
    import time
    import subprocess

    found_devices = {}

    def scan_devices():
        global found_devices
        found_devices.clear()

        # Koble til systembus
        bus = SystemBus()
        mngr = bus.get("org.bluez", "/")
        adapter = bus.get("org.bluez", "/org/bluez/hci0")



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

    def connect_to_device(name):
        address = get_device(name)
        if not address:
            print(f"Fant ikke enhet med navn {name}")
            return False
        
        bus = SystemBus()
        device_path = f"/org/bluez/hci0/dev_{address.replace(':', '_')}"
        try:
            device = bus.get("org.bluez", device_path)
            device.Connect()
            print(f"Koblet til {name} ({address})")
            return True
        except Exception as e:
            print(f"Feil ved tilkobling til {name}: {e}")
            return False
        
    def disconnect_from_device(name):
        address = get_device(name)
        if not address:
            print(f"Fant ikke enhet med navn {name}")
            return False

        bus = SystemBus()
        device_path = f"/org/bluez/hci0/dev_{address.replace(':', '_')}"
        print(f"[INFO] Frakobler fra path: {device_path}")

        try:
            device = bus.get("org.bluez", device_path)

            # Teststatus før frakobling
            connected = device.Connected
            print(f"[DEBUG] Enheten er {'tilkoblet' if connected else 'ikke tilkoblet'} før Disconnect()")

            if connected:
                device.Disconnect()
                print(f"Koblet fra {name} ({address})")
                return True
            
        except Exception as e:
            print(f"Feil ved frakobling av {name}: {e}")
            return False


    
