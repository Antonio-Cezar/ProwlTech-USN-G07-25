import pygame
import RPi.GPIO as GPIO
import time
import os
import spidev
from mcp2515 import MCP2515  # Import MCP2515 library (if you don't have it, you'll need to install it)

# GPIO setup for interrupt pin (GPIO 25)
GPIO.setmode(GPIO.BCM)
GPIO.setup(25, GPIO.IN)  # GPIO 25 is set to input to detect the interrupt

# MCP2515 CAN bus setup (Ensure you have a library for this like mcp2515 or similar)
spi = spidev.SpiDev()
spi.open(0, 0)  # Open SPI bus 0, device 0
mcp = MCP2515(spi)
mcp.set_bit_rate(500000)  # Set the CAN bus bitrate (adjust as necessary)
mcp.set_normal_mode()

def find_controller():
    pygame.joystick.quit()
    pygame.joystick.init()
    while pygame.joystick.get_count() == 0:
        print("Ingen kontroller funnet. Søker...")
        time.sleep(2)
        pygame.joystick.quit()
        pygame.joystick.init()
    return pygame.joystick.Joystick(0)

def send_can_message(button_index):
    """ Sends a message over CAN to indicate which button was pressed """
    # Create a CAN message with the button index as data
    data = [button_index]
    message = mcp.Message(id=0x01, data=data)  # You can change the ID to your desired message ID
    mcp.send_message(message)
    print(f"Sent CAN message with button index: {button_index}")

def open_terminal():
    os.system("x-terminal-emulator -e python3 -c 'print(\"Joystick Status Window\")'")

def main():
    pygame.init()
    open_terminal()

    while True:
        joystick = find_controller()
        joystick.init()

        # Print joystick information when it starts
        print(f"Kontroller: {joystick.get_name()} tilkoblet.")
        print("Antall knapper på kontrollen som er aktive:", joystick.get_numbuttons())
        print("Antall akser på kontrollen som er aktive:", joystick.get_numaxes())

        num_axes = joystick.get_numaxes()  # Get the number of axes on the joystick
        siste_akse = [0.0] * num_axes  # Store previous axis values
        terskel = 0.05  # Threshold for axis movement
        dødssone = 0.15  # Deadzone to avoid tiny movements

        try:
            while True:
                pygame.event.pump()

                if pygame.joystick.get_count() == 0:
                    print("Kontroller frakoblet. Søker igjen...")
                    break

                button_names = {
                    0: "A",  # Xbox button A
                    1: "B",  # Xbox button B
                    2: "X",  # Xbox button X
                    3: "Y",  # Xbox button Y
                    9: "LB",  # Left Bumper
                    10: "RB",  # Right Bumper
                }
                
                # Read button presses and send CAN messages
                for button_index in range(joystick.get_numbuttons()):
                    if joystick.get_button(button_index):
                        button_name = button_names.get(button_index, f"Button {button_index}")
                        print(f"Button {button_index} pressed")
                        send_can_message(button_index)  # Send CAN message with the button index

                # Reading joystick axes values
                for i in range(num_axes):  # Iterate over all axes
                    try:
                        akse_verdi = joystick.get_axis(i)  # Get current axis value (-1 to 1)
                        if abs(akse_verdi) > dødssone and abs(akse_verdi - siste_akse[i]) > terskel:
                            # Assign names to the axes
                            if i == 0:
                                navn = "Venstre joystick, Horisontal"
                            elif i == 1:
                                navn = "Venstre joystick, Vertikal"
                            elif i == 2:
                                navn = "Høyre joystick, Horisontal"
                            elif i == 3:
                                navn = "Høyre joystick, Vertikal"
                            elif i == 4:
                                navn = "LT (Venstre trigger)"
                            elif i == 5:
                                navn = "RT (Høyre trigger)"
                            else:
                                navn = f"Akse {i}"

                            print(f"{navn} (akse {i}): {akse_verdi:.2f}", flush=True)
                            siste_akse[i] = akse_verdi
                    except Exception as e:
                        print(f"Feil ved lesing av aksen {i}: {e}", flush=True)

                time.sleep(0.05)

        except KeyboardInterrupt:
            print("Avslutter...")
            break

    GPIO.cleanup()
    pygame.quit()

if __name__ == "__main__":
    main()
