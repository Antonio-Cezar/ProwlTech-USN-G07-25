import can
import struct
import time
import pygame
import math
import os


# (PROWLTECH25 - CA)
#================================================================
# KONFIGURASJON
#================================================================
# Konfigurasjon for CAN-oppsett, brukt på Raspberry Pi med Zephyr-enhet
CHANNEL = 'can0'           # CAN-enhetens navn
BITRATE = 500000           # Hastighet på CAN-bussen
EXTENDED_ID = True         # Zephyr bruker 29-bit ID, så vi setter extended ID

# CAN-meldings-IDer (brukes for å identifisere meldinger på bussen)
MSG_ID_motor = 0x0000001   # Melding til motorstyring
MSG_ID_COM = 0x0000020     # Melding for kommandoer (eks. tone)


#================================================================
# FUNKSJONER FOR CAN-KOMMUNIKASJON
#================================================================
# Kontroller verdi sending.
# Funksjonen for pakken som blir sendt
def send_data(bus, fart, vinkel, rotasjon=0.0, sving_js=0.0):
    # Sender kontrollkommando til motor via CAN-bussen.
    # Parametrene representerer:
    # fart: linær hastighet (0.0 til 1.0)
    # vinkel: retning i radianer (0–2π), styrer hvilken vei bilen skal
    # rotasjon: rotasjonshastighet, vanligvis -1.0 (venstre) til +1.0 (høyre)
    # sving_js: verdi fra sving-joystick, brukes til finjustering av sving
    try:
        # Skalere flyttall til heltall (matchende format med Zephyr-C-struktur)
        vinkel_i = int(vinkel * 100)  # OBS: må bruke samme faktor som Zephyr → /100
        fart_i = int(fart * 100)
        rotasjon_i = int(rotasjon * 100)
        sving_js_i = int(sving_js * 100)

        data = struct.pack('<hhhh', fart_i, vinkel_i, rotasjon_i, sving_js_i)# # 8 byte → matcher C-struktur (4 x int16)

        melding = can.Message(
            arbitration_id=MSG_ID_motor,
            data=data,
            is_extended_id=EXTENDED_ID
        )
        bus.send(melding)
        vis_data(fart, vinkel, rotasjon, sving_js, data)

    except Exception as e:
        print("FEIL VED SENDING!!!")
        print(f"→ Fart={fart}, Vinkel={vinkel}, Rotasjon={rotasjon}, Sving={sving_js}")
        print(f"Feilmelding: {e}")

def send_tone(bus, aktiv=True):
    #Sender tone-kommando til microbit (start/stop lyd)
    try:
        # 1 byte: tone på/av
        data = struct.pack('<B', 1 if aktiv else 0)
        melding = can.Message(
            arbitration_id=MSG_ID_COM,  # Ny ID for tone-melding
            data=data,
            is_extended_id=True
        )
        bus.send(melding)
        print(f"{'Starter' if aktiv else 'Stopper'} tone på microbit")
    except Exception as e:
        print("FEIL ved sending av tone-kommando:", e)

def vis_data(fart, vinkel, rotasjon, sving_js, data):
    """Viser verdier sendt til CAN, nyttig for debug"""
    os.system('clear')
    print("========================================")
    print("SENDT DATA TIL CAN-BUS")
    print("========================================")
    print(f"Fart       : {fart:.2f}")
    print(f"Vinkel     : {vinkel:.2f} rad / {radianer_til_grader(vinkel):.1f}°")
    print(f"Rotasjon   : {rotasjon:+.2f}")
    print(f"Sving_js   : {sving_js:+.2f}")
    print("----------------------------------------")
    print(f"Int-verdier: Fart={int(fart*100)}, Vinkel={int(vinkel*100)}, "
          f"Rotasjon={int(rotasjon*100)}, Sving={int(sving_js*100)}")
    print(f"Data-lengde: {len(data)} byte")
    print(f"RAW-data   : {data.hex()}")
    print("========================================\n")



