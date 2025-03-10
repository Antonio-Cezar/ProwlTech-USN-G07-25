import pygame
import RPi.GPIO as GPIO
import time

# Sett opp GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(18, GPIO.OUT)  # Sett opp pinnen du vil bruke

# Initialiser pygame
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
        
        # Eksempel: Les av A-knappen (button 0 på Xbox-kontroller)
        if joystick.get_button(0):
            print("A-knappen er trykket")
            GPIO.output(18, GPIO.HIGH)
        else:
            GPIO.output(18, GPIO.LOW)
        
        time.sleep(0.1)

except KeyboardInterrupt:
    print("Avslutter...")
    
finally:
    GPIO.cleanup()
    pygame.quit()
