"""

bluetooth_dbus.py

Håndterer Bluetooth-enheter via D-Bus på Raspberry Pi.

Brukes i GUI for å koble til/fra kontroller og hente tilkoblingsstatus.
Inkluderer mock-versjon for bruk i Windows.

"""

import platform
import time

#------------------------------------Mock-versjon for å teste på Windows-------------------------------------------
# Sjekker om koden kjøres på Windows eller Linux
if platform.system() != "Linux":
    print("Kjører i Windows(tilkoblingsstatus)")

    mock_connected_device = None    # Lagrer mock-enhet som er tilkoblet

    # Simulerer funnede enheter (navn og MAC-adresse)
    found_devices = {"00:11:22:33:44:55": "Xbox Wireless Controller"}

    # Simulerer skanning med ventetid
    def scan_devices():
        print("Mock: Simulerer skanning...")
        time.sleep(2)   # Tid til å vise progressbaren 

    # Returnerer mock-enhetene
    def get_devices():
        return found_devices
    
    # Returnerer MAC-adressen basert på enhetsnavn
    def get_device(name):
        for addr, dev_name in found_devices.items():
            if dev_name == name:
                return addr
        return None
    
    # Simulerer tilkobling
    def connect_to_device(name):
        print(f"Mock: Koblet til {name}")
        global mock_connected_device
        mock_connected_device = name    # Setter til valgt navn
        return True
    
    # Simulerer frakobling
    def disconnect_from_device(name):
        global mock_connected_device
        print(f"Mock: Koblet fra {name}")
        if mock_connected_device == name:
            mock_connected_device = None
        return True
    
    # Returnerer alle mock-enheter med navn og om de er tilkoblet
    def get_raw_devices():
        devices = {}
        for addr, name in found_devices.items():
            devices[addr] = {
                "Name": name,
                "Connected": name == mock_connected_device
            }
        return devices

    
#------------------------------------Ekte versjon på Linux-------------------------------------------
#   Skanner etter tilgjengelige enheter via Bluetooth, filtrerer bort enheter uten navn. Koble til og koble fra enheter. Henter all informasjon og returnerer til GUI-kode.
else:
    from pydbus import SystemBus
    import subprocess
    import os

    found_devices = {}  # Liste for enheter funnet under skanning

    # Sjekker om pairing-info finnes
    def is_paired(adapter_address, device_address):
        path = f"/var/lib/bluetooth/{adapter_address}/{device_address}/info"
        if not os.path.exists(path):
            return False

        # Leser innholdet i pairing-filen og ser etter "Paired=True"
        with open(path, "r") as file:
            content = file.read()
            return "Paired=true" in content

    # Skanner etter enheter via Bluez D-Bus
    def scan_devices():
        global found_devices
        found_devices.clear()

        # Koble til D-Bus
        bus = SystemBus()
        mngr = bus.get("org.bluez", "/")    # Object manager
        adapter = bus.get("org.bluez", "/org/bluez/hci0")   # Bluetooth-adapter

        # Start Bluetooth-skanning via BlueZ
        print("Starter søket...")
        try:
            adapter.StartDiscovery()            # Starter skanning
            print("Søker...")
            time.sleep(10)                      # Tid for å oppdage enheter
            adapter.StopDiscovery()             # Stopper skanning
        except Exception as e:
            print(f"Skanning feilet: {e}")

        print("Skanning avsluttet.\n")

        managed_objects = mngr.GetManagedObjects()  # Henter enheter funnet via D-Bus object manager

        for path, interfaces in managed_objects.items():    # Går gjennom alle Bluetooth-objekter og filtrerer 
            if "org.bluez.Device1" in interfaces:
                dev = interfaces["org.bluez.Device1"]   # Henter enhetsdata
                address = dev.get("Address", "ukjent")  # Henter MAC-adresse
                name = dev.get("Name")                  # Henter navn

                if not name:
                    continue    # Ignorerer enheter uten navn

                # Henter status om paret og tilkoblet
                paired = dev.get("Paired", False)
                connected = dev.get("Connected", False)

                # Legger til enheter i listen som sendes til GUI
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
        address = get_device(name)  # Slår opp MAC-adresse basert på navn
        # Hvis ikke enheten finnes, returnerer False
        if not address:
            print(f"Fant ikke enhet med navn {name}")
            return False

        bus = SystemBus()   # Kobler til D-Bus via pydbus (gir tilgang til Bluez-tjenesten)
        device_path = f"/org/bluez/hci0/dev_{address.replace(':', '_')}"    # Lager D-Bus-sti basert på MAC-adressen

        try:
            device = bus.get("org.bluez", device_path)  # Prøver å hente enhet fra D-Bus med org.bluez
            device.Connect()    # Starter Bluetooth-tilkoblingen
            print(f"Tilkoblet {name}")
            return True
        except Exception as e:
            print(f"Feil ved tilkobling til {name}: {e}")
            return False

        
    # Kobler fra enhet og fjerner fra pairing-liste
    def disconnect_from_device(name):
        address = get_device(name)  # Slår opp MAC-adresse basert på navn
        # Hvis ikke enheten finnes, returnerer False
        if not address:
            print(f"Fant ikke enhet med navn {name}")
            return False

        bus = SystemBus()   # Kobler til D-Bus via pydbus (gir tilgang til Bluez-tjenesten)
        device_path = f"/org/bluez/hci0/dev_{address.replace(':', '_')}"    # Lager D-Bus-sti basert på MAC-adressen

        device = bus.get("org.bluez", device_path)  # Henter enheten
        adapter = bus.get("org.bluez", "/org/bluez/hci0")   # Henter adapteren (for å kunne fjerne paringen)

        # Hvis enheten er tilkoblet, så frakobles den
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
        bus = SystemBus()   # Kobler til D-Bus via pydbus (gir tilgang til Bluez-tjenesten)
        mngr = bus.get("org.bluez", "/")    # Henter Object Manager (oversikt over alle tilknyttede enheter)
        managed_objects = mngr.GetManagedObjects()  # Returnerer alle kjente Bluetooth-objekter (både adaptere og enheter)

        devices = {}
        # path: stien til enheten // interfaces: inholdet knyttet til objektet
        for path, interfaces in managed_objects.items():
            if "org.bluez.Device1" in interfaces:   # Filtrerer ut faktiske Bluetooth-enheter
                dev = interfaces["org.bluez.Device1"]  # Henter detaljene for enheten (navn, tilkobling, MAC-adresse osv.)
                address = dev.get("Address")    # MAC-adressen til enheten 
                devices[address] = dev  # Lagrer hele objektet med detaljer 
        return devices
