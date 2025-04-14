import can

MSG_ID = 0x3
CHANNEL = 'can0'

try:
    bus = can.interface.Bus(channel=CHANNEL, bustype='socketcan')
except OSError as e:
    print(f"Kunne ikke åpne CAN-grensesnittet: {e}")
    bus = None

def receive_sensor_data():
    if bus is None:
        print("CAN-bus ikke tilgjengelig")
        return None

    msg = bus.recv(timeout=1.0)
    if msg and msg.arbitration_id == MSG_ID and msg.dlc == 1:
        sensor_byte = msg.data[0]
        print(f"Mottatt sensorverdi: {sensor_byte}")
        return sensor_byte
    return None


