import pygame # Importer pygame library
import RPi.GPIO as GPIO # Importer GPIO library
import time
import os

# GPIO set up pin
GPIO.setmode(GPIO.BCM)
GPIO.setup(25, GPIO.OUT)  # Sett opp pinnen du vil bruke

def find_controller():
    pygame.joystick.quit()
    pygame.joystick.init()
    while pygame.joystick.get_count() == 0:
        print("Ingen kontroller funnet. Søker...")
        time.sleep(2)
        pygame.joystick.quit()
        pygame.joystick.init()
    return pygame.joystick.Joystick(0)

def main():
    pygame.init()
    open_terminal()

    while True:
        joystick = find_controller()
        joystick.init()
        
        #Printer ut informasjonen på kontrollen når den starter opp
        print(f"Kontroller: {joystick.get_name()} tilkoblet.")
        print("Antall knapper på kontrollen som er aktive:", joystick.get_numbuttons())
        print("Antall akser på kontrollen som er aktive:", joystick.get_numaxes())
        
        num_axes = joystick.get_numaxes() # Henter alle aksene kontrollen har
        siste_akse = [0.0] * num_axes  # Lager en liste for å lagre den forrige verdien til aksen
        terskel = 0.05  # Terskelverdi for hvor mye aksen må endre seg før den kan skrives ut
        dødssone = 0.15  # Dødssone på 15% for å unngå små bevegelser
        
        try:
            while True:
                pygame.event.pump()
                
                if pygame.joystick.get_count() == 0:
                    print("Kontroller frakoblet. Søker igjen...")
                    break
                
                # Lesing av A-knappen (button 0 på Xbox-kontroller (A))
                if joystick.get_button(0):
                    print("A-knappen er trykket (Indeks 0)")
                    GPIO.output(25, GPIO.HIGH)
                else:
                    GPIO.output(25, GPIO.LOW)
                
                # Lesing av B-knappen (button 1 på Xbox-kontroller (B))
                if joystick.get_button(1):
                    print("B-knappen er trykket (Indeks 1)")
                    GPIO.output(25, GPIO.HIGH)
                else:
                    GPIO.output(25, GPIO.LOW)
                
                # Lesing av X-knappen (button 2 på Xbox-kontroller (X))
                if joystick.get_button(2):
                    print("X-knappen er trykket (Indeks 2)")
                    GPIO.output(25, GPIO.HIGH)
                else:
                    GPIO.output(25, GPIO.LOW)
                
                # Lesing av Y-knappen (button 3 på Xbox-kontroller (Y))
                if joystick.get_button(3):
                    print("Y-knappen er trykket (Indeks 3)")
                    GPIO.output(25, GPIO.HIGH)
                else:
                    GPIO.output(25, GPIO.LOW)
                
                # Lesing av LB-knappen rotasjon (button 9 på Xbox-kontroller (LB))
                if joystick.get_button(9):
                    print("LB-knappen er trykket (Indeks 9)")
                    GPIO.output(25, GPIO.HIGH)
                else:
                    GPIO.output(25, GPIO.LOW)
                
                # Lesing av RB-knappen rotasjon (button 10 på Xbox-kontroller (RB))
                if joystick.get_button(10):
                    print("RB-knappen er trykket (Indeks 10)")
                    GPIO.output(25, GPIO.HIGH)
                else:
                    GPIO.output(25, GPIO.LOW)
                
                # Leser joystick-aksene
                for i in range(num_axes): # Itererer over alle aksene, variabel i er indeks til hver akse
                    try:
                        akse_verdi = joystick.get_axis(i) # Henter nåværende akseverdi (mellom 1 og -1)
                        if abs(akse_verdi) > dødssone and abs(akse_verdi - siste_akse[i]) > terskel:
                            # Gir navn til aksene på kontrollen  
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
                
                # Lesing av høyre joystick verdier (horisontal, venstre og høyre styring på bilen)
                right_stick_x = joystick.get_axis(3)
                right_stick_y = joystick.get_axis(4)
                
                # Lesing av venstre joystick verdier (svinging ved kjøring fremover og bakover)
                left_stick_x = joystick.get_axis(0)
                left_stick_y = joystick.get_axis(1)
                
                time.sleep(0.05)
        
        except KeyboardInterrupt:
            print("Avslutter...")
            break
    
    GPIO.cleanup()
    pygame.quit()

if __name__ == "__main__":
    main()