#================================================================
# HJELPEFUNKSJONER
#================================================================
# Funksjon for å beregne fart og vinkel.
def beregn_fart_og_vinkel(x, y):
    #Beregner fart og vinkel basert på joystickens posisjon
    # Død-sone – ignorer små bevegelser
    if abs(x) < 0.1:
        x = 0.0
    if abs(y) < 0.1:
        y = 0.0

    fart = math.sqrt(x ** 2 + y ** 2)
    fart = min(fart, 1.0)

    if fart < 0.1:
        fart = 0.0

    # Justert vinkel:
    vinkel = math.atan2(-x, -y)  # bytter x og y + inverterer x for å snu retningen

    if vinkel < 0:
        vinkel += 2 * math.pi

    return fart, vinkel


# Bestem fart basert på valgt profil
def skaler_fart(fart, profil):
    #Tilpasser fart etter valgt hastighetsmodus
    if profil == 1:
        return fart * 0.3
    elif profil == 2:
        return (fart * 0.3) + (0.3 if fart > 0 else 0.0)
    elif profil == 3:
        return (fart * 0.6) + (0.4 if fart > 0 else 0.0)
    return 0.0


# Funksjon for vibrasjon av kontroller
def vibrer_kont_funk(joystick, antall, varighet=50, mellomrom=0.1):
     #Vibrerer kontrolleren for tilbakemelding ved modusbytte
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



#================================================================
# INITIALISERING AV CAN OG KONTROLLER
#================================================================
# Starter opp CAN-bus sending
def start_bus():
    try:
        print("Starter CAN kommunikajson...")
        bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan', bitrate=BITRATE)
        print("CAN er på")
        return bus
    except Exception as e:
        print("FEIL: CAN er IKKE på", e)
        return None

def start_joystick():
    # Søker etter og initialiserer første tilkoblede joystick
    pygame.joystick.quit()
    pygame.joystick.init()

    while True:
        antall = pygame.joystick.get_count()

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
            print("Ingen kontroller funnet. Venter på tilkobling...")
            time.sleep(1)
            pygame.joystick.quit()
            pygame.joystick.init()



#================================================================
# HOVEDPROGRAM
#================================================================
# bibliotek pygame og kontroller
pygame.init()
bus = start_bus()

if bus is None:
    exit()

joystick = None
sist_sendte_fart = None # Husk forrige vinkel for å sjekke endring
sist_sendte_vinkel = None  # Husk forrige vinkel for å sjekke endring
aktiv_joystick = False
Hastighetsmodus = 1
forrige_x = 0
tone_aktiv = False

