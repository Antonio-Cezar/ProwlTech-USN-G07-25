USE_MOCK = False # Sett til False når Linux brukes 

# ---------- Mock-versjon for å teste i windows
if USE_MOCK:
    import time

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

# ---------- Ekte versjon for Linux
else:
    import can

    MSG_ID = 0x1  # Fra Zephyr-definisjonen
    CHANNEL = 'can0'  # Standard SocketCAN-grensesnitt på Raspberry Pi

    try:
        bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan')
    except OSError as e:
        print(f"Kunne ikke åpne CAN-bus: {e}")
        bus = None

    def receive_sensor_data():
        msg = bus.recv(timeout=1.0)
        if msg and msg.arbitration_id == MSG_ID and msg.dlc == 5:
            print(f"[CAN] Mottok rådata: {msg.data.hex()}")
            # Du kan gjøre hva du vil med msg.data[0] til msg.data[4] her
            return {
                'Front': msg.data[0],
                'Right': msg.data[1],
                'Left': msg.data[2],
                'Rear': msg.data[3]
            }
        print("[CAN] Ingen gyldig melding mottatt")
        return None


