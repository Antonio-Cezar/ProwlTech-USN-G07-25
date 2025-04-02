import can
import struct
import time
import pygame


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
def send_data(bus, knapper, status, venstre_y, høyre_y, lt, rt):
    try:
        data = struct.pack ('<BBhhBB', knapper, status, venstre_y, høyre_y, lt, rt) # Pakken som sendes.
        melding = can.Message(arbitration_id=MSG_ID, data=data, is_extended_id=EXTENDED_ID)
        bus.send(melding)
        print("Sendt: ", knapper, status, venstre_y, høyre_y, lt, rt)

    except Exception as e:
        print("Feil ved sending! :", e)

# Funksjonen for å skalere joystick-verdiene fra(-1.0 ril 1.0) til int16
def skaler_trigger(verdi):
    # Fra -1.0 til 1.0 → 0 til 255
    return int((verdi + 1) * 127.5)

def skaler_akse(verdi):
    return int(verdi * 32767)




#main
def main():
    # Starter opp CAN-bus sending
    try:
        bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan', bitrate=BITRATE)
        print("CAN er på")
    except Exception  as e:
        print("FEIL: Kan IKKE på")
        return

    kontroller = None


    # Loop som registrerer verdier og sender verdier til pakke funsjonene (send_data)
    while True:
        # bibliotek pygame og kontroller
        pygame.init()
        pygame.joystick.init()

        if pygame.joystick.get_count() == 0:
            print("Ingen kontroller funnet. Søker igjen...")
            time.sleep(1)
            continue
        try:
            kontroller = pygame.joystick.Joystick(0)
            kontroller.init()
            print("Kontroller koblet til")
        except Exception as e:
            print("Feil ved tilkobling av kontroller:")
            time.sleep(1)
            continue


        # Hoved loop som regristrer kontroller verdier
        while True:
            pygame.event.pump() # Den oppdaterer interne verdier for kontrolleren (og andre input-enheter).

        try:
            # Joystick
            venstre_x = kontroller.get_axis(0)
            venstre_y = kontroller.get_axis(1)
            høyre_x = kontroller.get_axis(3)
            høyre_y = kontroller.get_axis(4)

            # Trigger
            lt = kontroller.get_axis(2)
            rt = kontroller.get_axis(5)

            # Knapper over skulder
            lb = kontroller.get_button(4)
            rb = kontroller.get_button(5)

            # Knapper a, b, x og y
            a = kontroller.get_button(0)
            b = kontroller.get_button(1)
            x = kontroller.get_button(2)
            y = kontroller.get_button(3)

            # Bitmaske for knapper
            knapper = 0
            if a: knapper |= 0b00000001
            if b: knapper |= 0b00000010
            if x: knapper |= 0b00000100
            if y: knapper |= 0b00001000
            if lb: knapper |= 0b00010000
            if rb: knapper |= 0b00100000

            # Status for joystick
            status = 0
            if abs(venstre_x) > 0.1 or abs(venstre_y) > 0.1:
                status |= 0b00000001  # Venstre joystick aktiv
            if abs(høyre_x) > 0.1 or abs(høyre_y) > 0.1:
                status |= 0b00000010  # Høyre joystick aktiv

            # Skaler verdier
            venstre_y_sca = skaler_akse(venstre_y)
            høyre_y_sca = skaler_akse(høyre_y)
            lt_sca = skaler_trigger(lt)
            rt_sca = skaler_trigger(rt)

            # Sjekk om det skal sende
            noe_aktivt = knapper > 0 or status > 0 or lt_sca > 0 or rt_sca > 0
            if noe_aktivt:
                send_data(bus, knapper, status, venstre_y_sca, høyre_y_sca, lt_sca, rt_sca)
            else:
                print("[IDLE] Ingen sending")

        except pygame.error:
            print("Kontroller frakoblet. Søker igjen...")
            break

        time.sleep(0.2)
#================================================================