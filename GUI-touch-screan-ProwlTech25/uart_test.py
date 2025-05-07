import serial

try:
    with serial.Serial('/dev/ttyS0', baudrate=9600, timeout=2) as ser:
        print("Venter på data fra /dev/ttyS0 ... (2 sekunders timeouts)")
        while True:
            data = ser.read(16)
            if data:
                print("RAW BYTES:", data)
                print("HEX      :", data.hex())
                try:
                    print("TEKST    :", data.decode(errors='ignore'))
                except:
                    pass
            else:
                print("Ingen data mottatt...")
except Exception as e:
    print("Feil:", e)
