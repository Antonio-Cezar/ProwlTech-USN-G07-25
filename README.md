# ProwlTech
Bachelorgruppe 07 har forbedret en Mecanum-drevet bil med omnidireksjonell bevegelse ved å oppgradere både maskinvaren og programvaren. Ved hjelp av Zephyr på en Micro:bit ble en Raspberry Pi integrert som brukergrensesnitt. Årets ABC-krav ble oppfylt ved å forberede systemet for fremtidig autonom funksjonalitet gjennom modulær design og klargjøring.


# ABC-kravene:
For å sikre at systemet dekker de nødvendige funksjonene innenfor datadelen av prosjektet, ble det utarbeidet en kravliste. Disse kravene omhandler alt fra bevegelighet og brukergrensesnitt til sensor- og kommunikasjonsfunksjoner. Tabellen viser en oversikt over kravene for data, rangert
etter prioritet.

![alt text](img/IMG_7309.PNG)


# ProwlTech ferdigstilte produkt:

![alt text](img/ph.bilv2.jpg)


# Komponenter og tilkobling på kjøretøyet:

Fordi systemet er bygd opp av flere komponenter, såa brukes CAN-bus-protokollen til å kommunisere med hverandre. 

• Motorkontrollene bruker VESC-tools til å kommunisere med hverandre via et eget CAN-bus-system. ´En motorkontroller fungerer som master-slave og de andre fire er slave.

• Raspberry Pi og de tre microbit-ene forbindes med hverandre med et eget CAN-bus-system som er utviklet av en tidligere bachelorgruppe og utvikles videre på i år.

• GUI-enheten består av en Raspberry Pi koblet til en touchskjerm, som gir sanntidsinformasjon og kontrollmuligheter.

• Strømforsyningen er distribuert via en 12V-regulator for motorer og en 5V-regulator for Raspberry Pi og microbit-ene.

• Sensorer er strategisk plassert for å registrere omgivelsesdata.

![alt text](img/Bil%20og%20komponent%20modell-Data%20v5.jpg)


# Data komponenter:

![alt text](img/Bil%20og%20komponent%20modell-boks%20v2.jpg)


# CAN-BUS kommunikasjons protokoll:

![alt text](img/Komponent%20kobling%20på%20CAN-bus%20v9.jpg)


# Mobilitet og kjøreretninge.

Kjøretøyet skal ha et avansert bevegelsessystem basert p˚a Mecanum-hjul, som skal gi flere kjørealternativer.
Ved at man kan kontrollere hvert hjul individuelt, kan kjøretøyet bevege seg i følgende retninger.
Figuren viser visuelt hvordan dette fungerer

![alt text](img/Modell%20retning%20V2.jpg)


# Kontroller:

![alt text](img/Controller%20og%20kjøreretninger%20v3.jpg)


# GUI:

![alt text](img/GUI_wireframe_3.png)



![alt text](img/GUI_modell_4.png)