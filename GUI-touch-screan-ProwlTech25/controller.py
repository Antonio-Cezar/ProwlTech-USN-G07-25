"""

controller.py

Henter data for å sjekke hastighetsmodus fra kontrolleren. Bruker skriptet v.5_xbox_one_kontroller.py

"""

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
