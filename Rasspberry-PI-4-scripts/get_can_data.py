"""

get_can_data.py

Henter og tolker sensordata via CAN-bus.
Inkluderer en mock-versjon for bruk i Windows.

Brukes i bakgrunnen av GUI-et for å vise sanntidsdata.

"""

import platform
import time

# --------------------- Mock-versjon for å teste i windows
if platform.system() != "Linux":
    print("Kjører i Windows")

    # Simulerer sensorverdier 
    def receive_sensor_data():
        time.sleep(1)
        byte = 1

        sensor_states = {
            'Foran': (byte >> 0) & 1,
            'Høyre': (byte >> 1) & 1,
            'Venstre':  (byte >> 2) & 1,
            'Bak':  (byte >> 3) & 1
        }
        print(f"[MOCK] Mottatt sensorstatus: {sensor_states}")
        return sensor_states

# -------------------- Ekte versjon for Linux
else:
    import can

    MSG_ID = 0x1        # Forventet melding-ID fra CAN
    CHANNEL = 'can0'    # Standard grensesnitt på Raspberry Pi (SocketCAN)

    # Forsøker å åpne CAN-bussen
    try:
        bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan')
    except OSError as e:
        print(f"Kunne ikke åpne CAN-bus: {e}")
        bus = None

    def receive_sensor_data():
        if bus is None:
            return "CAN_INACTIVE"

        msg = bus.recv(timeout=1.0)     # Lytter i maks 1 sekund for melding

        # Sjekker om mottatt melding er gyldig og inneholder det som forventes
        if msg and msg.arbitration_id == MSG_ID and msg.dlc >= 1:       # msg: sjekker om melding ble mottatt. // msg.arbitration_id == MSG_ID: sjekker om det er riktig ID. // msg.dlc >= 1: sjekker om det er minst 1 byte.
            byte = msg.data[0]          # Leser første byte i CAN-meldingen
            sensor_states = {
                'Front': (byte >> 0) & 1,
                'Right': (byte >> 1) & 1,
                'Left':  (byte >> 2) & 1,
                'Rear':  (byte >> 3) & 1
            }
            print(f"[CAN] Mottatt sensorstatus: {sensor_states}")
            return sensor_states

        #print("[CAN] Ingen gyldig melding mottatt")
        return None



