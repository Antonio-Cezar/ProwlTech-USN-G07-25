use_mock = True # Sett til False når Linux brukes 

# ---------- Mock-versjon for å teste i windows
if use_mock:
    import random 
    import time

    def receive_sensor_data():
        time.sleep(1)
        byte = random.choice([0, 1])

        sensor_states = {
            'Front': (byte >> 0) & 1,
            'Right': (byte >> 1) & 1,
            'Left':  (byte >> 2) & 1,
            'Rear':  (byte >> 3) & 1
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
                'Front': (byte >> 0) & 1,
                'Right': (byte >> 1) & 1,
                'Left':  (byte >> 2) & 1,
                'Rear':  (byte >> 3) & 1
            }
            print(f"[CAN] Mottatt sensorstatus: {sensor_states}")
            return sensor_states
        return None


