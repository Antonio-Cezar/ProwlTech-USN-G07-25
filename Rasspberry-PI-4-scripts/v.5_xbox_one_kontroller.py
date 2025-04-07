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
def send_data(bus, fart, vinkel, rotasjon=0.0):
    try:
        data = struct.pack('<fff', fart, vinkel, rotasjon) # Pakken som sendes. (<ff) er slik den pakkes.
        melding = can.Message(arbitration_id=MSG_ID, data=data, is_extended_id=EXTENDED_ID)
        bus.send(melding)
        print(" ")
        print(" ")
        print("-----------------------")
        print(f"SENDT Fart: {fart:.2f}, Vinkel (rad): {vinkel:.2f}, Rotasjon: {rotasjon:+.2f}")
        print(f"SENDT Fart: {fart:.2f}, Vinkel: {radianer_til_grader(vinkel):.1f}°, Rotasjon: {rotasjon:+.2f}") #Brukte chat til å lage meg en print for debugg
        print("-----------------------")
        print(" ")
        print(" ")
    except Exception as e:
        print(" ")
        print(" ")
        print("-----------------------")
        print("Feil ved sending! :", e)
        print(f"Fart: {fart:.2f}, Vinkel (rad): {vinkel:.2f}, Rotasjon: {rotasjon:+.2f}")
        print(f"Fart: {fart:.2f}, Vinkel: {radianer_til_grader(vinkel):.1f}°, Rotasjon: {rotasjon:+.2f}") #Brukte chat til å lage meg en print for debugg
        print("-----------------------")
        print(" ")
        print(" ")

def send_rotasjon(bus, retning, fart):
    vinkel = 0.0  # placeholder vinkel for rotasjon
    try:
        send_data(bus, fart, vinkel, rotasjon=retning)
        print(" ")
        print(" ")
        print("-----------------------")
        print(f"[ROTASJON] Sendt → Retning: {retning:+.1f}, Fart: {fart:.2f}")
        print("-----------------------")
        print(" ")
        print(" ")
    except Exception as e:
        print(" ")
        print(" ")
        print("-----------------------")
        print("[ROTASJON] Feil ved sending:", e)
        print(f"Fart: {fart:.2f}, Vinkel: {radianer_til_grader(vinkel):.1f}°, Rotasjon: {retning:+.1f}")
        print("-----------------------")
        print(" ")
        print(" ")


# Funksjon for å beregne fart og vinkel.
def beregn_fart_og_vinkel(x, y):
    # dodsone – ignorer små bevegelser
    if abs(x) < 0.1:
        x = 0.0
    if abs(y) < 0.1:
        y = 0.0

    fart = math.sqrt(x ** 2 + y ** 2)
    fart = min(fart, 1.0)

    if fart < 0.1:
        fart = 0.0

    # Vinkel: 0° = opp, 90° = høyre, 180° = ned, 270° = venstre
    vinkel = math.atan2(x, -y)  # OBS: bytt rekkefølge og tegn
    if vinkel < 0:
        vinkel += 2 * math.pi

    return fart, vinkel

# Bestem fart basert på valgt profil
def skaler_fart(fart, profil):
    if profil == 1:
        return fart * 0.3
    elif profil == 2:
        return (fart * 0.3) + (0.3 if fart > 0 else 0.0)
    elif profil == 3:
        return (fart * 0.6) + (0.4 if fart > 0 else 0.0)
    return 0.0


# Funksjon for vibrasjon av kontroller
def rumble_ganger(joystick, antall, varighet=50, mellomrom=0.1):
    for _ in range(antall):
        try:
            joystick.rumble(1.0, 1.0, varighet)
            time.sleep(mellomrom)
            joystick.rumble(0.0, 0.0, 0)  # Stopp
            time.sleep(mellomrom)
        except Exception:
            print("Rumble ikke støttet av kontrolleren.")
            break


def radianer_til_grader(rad):
    return (math.degrees(rad) + 360) % 360

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
hastighetsprofil = 1

