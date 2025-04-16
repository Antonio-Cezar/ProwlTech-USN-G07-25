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

    MSG_ID = 0x3  # Fra Zephyr-definisjonen
    CHANNEL = 'can0'  # Standard SocketCAN-grensesnitt på Raspberry Pi

    try:
        bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan')
    except OSError as e:
        print(f"Kunne ikke åpne CAN-bus: {e}")
        bus = None

    def receive_sensor_data():
        if bus is None:
            print("CAN-bus ikke tilgjengelig")
            return None

        msg = bus.recv(timeout=1.0)
        if msg and msg.arbitration_id == MSG_ID and msg.dlc == 1:
            byte = msg.data[0]
            sensor_states = {
                'Foran': (byte >> 0) & 1,
                'Høyre': (byte >> 1) & 1,
                'Venstre':  (byte >> 2) & 1,
                'Bak':  (byte >> 3) & 1
            }
            print(f"[CAN] Mottatt sensorstatus: {sensor_states}")
            return sensor_states
        return None


