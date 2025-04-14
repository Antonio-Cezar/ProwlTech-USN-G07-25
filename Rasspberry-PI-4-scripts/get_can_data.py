use_mock = False # Sett til False når Linux brukes 

# ---------- Mock-versjon for å teste i windows
if use_mock:
    import random 
    import time

    def receive_sensor_data():
        time.sleep(1)
        return random.choice([0, 1])
    import can

# ---------- Ekte versjon for Linux
else:
    MSG_ID = 0x3  # Fra Zephyr-definisjonen
    CHANNEL = 'can0'  # Standard SocketCAN-grensesnitt på Raspberry Pi

    def receive_sensor_data():
        bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan')
        print(f"Lytter på CAN-ID {hex(MSG_ID)}...")
        
        while True:
            msg = bus.recv(timeout=1.0)
            if msg and msg.arbitration_id == MSG_ID and msg.dlc == 1:
                sensor_byte = msg.data[0]
                print(f"Mottatt sensorverdi: {sensor_byte}")
                return sensor_byte  # Du kan returnere eller sende dette til GUI


