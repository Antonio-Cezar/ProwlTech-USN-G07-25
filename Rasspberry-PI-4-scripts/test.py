import can
import time

# VESC configuration
vesc_id = 15  # Must match VESC Tool
target_rpm = 3000
duration = 10  # seconds
COMM_SET_RPM = 0x05
can_id = 0x100 + vesc_id  # ✅ This is the FIX!

def create_rpm_packet(rpm):
    rpm_bytes = rpm.to_bytes(4, byteorder='big', signed=True)
    return bytes([COMM_SET_RPM]) + rpm_bytes

# Setup CAN interface
bus = can.interface.Bus(channel='can0', bustype='socketcan')

print(f"Sending {target_rpm} RPM to VESC ID {vesc_id} for {duration} seconds...")

start_time = time.time()
while time.time() - start_time < duration:
    msg = can.Message(arbitration_id=can_id,
                      data=create_rpm_packet(target_rpm),
                      is_extended_id=False)
    try:
        bus.send(msg)
    except can.CanError:
        print("CAN send failed")
    time.sleep(0.1)

# Stop motor
print("Stopping motor...")
stop_msg = can.Message(arbitration_id=can_id,
                       data=create_rpm_packet(0),
                       is_extended_id=False)
bus.send(stop_msg)

print("Motor stopped.")
