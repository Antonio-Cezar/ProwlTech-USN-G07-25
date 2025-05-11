"""

controller.py

Henter data for å sjekke hastighetsmodus fra kontrolleren. Bruker skriptet v.5_xbox_one_kontroller.py

"""

import platform

#------------------------------------Mock-versjon for å teste på Windows-------------------------------------------
# Sjekker om koden kjøres på Windows eller Linux
if platform.system() != "Linux":
    print("Kjører i Windows")

    import threading
    import time
    from bluetooth_dbus import get_raw_devices

    _listeners = []

    class ControllerThread(threading.Thread):
        def __init__(self):
            super().__init__(daemon=True)
            self.proc = None
            self.sent_once = False
            self.last_connected = False

        def add_mode_listener(self, fn):
                _listeners.append(fn)

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
                    for fn in _listeners:
                        fn(1)

                # Frakoblet nå, men var tilkoblet før → send None
                elif not connected and self.last_connected:
                    print("Mock: Kontroller frakoblet – sender None")
                    for fn in _listeners:
                        fn("-")

                self.last_connected = connected
                time.sleep(3)

    ControllerThread().start()

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
        MODE_RE = re.compile(r"Hastighetsmodus\s*(\d)\s*valgt", re.IGNORECASE)  # \d: ett siffer // \s*: mellomrom // re.IGNORECASE: ignorerer forskjellen på store/små bokstaver

        def __init__(self):
            super().__init__(daemon=True)   # daemon slik at tråden stopper når hovedprogrammet avsluttes
            self._listeners = []    # Lagrer funksjoner som skal kalles når en modus endres (callback-system)

            # Starter orginalskriptet via Python
            self.proc = subprocess.Popen(
                [sys.executable, "-u", str(SCRIPT)],    # -u: unbuffered (viktig for sanntid)
                cwd=str(SCRIPT.parent), # Setter arbeidsmappe til skriptets mappe
                stdout=subprocess.PIPE, # Fanger all output
                stderr=subprocess.STDOUT, # Fanger all output
                bufsize=1,  # Linjebufret
                text=True,  # Tekststrenger, ikke bytes
            )
            
        # Callback som mottar det nye modus-tallet, kalles når en match oppdages
        def add_mode_listener(self, fn):
            self._listeners.append(fn)

        # Kall alle listeners med modus-verdien
        def _notify(self, mode):
            for fn in self._listeners:
                fn(mode)

        # Kjøres når tråden starter 
        def run(self):
            assert self.proc.stdout is not None # Sjekker at output-strømmen er tilgjengelig

            # Leser linje for linje 
            for line in self.proc.stdout:
                m = self.MODE_RE.search(line)   # Sjekker om linjen inneholder ønsket tekst (bruker regex-en fra tidligere)

                # Ved match
                if m:
                    mode = int(m.group(1))  # Trekker ut sifferet 
                    print(f"Fant modus: {mode}")
                    self._notify(mode)  # Varsler alle callbacks

    _controller = ControllerThread()
    _controller.start()

    def add_mode_listener(fn):
        _controller.add_mode_listener(fn)
