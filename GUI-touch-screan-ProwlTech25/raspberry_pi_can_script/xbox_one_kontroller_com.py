import pygame
import RPi.GPIO as GPIO
import time

# GPIO set up pin
GPIO.setmode(GPIO.BCM)
GPIO.setup(25, GPIO.OUT)  # Sett opp pinnen du vil bruke

# Initialiser bruk av pygame
pygame.init()
pygame.joystick.init()

# Sjekk om en kontroller er tilkoblet
if pygame.joystick.get_count() == 0:
    print("Ingen kontroller funnet.")
    pygame.quit()
    exit()

joystick = pygame.joystick.Joystick(0)
joystick.init()

print(f"Kontroller: {joystick.get_name()} tilkoblet.")

try:
    while True:
        pygame.event.pump()
        
        # Lesing av A-knappen (button 1 på Xbox-kontroller (A))
        if joystick.get_button(1):
            print("A-knappen er trykket")
            GPIO.output(18, GPIO.HIGH)
        else:
            GPIO.output(18, GPIO.LOW)

        # Lesing av B-knappen (button 2 på Xbox-kontroller (B))
        if joystick.get_button(2):
            print("B-knappen er trykket")
            GPIO.output(18, GPIO.HIGH)
        else:
            GPIO.output(18, GPIO.LOW)

        # Lesing av X-knappen (button 3 på Xbox-kontroller (X))
        if joystick.get_button(3):
            print("X-knappen er trykket")
            GPIO.output(18, GPIO.HIGH)
        else:
            GPIO.output(18, GPIO.LOW)

        # Lesing av Y-knappen (button 4 på Xbox-kontroller (Y))
        if joystick.get_button(4):
            print("Y-knappen er trykket")
            GPIO.output(18, GPIO.HIGH)
        else:
            GPIO.output(18, GPIO.LOW)

        # Lesing av LB-knappen rotasjon (button 5 på Xbox-kontroller (LB))
        if joystick.get_button(5):
            print("LB-knappen er trykket")
            GPIO.output(18, GPIO.HIGH)
        else:
            GPIO.output(18, GPIO.LOW)

        # Lesing av RB-knappen rotasjon (button 6 på Xbox-kontroller (RB))
        if joystick.get_button(6):
            print("RB-knappen er trykket")
            GPIO.output(18, GPIO.HIGH)
        else:
            GPIO.output(18, GPIO.LOW)

        # Lesing av høyre joystick verdier (horisontal, venstre og høyre styring på bilen)
        right_stick_x = joystick.get_axis(3)
        right_stick_y = joystick.get_axis(4)

        # Lesing av venstre joystick verdier (svinging ved kjøring fremover og bakover)
        left_stick_x = joystick.get_axis(0)
        left_stick_y = joystick.get_axis(1)

        print(
            f"Venstre stikke: x={left_stick_x:.2f}, y={left_stick_y:.2f} | "
            f"Høyre stikke: x={right_stick_x:.2f}, y={right_stick_y:.2f}")
        
        time.sleep(0.1)

except KeyboardInterrupt:
    print("Avslutter...")
    
finally:
    GPIO.cleanup()
    pygame.quit()
