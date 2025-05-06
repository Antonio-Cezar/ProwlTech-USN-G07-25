# can_wrapper.py
import platform
import time
import os
import sys
import importlib.util
import pygame

if platform.system() != "Linux":
    # ------------------- Mock på Windows -------------------
    print("Kjører på Windows – bruker mock-CAN-bus")
    class DummyBus:
        def send(self, msg):
            print(f"[MOCK CAN] send(): {msg}")

    def initialize_bus():
        """Returnerer en dummy-buss som bare printer meldinger."""
        print("Mock: initialize_bus()")
        return DummyBus()
    
    def initialize_joystick():
        pygame.joystick.init()
        if pygame.joystick.get_count() > 0:
            js = pygame.joystick.Joystick(0)
            js.init()
            return js
        else:
            return None
        
    def rumble_ganger(joystick, antall, varighet=50, mellomrom=0.1):
        print(f"[MOCK RUMBLE] joystick={joystick}, antall={antall}")



else:
    # ------------- Ekte CAN på Linux ----------------
    # finn originalskriptet på samme måte du alltid gjør
    script_dir = os.path.abspath(
        os.path.join(os.path.dirname(__file__),
                     "..", "Rasspberry-PI-4-scripts"))
    module_path = os.path.join(script_dir, "v.5_xbox_one_kontroller.py")

    spec = importlib.util.spec_from_file_location("xbox_ctrl", module_path)
    xbox_ctrl = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(xbox_ctrl)

    # eksporter funksjonene fra originalen
    initialize_bus = xbox_ctrl.initialize_bus
    initialize_joystick = xbox_ctrl.initialize_joystick
    rumble_ganger = xbox_ctrl.rumble_ganger
