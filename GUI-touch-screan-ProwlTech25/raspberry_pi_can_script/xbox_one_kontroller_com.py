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
        
        # Lesing av A-knappen (button 0 på Xbox-kontroller)
        if joystick.get_button(0):
            print("A-knappen er trykket")
            GPIO.output(18, GPIO.HIGH)
        else:
            GPIO.output(18, GPIO.LOW)

            # Lesing av A-knappen (button 0 på Xbox-kontroller)
        if joystick.get_button(1):
            print("B-knappen er trykket")
            GPIO.output(18, GPIO.HIGH)
        else:
            GPIO.output(18, GPIO.LOW)

        right_stick_x = joystick.get_axis(3)
        right_stick_y = joystick.get_axis(4)

        left_stick_x = joystick.get_axis(3)
        left_stick_y = joystick.get_axis(4)

        # Test print for å sjekke verdiene.
        if a_pressed:
            print("A-knappen er trykket")
        if b_pressed:
            print("B-knappen er trykket")

        print(
            f"Venstre stikke: x={left_stick_x:.2f}, y={left_stick_y:.2f} | "
            f"Høyre stikke: x={right_stick_x:.2f}, y={right_stick_y:.2f}")
        
        time.sleep(0.1)

except KeyboardInterrupt:
    print("Avslutter...")
    
finally:
    GPIO.cleanup()
    pygame.quit()
