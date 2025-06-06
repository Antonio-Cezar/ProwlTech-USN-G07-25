# ProwlTech
Bachelorgruppe 07 har forbedret en Mecanum-drevet bil med omnidireksjonell bevegelse ved å oppgradere både maskinvaren og programvaren. Ved hjelp av Zephyr på en Micro:bit ble en Raspberry Pi integrert som brukergrensesnitt. Årets ABC-krav ble oppfylt ved å forberede systemet for fremtidig autonom funksjonalitet gjennom modulær design og klargjøring.


# ABC-kravene
For å sikre at systemet dekker de nødvendige funksjonene innenfor datadelen av prosjektet, ble det utarbeidet en kravliste. Disse kravene omhandler alt fra bevegelighet og brukergrensesnitt til sensor- og kommunikasjonsfunksjoner. Tabellen viser en oversikt over kravene for data, rangert
etter prioritet.

![alt text](img/IMG_7309.PNG)


# ProwlTech ferdigstilte produkt

![alt text](img/ph.bilv2.jpg)


# Komponenter og tilkobling på kjøretøyet

Fordi systemet er bygd opp av flere komponenter, såa brukes CAN-bus-protokollen til å kommunisere med hverandre. 

• Motorkontrollene bruker VESC-tools til å kommunisere med hverandre via et eget CAN-bus-system. ´En motorkontroller fungerer som master-slave og de andre fire er slave.

• Raspberry Pi og de tre microbit-ene forbindes med hverandre med et eget CAN-bus-system som er utviklet av en tidligere bachelorgruppe og utvikles videre på i år.

• GUI-enheten består av en Raspberry Pi koblet til en touchskjerm, som gir sanntidsinformasjon og kontrollmuligheter.

• Strømforsyningen er distribuert via en 12V-regulator for motorer og en 5V-regulator for Raspberry Pi og microbit-ene.

• Sensorer er strategisk plassert for å registrere omgivelsesdata.

![alt text](img/Bil%20og%20komponent%20modell-Data%20v5.jpg)


# Kommunikasjonsflyt
For å få et overblikk av informasjon utvekslingen mellom komponentene og hvilken typer programvarer som ligger i de ulike komponentene så er det laget et kommunikasjonsflyt diagram (se Figur \ref{fig:Kommunikasjonsflyt-diagram}). I diagrammet kan man se komponenten i de grønne boksene og de ulike typer koder og funksjonaliteter de har. 

De grønne boksene representerer komponentene. Raspberry Pi prosesserer flere ulike funksjoner samtidig. Den har tre programmer der den ene er en CAN-bus kode som initialiserer og enheten \textit{can0} som er for modulen (MCP2515). Den har også en utviklet canbus-meny som skal gjøre det enklere å se status, starte på nytt kommunikasjonen og se \textit{CAN-bus-dump} som er trafikken og er nærmere forklart \textit{i kapittel} \ref{sec:CAN_meny} hvordan denne fungerer.

Det er også et Bluetooth-kontroller Python-skript som behandler og sender bevegelighetsverdier videre til motor-microbit. Det har også blitt lagt til en funksjonalitet for avspilling av en tone ved knappetrykk og forklares nærmere \textit{i kapittel} \ref{sec:Tredjepart_Bluetooth_kontroller}. 

Siste utviklet program på Raspberry Pi er GUI som forklares \textit{i kapittel} \ref{sec:Grafisk_brukergrensesnitt}. Dette er en visuell og interaktiv måte for å gi operatør av bilen et styrings panel for bilen. GUI som er utviklet har funksjonaliteter tilpasset behov for en operatør av bil som å kunne se status på batteri, hente ulike informasjoner og vise på vise det og koble til Bluetooth-kontroller.

Siste tre microbit-ene i diagrammet håndterer forskjellige deler av systemet. Det er en for sensor, motorstyring og COM. COM-microbit-en ble brukt før for tilkobling av en gammel kontroller, men denne ble byttet ut på grunn av manglende funksjonaliteter som forklares \textit{i kapittelet} \ref{sec:Tredjepart_Bluetooth_kontroller}. For å ikke fjerne denne microbit-en helt fra system med tanke på andre mulige fremtidige oppgaver denne kan bli brukt for, så har den da ikke blitt fjernet fra systemet og heller lagt til en funksjonalitet for et lydsignal som forklares \textit{i kapittel} \ref{sec:com_microbit}. motormicrobit-en styrer EL-drivlinjene til bilen der det har blitt utviklet ny metode for styring som forklares \textit{i kapittel} \ref{sec:Motorstyring}. Til slutt så er det sensor-microbit som det ikke har blitt gjort noen nye endringer på og forblir den samme koden som ble utviklet av forrige bachelorgruppe.

![alt text](img/kommunikasjonsflyt.jpg)


# CAN-BUS kommunikasjons protokoll

![alt text](img/Komponent%20kobling%20på%20CAN-bus%20v9.jpg)


# Mobilitet og kjøreretninge.

Kjøretøyet skal ha et avansert bevegelsessystem basert p˚a Mecanum-hjul, som skal gi flere kjørealternativer.
Ved at man kan kontrollere hvert hjul individuelt, kan kjøretøyet bevege seg i følgende retninger.
Figuren viser visuelt hvordan dette fungerer

![alt text](img/Modell%20retning%20V2.jpg)


# Kontroller

![alt text](img/Controller%20og%20kjøreretninger%20v3.jpg)


# GUI

![alt text](img/GUI_wireframe_3.png)



![alt text](img/GUI_modell_4.png)