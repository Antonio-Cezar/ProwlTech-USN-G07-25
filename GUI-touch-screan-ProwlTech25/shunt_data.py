"""

shunt_data.py

Henter data fra shunt og oversetter til leslig data.
Inkluderer mock-versjon for bruk i Windows.

"""


import platform
import time
import struct

#------------------------------------Mock-versjon for å teste på Windows-------------------------------------------
# Sjekker om koden kjøres på Windows eller Linux
if platform.system() != "Linux":
    print("Kjører i Windows")

    def get_battery_percent():
        print("Mock: 98%")
        return 98
    
#------------------------------------Ekte versjon på Linux-------------------------------------------
#   Skanner etter tilgjengelige enheter via Bluetooth, filtrerer bort enheter uten navn. Koble til og koble fra enheter. Henter all informasjon og returnerer til GUI-kode.
else:
    import serial

    def get_battery_percent():
        print("UART-tråd startet: Lytter på /dev/ttyS0 @ 9600")
        try:
            # Åpner seriallport ttyS0 med 9600 bps
            with serial.Serial('/dev/ttyS0', baudrate=9600, timeout=1) as ser:
                frame = ser.read(16)
                    
                if len(frame) != 16:    # Leser nøyaktig 16 byte, hvis færre, hopp over 
                    return None

                # Sjekker at rammen er gyldig 
                if frame[0] != 0xA5 or (sum(frame[0:15]) & 0xFF) != frame[15]:  # sum av byte skal stemme med checksum
                    return None

                # Parsing
                soc = frame[1]
                voltage = ((frame[2] << 8) | frame[3]) / 100.0  # Spenning: byte 2 og 3 (16-bit tall), del på 100 for volt
                capacity = struct.unpack('>I', frame[4:8])[0]   # Kapasitet (mAh) (32-bit tall)
                current = struct.unpack('>i', frame[8:12])[0]   # Strøm (mA) (32-bit tall)
                t = (frame[12] << 16) | (frame[13] << 8) | frame[14]    # Resterende tid i sekunder (leser 3 byte)
                hrs, rem = divmod(t, 3600)  # Timer og restsekunder
                mins, secs = divmod(rem, 60)    # Minutter og restsekunder
                remaining = f"{hrs:02d}:{mins:02d}:{secs:02d}"
                return soc

        except Exception as e:
            print(f"Feil ved UART: {e}")
            return None