# Loop som registrerer verdier og sender verdier til pakke funsjonene (send_data)
while True:
    if joystick is None:
        joystick = start_joystick()
        continue  # prøv igjen neste runde

    try:
        pygame.event.pump()

        antall = pygame.joystick.get_count()
        if antall == 0:
            raise pygame.error("Ingen kontroller funnet – kan være frakoblet")

        # Test ved å lese knapp
        _ = joystick.get_button(0)

    except (pygame.error, OSError):
        print("Kontroller frakoblet! Søker etter ny tilkobling...")
        joystick = None
        pygame.joystick.quit()
        pygame.joystick.init()
        time.sleep(1)
        continue

    try:
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

        # == Input Joystick høyre ===
        venstre_x = joystick.get_axis(2)
        venstre_y = joystick.get_axis(3)

        # === Input sving akse (venstre joystick) ===
        sving_js = joystick.get_axis(0)
        sving_js = 0.0 if abs(sving_js) < 0.1 else sving_js  # Død-sone

        # === Input Trigger RT og LT ===
        rt = joystick.get_axis(4)  # Høyre trigger (RT)
        lt = joystick.get_axis(5)  # Venstre trigger (LT)

        rt = (rt + 1.0) / 2.0
        lt = (lt + 1.0) / 2.0

        # Håndter rotasjon (RB og LB)
        # Bestem rotasjon basert på bumpere

        x = joystick.get_button(3)

        # Send tone på når knappen trykkes ned, og tone av når den slippes
        if x != forrige_x:
            if x == 1:
                send_tone(bus, aktiv=True)
            else:
                send_tone(bus, aktiv=False)

        forrige_x = x  # Oppdater knappestatus



        # === Endre hastighetsmoduser ===
        if y:
            Hastighetsmodus = 1
            print("Hastighetsmodus 1 valgt (0.0-0.3)")
            vibrer_kont_funk(joystick, 1)

        elif b:
            Hastighetsmodus = 2
            print("Hastighetsmodus 2 valgt (0.3-0.6)")
            vibrer_kont_funk(joystick, 2)

        elif a:
            Hastighetsmodus = 3
            print("Hastighetsmodus 3 valgt (0.4-1.0)")
            vibrer_kont_funk(joystick, 3)

        # Velg rotasjonsfart som faktor
        rotasjon_faktor = {1: 0.2, 2: 0.5, 3: 0.7}.get(Hastighetsmodus, 0.2)

        # Håndter RT (fremover)
        if rt > 0.05:
            fart = skaler_fart(rt, Hastighetsmodus)
            vinkel = 0.0  # Radianer
            fart_r = round(fart, 2)
            vinkel_r = round(vinkel, 2)
            send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon, sving_js=sving_js)

            aktiv_joystick = True
            continue  # hopp over resten av loopen

        # Håndter LT (bakover)
        if lt > 0.05:
            fart = skaler_fart(lt, Hastighetsmodus)
            vinkel = math.pi  # 180°
            fart_r = round(fart, 2)
            vinkel_r = round(vinkel, 2)
            send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon, sving_js=sving_js)

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
            send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon, sving_js=sving_js)
            aktiv_joystick = True
            sist_sendte_fart = fart_r
            sist_sendte_vinkel = vinkel_r
            continue

        # Bare send hvis fart er stor nok så vil den sende til bus
        if fart_r > 0.01:
            if fart_r != sist_sendte_fart or vinkel_r != sist_sendte_vinkel:
                send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon, sving_js=sving_js)
                sist_sendte_fart = fart_r
                sist_sendte_vinkel = vinkel_r
                aktiv_joystick = True
                continue
        else:
            if rotasjon != 0.0:
                vinkel = 0.0  # Kan være hva som helst egentlig
                send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon, sving_js=sving_js)
                aktiv_joystick = True
                sist_sendte_fart = fart
                sist_sendte_vinkel = vinkel

            elif aktiv_joystick:
                # Stoppbevegelse kun én gang
                send_data(bus, 0.0, 0.0, rotasjon=0.0, sving_js=0.0)
                aktiv_joystick = False
                sist_sendte_fart = 0.0
                sist_sendte_vinkel = 0.0

        # Håndterer sving funksjonaliteten
        # Håndterer sving-funksjonalitet uten å endre vinkel
        if abs(sving_js) > 0.01:
            # La vinkel være uendret – vi bare sender sving_js
            send_data(bus, fart_r, vinkel_r, rotasjon=rotasjon, sving_js=sving_js)
            aktiv_joystick = True
            sist_sendte_fart = fart_r
            sist_sendte_vinkel = vinkel_r
            continue

        print(" ")
        print(" ")
        os.system('clear')
        print("========================================")
        print("IDLE – Joystick i ro")
        print("========================================")
        print(f"Fart       : {fart:.2f}")
        print(f"Vinkel     : {vinkel:.2f} rad / {radianer_til_grader(vinkel):.1f}°")
        print(f"Rotasjon   : {rotasjon:+.2f}")
        print(f"Sving_js   : {sving_js:+.2f}")
        print("========================================\n")
        print(" ")
        print(" ")


    except pygame.error:
        print("Kontroller frakoblet. Søker igjen...")
        time.sleep(1)

    time.sleep(0.1)
#================================================================
#================================================================