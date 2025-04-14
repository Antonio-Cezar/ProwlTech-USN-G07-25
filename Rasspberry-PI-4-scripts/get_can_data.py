import can

MSG_ID = 0x3
CHANNEL = 'can0'

try:
    bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan')
except OSError as e:
    print(f"Kunne ikke åpne CAN-grensesnittet: {e}")
    bus = None

def receive_sensor_data():
    msg = bus.recv(timeout=1.0)
    if msg and msg.arbitration_id == MSG_ID and msg.dlc == 1:
        byte = msg.data[0]

        # Pakk ut hver bit i byte'en som separate sensorer
        sensor_values = {
            'sensor_0': (byte >> 0) & 1,
            'sensor_1': (byte >> 1) & 1,
            'sensor_2': (byte >> 2) & 1,
            'sensor_3': (byte >> 3) & 1
        }

        print(f"Mottatt sensorverdier: {sensor_values}")
        return sensor_values
    return None



