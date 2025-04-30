import serial
import time

def read_battery_percentage():
    try:
        with serial.Serial('/dev/ttyS0', baudrate=9600, timeout=1) as ser:
            print("Lytter på batteridata fra TF03K...\n")

            while True:
                frame = ser.read(16)

                if len(frame) != 16:
                    continue

                checksum = sum(frame[0:15]) & 0xFF
                if checksum != frame[15]:
                    print("Sjekksumfeil, hopper over.")
                    continue

                percent = frame[9]
                print(f"Batteriprosent: {percent}%")
                time.sleep(1)

    except Exception as e:
        print(f"Feil ved seriell tilkobling eller lesing: {e}")

if __name__ == "__main__":
    read_battery_percentage()