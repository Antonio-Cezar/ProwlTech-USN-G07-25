import platform
import time

#------------------------------------Mock-versjon for å teste på Windows-------------------------------------------
# Sjekker om koden kjøres på Windows eller Linux
if platform.system() != "Linux":
    print("Kjører i Windows")

    # Oppretter falsk data
    found_devices = {"00:11:22:33:44:55": "TestController A", "66:77:88:99:AA:BB": "TestController B"}

    # Simulerer skanning med ventetid
    def scan_devices():
        print("Mock: Simulerer skanning...")
        time.sleep(2)   # Tid til å vise progressbaren 

    # Returnerer de falske enhetene
    def get_devices():
        return found_devices
    
    # Returnerer MAC-adressen basert på enhet-navn
    def get_device(name):
        for addr, dev_name in found_devices.items():
            if dev_name == name:
                return addr
        return None
    
    # Simulerer tilkobling
    def connect_to_device(name):
        print(f"Mock: Koblet til {name}")
        return True
    
    # Simulerer frakobling
    def disconnect_from_device(name):
        print(f"Mock: Koblet fra {name}")
        return True
    
    def get_raw_devices():
        return {}

    
#------------------------------------Ekte versjon på Linux-------------------------------------------
#   Skanner etter tilgjengelige enheter via Bluetooth, filtrerer bort enheter uten navn. Koble til og koble fra enheter. Henter all informasjon og returnerer til GUI-kode.
else:
    from pydbus import SystemBus
    import subprocess
    import os

    found_devices = {}  # Liste for enheter funnet under skanning

    # Sjekker om enheten er paret med adapteren
    def is_paired(adapter_address, device_address):
        path = f"/var/lib/bluetooth/{adapter_address}/{device_address}/info"
        if not os.path.exists(path):
            return False

        # Leser info-filen og sjekker etter Paried=True
        with open(path, "r") as file:
            content = file.read()
            return "Paired=true" in content

    # Skanner etter enheter via Bluez D-Bus
    def scan_devices():
        global found_devices
        found_devices.clear()

        # Koble til D-Bus
        bus = SystemBus()
        mngr = bus.get("org.bluez", "/")
        adapter = bus.get("org.bluez", "/org/bluez/hci0")

        # Start Bluetooth-skanning via BlueZ
        print("Starter søket...")
        try:
            adapter.StartDiscovery()
            print("Søker...")
            time.sleep(10)  # Tid for å oppdage enheter
            adapter.StopDiscovery()
        except Exception as e:
            print(f"Skanning feilet: {e}")

        print("Skanning avsluttet.\n")

        # Henter enheter funnet via D-Bus object manager
        managed_objects = mngr.GetManagedObjects()

        for path, interfaces in managed_objects.items():
            if "org.bluez.Device1" in interfaces:
                dev = interfaces["org.bluez.Device1"]
                address = dev.get("Address", "ukjent")
                name = dev.get("Name")

                if not name:
                    continue    # Ignorerer enheter uten navn

                # Henter status om paret og tilkoblet
                paired = dev.get("Paired", False)
                connected = dev.get("Connected", False)

                # Legger til enheter i listen
                found_devices[address] = name
                print(f" {name} ({address})")

              
    # Returnerer liste med funnede enheter
    def get_devices():
        return found_devices

    # Returnerer MAC-adresser basert på enhet-navn
    def get_device(name):
        for addr, dev_name in found_devices.items():
            if dev_name == name:
                return addr
        return None

    # Kobler til enhet basert på navn
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
            print(f"Tilkoblet {name}")
            return True
        except Exception as e:
            print(f"Feil ved tilkobling til {name}: {e}")
            return False


        
    # Kobler fra enhet og fjerner fra pairing-liste
    def disconnect_from_device(name):
        address = get_device(name)
        if not address:
            print(f"Fant ikke enhet med navn {name}")
            return False

        bus = SystemBus()
        device_path = f"/org/bluez/hci0/dev_{address.replace(':', '_')}"
        device = bus.get("org.bluez", device_path)
        adapter = bus.get("org.bluez", "/org/bluez/hci0")

        try:
            if device.Connected:
                device.Disconnect()     # Kobler fra
                print(f"Koblet fra {name} ({address})")
                
            adapter.RemoveDevice(device_path)   # Fjerner enheten fra listen over kjente paringer
            print(f"Fjernet {name} fra kjent-liste")
            return True
            
        except Exception as e:
            print(f"Feil ved frakobling av {name}: {e}")
            return False

    # Funksjon for å returnere alle enheter som aktivt er tilkoblet
    def get_raw_devices():
        bus = SystemBus()
        mngr = bus.get("org.bluez", "/")
        managed_objects = mngr.GetManagedObjects()

        devices = {}
        for path, interfaces in managed_objects.items():
            if "org.bluez.Device1" in interfaces:
                dev = interfaces["org.bluez.Device1"]  # Bruk data direkte
                address = dev.get("Address")
                devices[address] = dev  # Lagrer device-info for senere
        return devices