#================================================================
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
        rotasjon = 0.0
        rotasjon_aktiv = False

        # === Knapper ===
        a = joystick.get_button(0)  # A-knapp
        b = joystick.get_button(1)  # B-knapp
        y = joystick.get_button(3)  # Y-knapp

        # === Endre hastighetsprofil ===
        if y:
            hastighetsprofil = 1
            print("Hastighetsprofil 1 valgt (0.0-0.3)")
            rumble_ganger(joystick, 1)

        elif b:
            hastighetsprofil = 2
            print("Hastighetsprofil 2 valgt (0.3-0.6)")
            rumble_ganger(joystick, 2)

        elif a:
            hastighetsprofil = 3
            print("Hastighetsprofil 3 valgt (0.4-1.0)")
            rumble_ganger(joystick, 3)

        # === Trigger RT og LT ===
        rt = joystick.get_axis(5)  # Høyre trigger (RT)
        lt = joystick.get_axis(4)  # Venstre trigger (LT)

        rt = (rt + 1.0) / 2.0
        lt = (lt + 1.0) / 2.0


        # Håndter RT (fremover)
        if rt > 0.05:
            fart = skaler_fart(rt, hastighetsprofil)
            vinkel = 0.0  # Radianer
            fart_r = round(fart, 2)
            vinkel_r = round(vinkel, 2)
            send_data(bus, fart_r, vinkel_r)
            aktiv_joystick = True
            continue  # hopp over resten av loopen

        # Håndter LT (bakover)
        if lt > 0.05:
            fart = skaler_fart(lt, hastighetsprofil)
            vinkel = math.pi  # 180°
            fart_r = round(fart, 2)
            vinkel_r = round(vinkel, 2)
            send_data(bus, fart_r, vinkel_r)
            aktiv_joystick = True
            continue # hopp over resten av loopen

        # === kanpper for rotasjon LB og RB ===
        rb = joystick.get_button(4)  # Høyre bumper
        lb = joystick.get_button(5)  # Venstre bumper

        # Håndter rotasjon (RB og LB)
        if rb:
            rotasjon = +1.0
            rotasjon_aktiv = True

        elif lb:
            rotasjon = -1.0
            rotasjon_aktiv = True

        if rotasjon_aktiv:
            rotasjon_fart = {1: 0.2, 2: 0.5, 3: 0.7}.get(hastighetsprofil, 0.2)
            send_data(bus, rotasjon_fart, 0.0, rotasjon=rotasjon)
            aktiv_joystick = True
            continue


        # Joystick
        venstre_x = joystick.get_axis(2)
        venstre_y = joystick.get_axis(3)

        # Beregn fart og vinkel
        fart, vinkel = beregn_fart_og_vinkel(venstre_x, venstre_y)

        # === Skaler fart i henhold til valgt hastighetsprofil ===
        if hastighetsprofil == 1:
            fart = fart * 0.3
        elif hastighetsprofil == 2:
            fart = (fart * 0.3) + (0.3 if fart > 0 else 0.0)
        elif hastighetsprofil == 3:
            fart = (fart * 0.6) + (0.4 if fart > 0 else 0.0)

        # Rund av og send
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
                send_data(bus, 0.0, 0.0, rotasjon=0.0)
                aktiv_joystick = False

            print(" ")
            print(" ")
            print("-----------------------")
            print("[IDLE] Joystick i ro - fart for lav")
            print(f"Fart: {fart:.2f}, Vinkel (rad): {vinkel:.2f}, Rotasjon: {rotasjon:+.2f}")
            print(f"Fart: {fart:.2f}, Vinkel: {radianer_til_grader(vinkel):.1f}°, Rotasjon: {rotasjon:+.2f}")
            print(f"Aktiv profil: {hastighetsprofil}")
            print("-----------------------")
            print(" ")
            print(" ")


    except pygame.error:
        print("Kontroller frakoblet. Søker igjen...")
        time.sleep(1)

    time.sleep(0.1)
#================================================================
#================================================================