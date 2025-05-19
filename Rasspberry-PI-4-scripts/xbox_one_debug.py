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
BITRATE = 125000           # Hastighet på CAN-bussen
EXTENDED_ID = True        # Zephyr bruker 29-bit ID, så extended ID settes true

# CAN-meldings-IDer (brukes for å identifisere meldinger på bussen)
MSG_ID_motor = 0x0000001   # Melding til motorstyring
MSG_ID_COM = 0x0000020     # Melding for kommando tone



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
        vinkel_i = int(vinkel * 100)  # Bruke samme faktor som Zephyr → /100
        fart_i = int(fart * 100)
        rotasjon_i = int(rotasjon * 100)
        sving_js_i = int(sving_js * 100)

        data = struct.pack('<hhhh', fart_i, vinkel_i, rotasjon_i, sving_js_i)# 8 byte → sammensvarer C-struktur (4 x int16). h = Short integer size 2.

        # Oppretter en CAN-melding med riktig ID, data og ID-type
        melding = can.Message(
            arbitration_id=MSG_ID_motor,
            data=data,
            is_extended_id=EXTENDED_ID
        )
        # Sender meldingen på CAN-bussen
        bus.send(melding)

        # Viser de opprinnelige verdiene og pakkede data for feilsøking/logging
        vis_data(fart, vinkel, rotasjon, sving_js, data)

    except Exception as e:
        # Hvis noe går galt i try-blokken, så vil feilmelding og verdiene som ble forsøkt sendt vises i konsol
        print("FEIL VED SENDING!!!")
        print(f"→ Fart={fart}, Vinkel={vinkel}, Rotasjon={rotasjon}, Sving={sving_js}")
        print(f"Feilmelding: {e}")

def send_tone(bus, aktiv=True):
    #Sender tone-kommando til microbit (start/stop lyd)
    try:
        # 1 byte: tone på/av
        data = struct.pack('<B', 1 if aktiv else 0) # B = unsigned char size 1.

        # Oppretter en CAN-melding med riktig ID, data og ID-type
        melding = can.Message(
            arbitration_id=MSG_ID_COM,  # ID for tone-melding
            data=data,
            is_extended_id=EXTENDED_ID
        )
        # Sender meldingen på CAN-bussen
        bus.send(melding)

        # Viser de opprinnelige verdiene og pakkede data for feilsøking/logging
        print(f"{'Starter' if aktiv else 'Stopper'} tone på microbit")
    except Exception as e:
        # Hvis noe går galt i try-blokken, så vil feilmelding og verdiene som ble forsøkt sendt vises i konsol
        print("FEIL ved sending av tone-kommando:", e)

def vis_data(fart, vinkel, rotasjon, sving_js, data):
    #Viser verdier sendt til CAN, nyttig for debug
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
    # Død-sone – ignorer små bevegelser
    if abs(x) < 0.1:
        x = 0.0
    if abs(y) < 0.1:
        y = 0.0
    # Beregn fart som hypotenusen til vektoren (x, y)
    fart = math.sqrt(x ** 2 + y ** 2)

    # Begrens farten til maks 1.0
    # Denne kutter verdier over 1.0 så vis "min(1.3, 1.0) = 1.0""
    fart = min(fart, 1.0)

    # Beregn vinkel i radianer – atan2 gir retning med korrekt kvadrant (her inverteres x og y)
    # anran2 er arc tanget 2 brukes til å finne vinkelen til et punkt (x, y) i forhold til origo (0,0)
    vinkel = math.atan2(-x, -y)

    # Gjør vinkelen positiv ved å legge til 2π hvis den er negativ (holder den i 0–2π området)
    if vinkel < 0:
        vinkel += 2 * math.pi
    return fart, vinkel

# Skaler fart i henhold til valgt Hastighetsmodus
def skaler_fart(fart, profil):
     # Hastighet 1: Lav hastighet
    if profil == 1:
        return fart * 0.1  # 0.0–0.1
    
    # Hastighet 2: Moderat hastighet – litt raskere
    elif profil == 2:
        return (fart * 0.5) + (0.1 if fart > 0 else 0.0)  # 0.1–0.6
    
    # Hastighet 3: Høy hastighet – raskeste
    elif profil == 3:
        return (fart * 0.7) + (0.3 if fart > 0 else 0.0)  # 0.3–1.0
    return 0.0

