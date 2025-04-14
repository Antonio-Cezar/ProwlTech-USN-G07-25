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

    if msg and msg.arbitration_id == MSG_ID and msg.dlc == 4:
        distances = {
            'Front': msg.data[0],   # Avstand i cm fra sensor foran
            'Right': msg.data[1],   # Høyre
            'Left':  msg.data[2],   # Venstre
            'Rear':  msg.data[3]    # Bak
        }
        print(f"Mottatt avstander: {distances}")
        return distances
    return None



