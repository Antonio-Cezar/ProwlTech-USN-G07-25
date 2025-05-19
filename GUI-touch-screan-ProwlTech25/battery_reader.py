import serial
import time

def read_battery_percentage():
    try:
        # Åpner seriallport ttyS0 med 9600 bps
        with serial.Serial('/dev/ttyS0', baudrate=9600, timeout=1) as ser:
            print("Lytter på batteridata fra shunt...\n")

            while True:
                frame = ser.read(16)

                if len(frame) != 16:    # Leser nøyaktig 16 byte, hvis færre, hopp over
                    continue

                if frame[0] != 0xA5 or (sum(frame[0:15]) & 0xFF) != frame[15]:  # sum av byte skal stemme med checksum
                        continue 

                percent = frame[1]  # Parsing
                print(f"Batteriprosent: {percent}%")
                time.sleep(1)

    except Exception as e:
        print(f"Feil ved seriell tilkobling eller lesing: {e}")

if __name__ == "__main__":
    read_battery_percentage()