# Funksjon for vibrasjon av kontroller
def vibrer_kont_funk(joystick, antall, varighet=50, mellomrom=0.1):
     #Vibrerer kontrolleren for tilbakemelding ved modusbytte
    for _ in range(antall):
        try:
            # Start vibrasjon
            joystick.rumble(1.0, 1.0, varighet)

            # Vent litt før stopp
            time.sleep(mellomrom)

            # Stopper vibrasjonen
            joystick.rumble(0.0, 0.0, 0)  # Stopp

            # Pause før neste vibrasjon. Dette er for å kunne følle på forskjellen av antal vibrasjoner som skal lik hastighetsmodus 1, 2 eller 3.
            time.sleep(mellomrom)
        except Exception:
            break

def radianer_til_grader(rad):
    # Konverterer radianer til grader og sørger for at resultatet alltid er mellom 0 og 360
    # Blitt brukt for debugging.
    return (math.degrees(rad) + 360) % 360



#================================================================
# INITIALISERING AV CAN OG KONTROLLER
#================================================================
# Starter opp CAN-bus sending
def start_bus():
    try:
        print("Starter CAN kommunikajson...")

        # Opprett CAN-bus med spesifisert kanal og bitrate
        bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan', bitrate=BITRATE)
        print("CAN er på")
        return bus
    except Exception as e:
        # Hvis noe er galt
        print("FEIL: CAN er IKKE på", e)
        return None

def start_joystick():
    # Søker etter og initialiserer første tilkoblede joystick
    pygame.joystick.quit()
    pygame.joystick.init()

    while True:
        # Hent antall tilkoblede joysticks
        antall = pygame.joystick.get_count()

        if antall > 0:
            # Hvis det finnes en eller flere joysticks
            for i in range(antall):
                js = pygame.joystick.Joystick(i) # Velg joystick nr. i
                js.init() # Initialiser den
                print(f"Joystick {i}: {js.get_name()}") # Skriv ut navnet
            joystick = pygame.joystick.Joystick(0)
            joystick.init()
            print(f"Kontroller {i}: {js.get_name()}")
            return joystick
        else:
            # Ingen joystick funnet – vent og prøv igjen
            print("Ingen kontroller funnet. Venter på tilkobling...")
            time.sleep(1)

             # Re-initier for å sjekke på nytt
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
forrige_a = 0
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
        x = joystick.get_button(3)  # x-knapp

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

        # Send tone på når knappen trykkes ned, og tone av når den slippes
        if a != forrige_a:
            if a == 1:
                send_tone(bus, aktiv=True)
            else:
                send_tone(bus, aktiv=False)

        forrige_a = a  # Oppdater knappestatus

        # === Endre hastighetsmoduser ===
        if y:
            Hastighetsmodus = 1
            print("Hastighetsmodus 1 valgt (0.0-0.1)")
            vibrer_kont_funk(joystick, 1)

        elif b:
            Hastighetsmodus = 2
            print("Hastighetsmodus 2 valgt (0.1-0.6)")
            vibrer_kont_funk(joystick, 2)

        elif x:
            Hastighetsmodus = 3
            print("Hastighetsmodus 3 valgt (0.3-1.0)")
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
        fart = skaler_fart(fart, Hastighetsmodus)

        
        # Rund av og send
        fart_r = round(fart, 2)
        vinkel_r = round(vinkel, 2)

        if rotasjon != 0.0:
            # Bruk maks fart for valgt modus, uavhengig av joystick-input
            if Hastighetsmodus == 1:
                fart_r = 0.1
            elif Hastighetsmodus == 2:
                fart_r = 0.4
            elif Hastighetsmodus == 3:
                fart_r = 0.5
            else:
                fart_r = 0.1  # fallback

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
                if abs(venstre_x) < 0.1 and abs(venstre_y) < 0.1 and abs(sving_js) < 0.1 and rotasjon == 0.0:
                    send_data(bus, 0.0, 0.0, rotasjon=0.0, sving_js=0.0)
                    aktiv_joystick = False
                    sist_sendte_fart = 0.0
                    sist_sendte_vinkel = 0.0


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