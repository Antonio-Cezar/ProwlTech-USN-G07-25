import can
import struct
import time
import pygame
import math


#================================================================
# (PROWLTECH25 - CA)
# Konfigurasjon (tilpasset Zephyt microbit oppsett)
CHANNEL = 'can0'           # CAN på PI
BITRATE = 500000           # Bitrate
EXTENDED_ID = True         # Brukes fordi Zephyr bruker 29-bit ID (IDE flag satt definert fra før)

# ID til scriptet for sending av kontroller verdier
MSG_ID = 0x0000004
#================================================================


#================================================================
# (PROWLTECH25 - CA)
# Kontroller verdi sending.

# Funksjonen for pakken som blir sendt
def send_data(bus, fart, vinkel):
    try:
        data = struct.pack('<ff', fart, vinkel) # Pakken som sendes. (<ff) er slik den pakkes.
        melding = can.Message(arbitration_id=MSG_ID, data=data, is_extended_id=EXTENDED_ID)
        bus.send(melding)
        print(f"SENDT Fart: {fart:.2f}, Vinkel (rad): {vinkel:.2f}")


    except Exception as e:
        print("Feil ved sending! :", e)

# Funksjon for å beregne fart og vinkel.
def beregn_fart_og_vinkel(x, y):
    # Deadzone for å ignorere små bevegelser
    if abs(x) < 0.05:
        x = 0.0
    if abs(y) < 0.05:
        y = 0.0

    fart = math.hypot(x, y)
    fart = min(fart, 1.0)

    vinkel = math.atan2(-y, x)
    vinkel = (vinkel + math.pi / 2) % (2 * math.pi)

    return fart, vinkel


# Starter opp CAN-bus sending
def initialize_bus():
    try:
        print("Starter CAN kommunikajson...")
        bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan', bitrate=BITRATE)
        print("CAN er på")
        return bus
    except Exception as e:
        print("FEIL: CAN er IKKE på", e)
        return None

def initialize_joystick():
    pygame.joystick.init()
    antall = pygame.joystick.get_count()
    print(f"Antall kontrollere funnet: {antall}")
    
    if antall > 0:
        for i in range(antall):
            js = pygame.joystick.Joystick(i)
            js.init()
            print(f"Joystick {i}: {js.get_name()}")
        joystick = pygame.joystick.Joystick(0)
        joystick.init()
        print(f"Bruker joystick 0: {joystick.get_name()}")
        return joystick
    else:
        print("Feil: ingen kontroller funnet.")
        time.sleep(1)
        return None


# bibliotek pygame og kontroller
pygame.init()
bus = initialize_bus()

if bus is None:
    exit()

joystick = None
sist_sendte_fart = None # Husk forrige vinkel for å sjekke endring
sist_sendte_vinkel = None  # Husk forrige vinkel for å sjekke endring
aktiv_joystick = False

# Loop som registrerer verdier og sender verdier til pakke funsjonene (send_data)
while True:

    if joystick is None:
        joystick = initialize_joystick()
    if joystick is None:
        print("Ingen kontroller funnet. Søker igjen snart...")
        time.sleep(1)
        continue

    try:
        pygame.event.pump() # Den oppdaterer interne verdier for kontrolleren (og andre input-enheter).

        # Joystick
        venstre_x = joystick.get_axis(3)
        venstre_y = joystick.get_axis(4)

        # Beregn fart og vinkel
        fart, vinkel = beregn_fart_og_vinkel(venstre_x, venstre_y)
        fart_r = round(fart, 2)
        vinkel_r = round(vinkel, 2)

        # Bare send hvis joystick røres (fart) og vinkelendring er stor nok
        if fart_r > 0.01:
            send_data(bus, fart_r, vinkel_r)
            sist_sendte_fart = fart_r
            sist_sendte_vinkel = vinkel_r
            aktiv_joystick = True  # Nå er joystick aktiv
        else:
            if aktiv_joystick:
                # Send null-pakke når den slippes
                send_data(bus, 0.0, 0.0)
                aktiv_joystick = False
            print("[IDLE] Joystick i ro - fart for lav")

    except pygame.error:
        print("Kontroller frakoblet. Søker igjen...")
        time.sleep(1)

    time.sleep(0.2)

#================================================================