import threading
import subprocess
import sys
import re
from pathlib import Path

# Finner stien til det ekstere skriptet som er brukt
SCRIPT = Path(__file__).parent.parent / "Rasspberry-PI-4-scripts" / "v.5_xbox_one_kontroller.py"


class ControllerThread(threading.Thread):
    # Regex for å fange opp linjer med Hastighetsmodus-informasjon
    MODE_RE = re.compile(r"Hastighetsmodus\s*(\d)\s*valgt", re.IGNORECASE)

    def __init__(self):
        super().__init__(daemon=True)   # daemon slik at tråden stopper når hovedprogrammet avsluttes
        self._listeners = []    # Liste over funksjoner som kalles når modus endres

        # Starter orginalskriptet som egen prosess
        self.proc = subprocess.Popen(
            [sys.executable, "-u", str(SCRIPT)],    # umiddelbar linjebufring
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

    def run(self):
        assert self.proc.stdout is not None
        # Leser linje for linje 
        for line in self.proc.stdout:
            m = self.MODE_RE.search(line)   # Sjekker om linjen inneholder ønsket tekst
            if m:
                mode = int(m.group(1))  # Trekker ut sifferet 
                print(f"Fant modus: {mode}")
                self._notify(mode)  # Varsler alle callbacks
