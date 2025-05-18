"""

controller.py

Henter data for å sjekke hastighetsmodus fra kontrolleren. Bruker skriptet v.5_xbox_one_kontroller.py

"""

import platform

#------------------------------------Mock-versjon for å teste på Windows-------------------------------------------
# Sjekker om koden kjøres på Windows eller Linux
if platform.system() != "Linux":
    print("Kjører i Windows(hastighetsmodus)")

    import threading
    import time
    from bluetooth_dbus import get_raw_devices

    class ControllerThread(threading.Thread):
        def __init__(self):
            super().__init__(daemon=True)
            self.proc = None
            self.sent_once = False
            self.last_connected = False

            self.listeners = []

        def register_callback(self, fn):
            self.listeners.append(fn)

        def notify_all(self, mode):
            for fn in self.listeners:
                fn(mode)

        def run(self):
            while True:
                devices = get_raw_devices()
                connected = False

                for dev in devices.values():
                    if dev["Name"] == "Xbox Wireless Controller" and dev["Connected"]:
                        connected = True
                        break

                # Tilkoblet nå, men var ikke før → send modus
                if connected and not self.last_connected:
                    print("Mock: Kontroller tilkoblet – sender modus 1")
                    for fn in self.listeners:
                        fn(1)

                # Frakoblet nå, men var tilkoblet før → send None
                elif not connected and self.last_connected:
                    print("Mock: Kontroller frakoblet – sender None")
                    for fn in self.listeners:
                        fn("-")

                self.last_connected = connected
                time.sleep(3)

    controller = ControllerThread()
    controller.start()

    # Hjelpefunksjon for andre skripts til å registrere seg
    def add_mode_listener(fn):
        controller.register_callback(fn)

#------------------------------------Ekte versjon på Linux-------------------------------------------
#   Skanner etter tilgjengelige enheter via Bluetooth, filtrerer bort enheter uten navn. Koble til og koble fra enheter. Henter all informasjon og returnerer til GUI-kode.
else:

    import threading    # For å kjøre kode i bakgrunnen
    import subprocess   # Bruke eksterne skript
    import sys  
    import re   # Lete etter tekstmønstre med regex
    from pathlib import Path    # Bygge filstier 

    # Finner stien til det ekstere skriptet som er brukt
    SCRIPT = Path(__file__).parent.parent / "Rasspberry-PI-4-scripts" / "v.5_xbox_one_kontroller.py"


    class ControllerThread(threading.Thread):
        # Regex (mønstersøk) for å fange opp linjer med Hastighetsmodus-informasjon
        mode_re = re.compile(r"Hastighetsmodus\s*(\d)\s*valgt", re.IGNORECASE)  # \d: ett siffer // \s*: mellomrom // re.IGNORECASE: ignorerer forskjellen på store/små bokstaver

        def __init__(self):
            super().__init__(daemon=True)   # daemon slik at tråden stopper når hovedprogrammet avsluttes
            self.listeners = []    # Liste med "lyttere" (funksjoner som skal få beskjed når modus endres)

            # Starter orginalskriptet via Python
            self.proc = subprocess.Popen(
                [sys.executable, "-u", str(SCRIPT)],    # -u: unbuffered (viktig for sanntid)
                cwd=str(SCRIPT.parent), # Setter arbeidsmappe til skriptets mappe
                stdout=subprocess.PIPE, # Fanger all output
                stderr=subprocess.STDOUT, # Fanger all output
                bufsize=1,  # Linjebufret
                text=True,  # Tekststrenger, ikke bytes
            )
            
        # Legger til funksjonene som skal få beskjed om endret modus
        def register_callback(self, fn):
            self.listeners.append(fn)

        # Gir ut den nye modusen til alle "lytterene"
        def notify_all(self, mode):
            for fn in self.listeners:
                fn(mode)

        # Kjøres når tråden starter 
        def run(self):
            assert self.proc.stdout is not None

            # Leser linje for linje 
            for line in self.proc.stdout:
                m = self.mode_re.search(line)   # Sjekker om linjen inneholder ønsket tekst (bruker regex-en fra tidligere)
                if m:
                    mode = int(m.group(1))  # Trekker ut sifferet 
                    print(f"Fant modus: {mode}")
                    self.notify_all(mode)  # Varsler alle callbacks

    # Starter bakgrunnstråd
    controller = ControllerThread()
    controller.start()

    # Hjelpefunksjon for andre skripts til å registrere seg
    def add_mode_listener(fn):
        controller.register_callback(fn)
