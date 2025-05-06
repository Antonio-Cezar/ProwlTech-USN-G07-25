import threading
import subprocess
import sys
import re
import time
import pygame
from pathlib import Path

SCRIPT = Path(__file__).parent.parent / "Rasspberry-PI-4-scripts" / "v.5_xbox_one_kontroller.py"

#from can_wrapper import initialize_bus, initialize_joystick, rumble_ganger

class ControllerThread(threading.Thread):
    MODE_RE = re.compile(r"Hastighetsmodus\s*(\d)\s*valgt")

    def __init__(self):
        super().__init__(daemon=True)
        self._listeners = []

        self.proc = subprocess.Popen(
            [sys.executable, str(SCRIPT)],
            cwd=str(SCRIPT.parent),
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )

        #self.bus = initialize_bus() # Dummy på Windows

        #pygame.init()
        #pygame.joystick.init()
        #self.joystick = initialize_joystick()

        #self.current_mode = 1
        

    def add_mode_listener(self, fn):
        self._listeners.append(fn)

    def _notify(self, mode):
        for fn in self._listeners:
            fn(mode)

    def run(self):
        assert self.proc.stdout is not None
        for line in self.proc.stdout:
            m = self.MODE_RE.search(line)
            if m:
                mode = int(m.group(1))
                self._notify(mode)


        '''
        while True:
            pygame.event.pump()
            if not self.joystick:
                time.sleep(0.1)
                continue

            y = self.joystick.get_button(4)
            b = self.joystick.get_button(1)
            x = self.joystick.get_button(3)

            # Sjekk modus-knappene
            if y and self.current_mode != 1:
                self.current_mode = 1
                self._notify(1)
                rumble_ganger(self.joystick, 1)
            elif b and self.current_mode != 2:
                self.current_mode = 2
                self._notify(2)
                rumble_ganger(self.joystick, 2)
            elif x and self.current_mode != 3:
                self.current_mode = 3
                self._notify(3)
                rumble_ganger(self.joystick, 3)


            time.sleep(0.1)    
        '''
