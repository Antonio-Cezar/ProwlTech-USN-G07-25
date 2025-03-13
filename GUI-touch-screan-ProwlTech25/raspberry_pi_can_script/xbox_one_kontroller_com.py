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

#Printer ut informasjonen på kontrollen når den starter opp
print(f"Kontroller: {joystick.get_name()} tilkoblet.")
print("Antall knapper på kontrollen som er aktive:", joystick.get_numbuttons())
print("Antall akser på kontrollen som er aktive:", joystick.get_numaxes())

#lagrer forrige akseverdi for å unngå printing når verdi ikke endres
num_axes = joystick.get_numaxes() #henter alle aksene kontrollen har
siste_akse = [0.0] * joystick.get_numaxes() #lager en liste for å lagre den forrige verdien til aksen
terksel = 0.05 #lager en terskelverdi for hvor mye aksen må endre seg før den kan skrives ut.

try:
    while True:
        pygame.event.pump()
        
        # Lesing av A-knappen (button 0 på Xbox-kontroller (A))
        if joystick.get_button(0):
            print("A-knappen er trykket (Indeks 0)") #indeksnummer er pygame sitt referansenummer for knappen
            GPIO.output(25, GPIO.HIGH)
        else:
            GPIO.output(25, GPIO.LOW)

        # Lesing av B-knappen (button 1 på Xbox-kontroller (B))
        if joystick.get_button(1):
            print("B-knappen er trykket (indeks 1)") #indeksnummer er pygame sitt referansenummer for knappen
            GPIO.output(25, GPIO.HIGH)
        else:
            GPIO.output(25, GPIO.LOW)

        # Lesing av X-knappen (button 2 på Xbox-kontroller (X))
        if joystick.get_button(2):
            print("X-knappen er trykket (indeks 2)")
            GPIO.output(25, GPIO.HIGH)
        else:
            GPIO.output(25, GPIO.LOW)

        # Lesing av Y-knappen (button 3 på Xbox-kontroller (Y))
        if joystick.get_button(3):
            print("Y-knappen er trykket (indeks 3)")
            GPIO.output(25, GPIO.HIGH)
        else:
            GPIO.output(25, GPIO.LOW)

        # Lesing av LB-knappen rotasjon (button 9 på Xbox-kontroller (LB))
        if joystick.get_button(9):
            print("LB-knappen er trykket (indeks 9)")
            GPIO.output(25, GPIO.HIGH)
        else:
            GPIO.output(25, GPIO.LOW)

        # Lesing av RB-knappen rotasjon (button 10 på Xbox-kontroller (RB))
        if joystick.get_button(10):
            print("RB-knappen er trykket (indeks 10)")
            GPIO.output(25, GPIO.HIGH)
        else:
            GPIO.output(25, GPIO.LOW)

        #BRUKT FOR DEBUGGING AV KNAPPEINDEKS
        #print("Sjekker hvilken knapp som tilhører hvilken verdi på kontrollen", flush=True)
        #for i in range(joystick.get_numbuttons()): #for å iterere over alle knapper til kontrollen
            #try:
                #if joystick.get_button(i):
                    #print(f"Button {i} er trykket", flush=True)  #sjekker om en knapp er trykket og printer ut knappen
            #except Exception as e:
                #print(f"Feil ved lesing av button {i}: {e}", flush=True) #flush brukes for å printe ut med engang.

         #leser joystick-aksene
          for i in range(num_axes): #itererer over alle aksene, variabel i er indeks til hver akse
            try:
                akse_verdi = joystick.get_axis(i) #henter nåværende akseverdi (mellom 1 og -1)
                if abs(akse_verdi - siste_akse[i]) > terskel: #abs er absoluttverdi og koden sammenligner nåværende verdi og forrige og hvis forskjellen er større enn 0.05 printes det ut.
                    #Gir navn til aksene på kontrollen
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

                    print(f"{navn} (akse {i}: {akse_verdi:.2f}" flush=TRUE)  #skriver ut navn, indeks og verdi med 2desimaler
                    siste_akse[i] = akse_verdi #lagrer nye verdien for aksen i variabelen
            except Exception as e:
                print(f"Feil ved lesing av aksen {i}: {e}", flush=TRUE)  #skjer det en feil vil dette fange det opp og printe ut

        # Lesing av høyre joystick verdier (horisontal, venstre og høyre styring på bilen)
        right_stick_x = joystick.get_axis(3)
        right_stick_y = joystick.get_axis(4)

        # Lesing av venstre joystick verdier (svinging ved kjøring fremover og bakover)
        left_stick_x = joystick.get_axis(0)
        left_stick_y = joystick.get_axis(1)

        
        time.sleep(0.1)

except KeyboardInterrupt:
    print("Avslutter...")
    
finally:
    GPIO.cleanup()
    pygame.quit()
