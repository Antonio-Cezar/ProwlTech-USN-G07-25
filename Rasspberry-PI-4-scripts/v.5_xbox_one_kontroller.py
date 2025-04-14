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
MSG_ID = 0x0000001  # samme som RECEIVE_ID hos motor-MB
#================================================================


#================================================================
# (PROWLTECH25 - CA)
# Kontroller verdi sending.

# Funksjonen for pakken som blir sendt
def send_data(bus, fart, vinkel, rotasjon=0.0):
    try:
        vinkel_i = int(vinkel * 100)  # OBS: må bruke samme faktor som Zephyr → /100
        fart_i = int(fart * 100)
        rotasjon_i = int(rotasjon * 100)

        data = struct.pack('<hhh', fart_i, vinkel_i, rotasjon_i)  # 6 byte → matcher C

        melding = can.Message(
            arbitration_id=MSG_ID,
            data=data,
            is_extended_id=EXTENDED_ID
        )
        bus.send(melding)
        print(" ")
        print(" ")
        print("-----------------------")
        print(f"SENDT → Fart: {fart:.2f}, Vinkel (rad): {vinkel:.2f}, Rotasjon: {rotasjon:+.2f}")
        print(f"SENDT → Fart: {fart:.2f}, Vinkel: {radianer_til_grader(vinkel):.1f}°, Rotasjon: {rotasjon:+.2f}")
        print(" ")
        print(" ")
        print(f"[DEBUG] Skal sende → Fart: {fart}, Vinkel: {vinkel}, Rotasjon: {rotasjon}")
        print(f"[DEBUG] RB: {rb}, LB: {lb}, rotasjon: {rotasjon}")
        print(f"[DEBUG] Data-lengde: {len(data)} byte")
        print("RAW DATA (hex):", data.hex())
        print("-----------------------")
        print(" ")
        print(" ")
    except Exception as e:
        print(" ")
        print(" ")
        print("-----------------------")
        print("Feil ved sending! :", e)
        print(f"→ Fart: {fart:.2f}, Vinkel (rad): {vinkel:.2f}, Rotasjon: {rotasjon:+.2f}")
        print(f"→ Fart: {fart:.2f}, Vinkel: {radianer_til_grader(vinkel):.1f}°, Rotasjon: {rotasjon:+.2f}")
        print(" ")
        print(" ")
        print(f"[DEBUG] Skal sende → Fart: {fart}, Vinkel: {vinkel}, Rotasjon: {rotasjon}")
        print(f"[DEBUG] RB: {rb}, LB: {lb}, rotasjon: {rotasjon}")
        print(f"[DEBUG] Data-lengde: {len(data)} byte")
        print("RAW DATA (hex):", data.hex())
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
Hastighetsmodus = 1

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

        # === Input Knapper ===
        a = joystick.get_button(0)  # A-knapp
        b = joystick.get_button(1)  # B-knapp
        y = joystick.get_button(4)  # Y-knapp

        # === Input kanpper for rotasjon LB og RB ===
        rb = joystick.get_button(7)  # Høyre bumper
        lb = joystick.get_button(6)  # Venstre bumper

        rotasjon = 0.0
        if rb:
            rotasjon = +1.0
        elif lb:
            rotasjon = -1.0

        # == Input Joystick ===
        venstre_x = joystick.get_axis(2)
        venstre_y = joystick.get_axis(3)

        # === Input Trigger RT og LT ===
        rt = joystick.get_axis(4)  # Høyre trigger (RT)
        lt = joystick.get_axis(5)  # Venstre trigger (LT)

        rt = (rt + 1.0) / 2.0
        lt = (lt + 1.0) / 2.0

        # Håndter rotasjon (RB og LB)
        # Bestem rotasjon basert på bumpere

        # === Endre hastighetsmoduser ===
        if y:
            Hastighetsmodus = 1
            print("Hastighetsmodus 1 valgt (0.0-0.3)")
            rumble_ganger(joystick, 1)

        elif b:
            Hastighetsmodus = 2
            print("Hastighetsmodus 2 valgt (0.3-0.6)")
            rumble_ganger(joystick, 2)

        elif a:
            Hastighetsmodus = 3
            print("Hastighetsmodus 3 valgt (0.4-1.0)")
            rumble_ganger(joystick, 3)

        # Velg rotasjonsfart som faktor
        rotasjon_faktor = {1: 0.2, 2: 0.5, 3: 0.7}.get(Hastighetsmodus, 0.2)

        # Håndter RT (fremover)
        if rt > 0.05:
            fart = skaler_fart(rt, Hastighetsmodus)
            vinkel = 0.0  # Radianer
            fart_r = round(fart, 2)
            vinkel_r = round(vinkel, 2)
            send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon * rotasjon_faktor)
            aktiv_joystick = True
            continue  # hopp over resten av loopen

        # Håndter LT (bakover)
        if lt > 0.05:
            fart = skaler_fart(lt, Hastighetsmodus)
            vinkel = math.pi  # 180°
            fart_r = round(fart, 2)
            vinkel_r = round(vinkel, 2)
            send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon * rotasjon_faktor)
            aktiv_joystick = True
            continue # hopp over resten av loopen

        # Beregn fart og vinkel
        fart, vinkel = beregn_fart_og_vinkel(venstre_x, venstre_y)

        # === Skaler fart i henhold til valgt Hastighetsmodus ===
        if Hastighetsmodus == 1:
            fart = fart * 0.3
        elif Hastighetsmodus == 2:
            fart = (fart * 0.3) + (0.3 if fart > 0 else 0.0)
        elif Hastighetsmodus == 3:
            fart = (fart * 0.6) + (0.4 if fart > 0 else 0.0)

        # Rund av og send
        fart_r = round(fart, 2)
        vinkel_r = round(vinkel, 2)

        if rotasjon != 0.0:
            # Bruk maks fart for valgt modus, uavhengig av joystick-input
            if Hastighetsmodus == 1:
                fart_r = 0.3
            elif Hastighetsmodus == 2:
                fart_r = 0.6
            elif Hastighetsmodus == 3:
                fart_r = 1.0
            else:
                fart_r = 0.3  # fallback

            vinkel_r = 0.0  # fast retning for rotasjon
            send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon * rotasjon_faktor)
            aktiv_joystick = True
            sist_sendte_fart = fart_r
            sist_sendte_vinkel = vinkel_r
            continue



        # Bare send hvis fart er stor nok så vil den sende til bus
        if fart_r > 0.01:
            if fart_r != sist_sendte_fart or vinkel_r != sist_sendte_vinkel:
                send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon * rotasjon_faktor)
                sist_sendte_fart = fart_r
                sist_sendte_vinkel = vinkel_r
                aktiv_joystick = True
        else:
            if rotasjon != 0.0:
                vinkel = 0.0  # Kan være hva som helst egentlig
                send_data(bus, fart, vinkel, rotasjon=rotasjon * rotasjon_faktor)
                aktiv_joystick = True
                sist_sendte_fart = fart
                sist_sendte_vinkel = vinkel

            elif aktiv_joystick:
                # Stoppbevegelse kun én gang
                send_data(bus, 0.0, 0.0, rotasjon=0.0)
                aktiv_joystick = False
                sist_sendte_fart = 0.0
                sist_sendte_vinkel = 0.0


            print(" ")
            print(" ")
            print("-----------------------")
            print("[IDLE] Joystick i ro – fart for lav")
            print(f"→ Fart: {fart:.2f}, Vinkel (rad): {vinkel:.2f}, Rotasjon: {rotasjon:+.2f}")
            print(f"→ Fart: {fart:.2f}, Vinkel: {radianer_til_grader(vinkel):.1f}°, Rotasjon: {rotasjon:+.2f}")
            print(f"Aktiv hastighetsmodus: {Hastighetsmodus}")
            print(" ")
            print(" ")
            print(f"[DEBUG] Skal sende → Fart: {fart}, Vinkel: {vinkel}, Rotasjon: {rotasjon}")
            print(f"[DEBUG] RB: {rb}, LB: {lb}, rotasjon: {rotasjon}")
            print("-----------------------")
            print(" ")
            print(" ")


    except pygame.error:
        print("Kontroller frakoblet. Søker igjen...")
        time.sleep(1)

    time.sleep(0.1)
#================================================================
#================================================================