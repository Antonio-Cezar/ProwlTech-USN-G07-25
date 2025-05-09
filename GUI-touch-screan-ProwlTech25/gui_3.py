import customtkinter as ctk
from PIL import Image   # Bildehåndtering
import threading    # Prosesshåndtering
import bluetooth_dbus   # Bluetoothhåndtering
import subprocess   # Kjøring av eksterne script
import sys
import os

# Sti til mappe med eksterne script
script_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "Rasspberry-PI-4-scripts"))
sys.path.append(script_dir)

from get_can_data import receive_sensor_data    # Funksjon til å hente ut sensordata via CAN-bus
from can_menu_gui import CanMenuWindow  # Bruker egen klasse for CAN-menyen
from popup_window import PopupWindow    # Bruker egen klasse for popup-vinduer

# Importerer bilder og ikoner
from assets import  prowltech_logo, usn_logo, usn_logo_sort, info_icon, bluetooth_icon, bolt_icon, can_icon, cross_icon, loading_icon, menu_icon, sensor_icon, signal_icon, temp_icon, update_icon, warning_icon, start_icon, controller_pic



# Setter mørkt tema for hele GUI-et
ctk.set_appearance_mode("dark")

# Definerer farger brukt i designet for konsistens og enkel endring
text_color = "#FFFFFF"
background_color = "#0D0D1F"
top_panel_color = "#230F46"
error_section = "#2C161F"
frame_color = "#230F46"
frame_border_color = "#503C74"

button_color = "#595992"
button_border_color = "black"
button_hover_color = "#6C578A"

popup_background_color = "#200F2D"
popup_top_color = "#3A2557"
popup_button_color = "#6C3DAF"

# Definerer spesifikke størrelser 
border_size = 10
corner = 30
container_text_size = 14
button_corner = 40

# Hovedklasse for GUI-et
class ProwlTechApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("ProwlTech Kontrollpanel")           # Setter vindutittel
        self.geometry("800x480")                        # Setter størrelse 
        #self.attributes("-fullscreen", True)            # Fullskjerm på Raspberry Pi
        self.bind("<Escape>", self.exit_fullscreen)     # ESC lukker programmet
        #self.config(cursor="none")                      # Skjuler musepeker når GUI er i gang

        self.bluetooth_devices = []                      # Liste for bluetooth-enheter
        self.device_menu = None 
        self.connected_device = None
        self.device_widgets = {}  

        # Konfigurerer rutenett for strukturen

        # 3 rader
        self.grid_rowconfigure(0, minsize=100)
        self.grid_rowconfigure(1, weight=1)
        self.grid_rowconfigure(2, weight=1)

        # 3 kolonner
        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)
        self.grid_columnconfigure(2, weight=1)

        self.front_page()

#--------------------SEKSJONER-------------------------  
    def front_page(self):
        self.front_frame = ctk.CTkFrame(self, fg_color="#D3BBFD")
        self.front_frame.grid(row=0, column=0, rowspan=4, columnspan=4, sticky="nsew")

        # Bilde av ProwlTech-logo
        self.logo_label = ctk.CTkLabel(self.front_frame, image=prowltech_logo, text="")
        self.logo_label.place(relx=0.5, rely=0.45, anchor="center")

        # Legger til USN-logo           
        self.logo_label = ctk.CTkLabel(self.front_frame, image=usn_logo_sort, text="")      
        self.logo_label.place(x=700, y=15) 

        # Start-knapp
        self.start_button = ctk.CTkButton(
            self.front_frame,
            text="Start",
            font=("Century Gothic", 20),
            width=150,
            height=40,
            fg_color="#714FB1",
            hover_color=button_hover_color,
            text_color="white",
            image=start_icon,
            compound="right",
            corner_radius=button_corner,
            command=self.open_control_panel
        )
        self.start_button.place(relx=0.5, rely=0.9, anchor="center")

        # Info-knapp
        self.info_button = ctk.CTkButton(
            self.front_frame,
            text="Info",
            font=("Century Gothic", 20),
            width=150,
            height=40,
            fg_color="#503C74",
            hover_color=button_hover_color,
            text_color="white",
            image=info_icon,
            compound="right",
            corner_radius=button_corner,
            command=self.open_info_window
        )
        self.info_button.place(relx=0.8, rely=0.9, anchor="center")

        # CAN_bus-knapp
        self.can_bus_button = ctk.CTkButton(
            self.front_frame,
            text="CAN-meny",
            font=("Century Gothic", 20),
            width=150,
            height=40,
            fg_color="#503C74",
            hover_color=button_hover_color,
            text_color="white",
            image=can_icon,
            compound="right",
            corner_radius=button_corner,
            command=self.run_can_script
        )
        self.can_bus_button.place(relx=0.2, rely=0.9, anchor="center")

        # Avslutt-knapp
        self.exit_button = ctk.CTkButton(
            self.front_frame,
            text="Avslutt",
            font=("Century Gothic", 20),
            width=130,
            height=40,
            fg_color="#503C74",
            hover_color=button_hover_color,
            text_color="white",
            image=cross_icon,
            compound="right",
            corner_radius=button_corner,
            command=self.on_closing
        )
        self.exit_button.place(relx=0.1, rely=0.1, anchor="center")

    # Toppseksjon: logo, tittel og knapper
    def top_section(self):
        self.top_frame = ctk.CTkFrame(self, fg_color=top_panel_color, height=100)
        self.top_frame.grid(row=0, column=0, columnspan=3, sticky="nsew")
        self.top_frame.grid_propagate(False)

        # Legger til USN-logo           
        self.logo_label = ctk.CTkLabel(self.top_frame, image=usn_logo, text="")      
        self.logo_label.place(x=700, y=15)                                               

        # Titteltekst i midten av toppseksjonen
        self.title_label = ctk.CTkLabel(
            self.top_frame, 
            text="ProwlTech\nKontrollpanel",
            font=("Helvetica", 35),
            text_color="#E4E4E4",
            justify="center",
            anchor="center"
        )
        self.title_label.place(relx=0.5, rely=0.5, anchor=ctk.CENTER)

        # Hjem-knapp
        self.home_button = ctk.CTkButton(
            self.top_frame,
            width=85,
            height=40,
            text="Hjem",
            font=("Century Gothic", 18),
            text_color="white",
            image=menu_icon,
            compound="right",
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=button_corner,
            command=self.front_page
        )
        self.home_button.grid(row=0, column=0, padx=60, pady=30)

    # Bunnseksjon: viser batteri, tilkobling, sensor og feilmeldinger
    def bot_section(self):
        self.bot_frame = ctk.CTkFrame(self, fg_color=background_color)
        self.bot_frame.grid(row=1, column=0, columnspan=3, sticky="nsew")
        #self.mid_frame.grid_propagate(False)

        self.bot_frame.grid_rowconfigure(0, weight=3)  # for batteri/sensor/tilkobling
        self.bot_frame.grid_rowconfigure(1, weight=1)  # for feilmeldinger
        self.bot_frame.grid_columnconfigure((0, 1, 2), weight=1)

        # Batteristatus----------------------------------------------
        self.battery_container = ctk.CTkFrame(self.bot_frame, fg_color=background_color)
        self.battery_container.grid(row=0, column=0, padx=(30, 5), pady=10, sticky="nsew")
        self.battery_container.grid_propagate(False)

        # Batteristatus - Titteltekst
        self.battery_label = ctk.CTkLabel(
            self.battery_container, 
            text="BATTERISTATUS  ",
            font=("Century Gothic", container_text_size),
            image=bolt_icon,
            compound="right",
            text_color=text_color,
            justify="left"
        )
        self.battery_label.pack(side="top", anchor="w", padx=40, pady=(40, 0))

        # Batteristatus - Ramme
        self.battery_frame = ctk.CTkFrame(self.battery_container, height=150, width=150, fg_color=frame_color, corner_radius=corner, border_color=frame_border_color, border_width=border_size)
        self.battery_frame.pack(side="top", expand=True, padx=10, pady=(0, 30))
        self.battery_frame.pack_propagate(False)

        # Batteristatus - Innhold
        self.battery_status = ctk.CTkLabel(
            self.battery_frame,
            text="- %",
            font=("Century Gothic", 18),
            text_color="white",
            anchor="center",
            justify="left"
        )
        self.battery_status.pack(expand=True)

        # Tilkoblingsstatus:----------------------------------------------
        self.connection_containter = ctk.CTkFrame(self.bot_frame, fg_color=background_color)
        self.connection_containter.grid(row=0, column=1, padx=5, pady=10, sticky="nsew")
        self.connection_containter.grid_propagate(False)

        # Tilkoblingsstatus - Titteltekst
        self.connection_label = ctk.CTkLabel(
            self.connection_containter, 
            text="TILKOBLINGSSTATUS  ",
            font=("Century Gothic", container_text_size),
            image=signal_icon,
            compound="right",
            text_color=text_color,
            justify="left"
        )
        self.connection_label.pack(side="top", anchor="w", padx=35, pady=0)

        # Tilkoblingsstatus - Ramme
        self.connection_frame = ctk.CTkFrame(self.connection_containter, height=250, width=225, fg_color=frame_color, corner_radius=corner, border_color=frame_border_color, border_width=border_size)
        self.connection_frame.pack(side="top", expand=True, padx=5, pady=(0, 10))
        self.connection_frame.pack_propagate(False)

        # Tilkoblingsstatus - Innhold
        self.connection_status = ctk.CTkLabel(
            self.connection_frame,
            text="Ingen kontroller tilkoblet",
            font=("Century Gothic", 16),
            text_color="white",
            anchor="center",
            justify="left"
        )
        self.connection_status.pack(expand=True)

        # Knapp for å koble til kontroller (åpner nytt vindu)
        self.connect_button = ctk.CTkButton(
            self.connection_frame,
            width=35,
            height=35,
            text="Koble til kontroller",
            image=bluetooth_icon,
            compound="right",                          
            font=("Century Gothic", 14),
            fg_color=button_color,
            hover_color=button_hover_color,
            text_color="white",
            corner_radius=button_corner,
            command=self.open_connection_window
        )
        self.connect_button.pack(pady=(5, 20))


        # Sensormåling:----------------------------------------------
        self.sensor_container = ctk.CTkFrame(self.bot_frame, fg_color=background_color)
        self.sensor_container.grid(row=0, column=2, padx=(20, 10), pady=10, sticky="nsew")
        self.sensor_container.grid_propagate(False)

        # Sensormåling - Tittel
        self.sensor_label = ctk.CTkLabel(
            self.sensor_container, 
            text="SENSORSTATUS  ",
            font=("Century Gothic", container_text_size),
            image=sensor_icon,
            compound="right",
            text_color=text_color,
            justify="left"
        )
        self.sensor_label.pack(side="top", anchor="w", padx=35, pady=(20, 0))

        # Sensormåling - Ramme
        self.sensor_frame = ctk.CTkFrame(self.sensor_container, height=180, width=250, fg_color=frame_color, corner_radius=corner, border_color=frame_border_color, border_width=border_size)
        self.sensor_frame.pack(side="top", expand=True, padx=5, pady=(0, 20))
        self.sensor_frame.pack_propagate(False)

        # Sensormåling - Innhold
        self.sensor_value_label = ctk.CTkLabel(
            self.sensor_frame,
            text="Ingen data",
            font=("Century Gothic", 18),
            text_color="white"
        )
        self.sensor_value_label.pack(expand=True)

        # Feilmeldinger:----------------------------------------------
        self.error_container = ctk.CTkFrame(self.bot_frame, fg_color=background_color)
        self.error_container.grid(row=1, column=0, columnspan=3, padx=10, pady=5, sticky="nsew")

        # Feilmeldinger - Tittel
        self.error_label = ctk.CTkLabel(
            self.error_container, 
            text="FEILMELDINGER  ",
            font=("Century Gothic", container_text_size),
            image=warning_icon,
            compound="right",
            text_color=text_color,
            justify="left"
        )
        self.error_label.pack(side="top", anchor="w", padx=35, pady=0)
        

        # Feilmeldinger - Ramme
        self.error_frame = ctk.CTkFrame(self.error_container, height=100, width=750, fg_color=error_section, corner_radius=30)
        self.error_frame.pack(expand=True)
        self.error_frame.pack_propagate(False)

        # Feilmeldinger - Tekstboks
        self.error_textbox = ctk.CTkTextbox(
            self.error_frame,
            font=("Century Gothic", 14),
            text_color="white",
            fg_color=error_section,
            wrap="word"
        )
        self.error_textbox.pack(expand=True, fill="both", padx=20, pady=5)
        self.error_textbox.insert("end", "Ingen feilmeldinger.\n")
        self.error_textbox.configure(state="disabled")

#--------------------POPUP-VINDUER------------------------- 
    # Åpner koble_til_kontroller-popup
    def open_connection_window(self):
        self.popup = PopupWindow(self, title="Enheter")

        # Knapp for å oppdatere søket etter tilgjengelige kontrollere
        self.update_button = ctk.CTkButton(
            self.popup.top,
            text="Søk",
            width=120,
            height=40,
            image=update_icon,
            compound="right",  
            font=("Century Gothic", 16),
            fg_color=popup_button_color,
            hover_color=button_hover_color,
            text_color="white",
            corner_radius=button_corner,
            command=self.start_update
        )
        self.update_button.place(relx=0.05, rely=0.5, anchor="w")

        

    # Åpner info-popup
    def open_info_window(self):
        self.popup = PopupWindow(self, title="Informasjon")

        self.info_content_frame = ctk.CTkFrame(self.popup.bottom, fg_color="transparent")
        self.info_content_frame.pack(padx=30, pady=20)

        # Info-tekst
        self.info_label = ctk.CTkLabel(
            self.info_content_frame,
            text="Denne bilen styres via en trådløs kontroller.\nStatus og feilmeldinger vises i kontrollpanelet.\n\nFør bilen skal kjøre: \n1. Koble til en kontroller via knappen i kontrollpanelet.   \n2. Sørg for at CAN-bus er aktiv.",
            font=("Century Gothic", 16),
            text_color="white",
            justify="left"
        )
        self.info_label._label.configure(wraplength=700)
        self.info_label.pack(padx=10, pady=40)

        # Kontrollerbilde
        self.controller_label = ctk.CTkLabel(
            self.info_content_frame,
            image=controller_pic,
             text=""
        )
        self.controller_label.pack(padx=10, pady=0)
        self.controller_label.pack_forget() # Skjuler bildet

        # Funksjon som bytter mellom teksten og bildet
        def toggle_image():
            if self.controller_label.winfo_ismapped():
                self.controller_label.pack_forget()
                self.info_label.pack(padx=10, pady=20)
                self.show_controller.configure(text="Kontrollerfunksjoner")
            else:
                self.info_label.pack_forget()
                self.controller_label.pack(padx=10, pady=0)
                self.show_controller.configure(text="Info")

        # Knapp for å åpne kontroller-layout
        self.show_controller = ctk.CTkButton(
            self.popup.top,
            text="Kontrollerfunksjoner",
            width=120,
            height=40,
            font=("Century Gothic", 16),
            fg_color=popup_button_color,
            hover_color=button_hover_color,
            text_color="white",
            corner_radius=button_corner,
            command=toggle_image
        )
        self.show_controller.place(relx=0.05, rely=0.5, anchor="w")
        


#--------------------KOBLE TIL KONTROLLER-------------------------  
    # Starter søkeprosess 
    def start_update(self):
        self.update_button.configure(text="Søker...", state="disabled")     # Teksten på knappen endrer seg og knappen blir deaktivert under søk

        # Progressbar
        self.progress = ctk.CTkProgressBar(self.popup.bottom, progress_color=frame_color)   # Oppretter progressbar
        self.progress.pack(pady=10, padx=20, fill="x")
        self.progress.configure(mode="indeterminate")   # Skal ikke vise tid, bare animasjon
        self.progress.set(0)
        self.progress.start()   # Starter animasjon

        threading.Thread(target=self.scan_and_show).start() # Kjører skanning i egen tråd slik at GUI-et ikke fryser

    # Skanner etter enheter og henter resultater
    def scan_and_show(self):
        bluetooth_dbus.scan_devices()   # Starter skanning
        devices = bluetooth_dbus.get_devices()  # Henter resultatene 
        self.popup.bottom.after(0, lambda: self.show_devices(devices))  # Kjører show_devices() i GUI-tråd

    # Viser resultatene
    def show_devices(self, devices):
        self.progress.stop()    # Stopper progressbar
        self.progress.destroy()     # Sletter progressbar


        # Fjerner gamle synlige enheter om det er noen slik at det ikke overlapper
        for widget in self.popup.bottom.winfo_children():
                    widget.destroy()

        # Lager rader for hver enhet om det er funnet noen
        if devices:
            for name in devices.values():
                row = ctk.CTkFrame(self.popup.bottom, fg_color="#50256D", height=50, width=400, corner_radius=30)
                row.pack(fill="x", pady=5, padx=20)
                row.pack_propagate(False)

                # Navn på enhet
                name_label = ctk.CTkLabel(
                    row,
                    text=name,
                    text_color="white",
                    font=("Century Gothic", 14),
                    anchor="w"
                )
                name_label.pack(side="left", padx=(15, 10))

                # Sjekker tilkoblingsstatus til enhet
                is_connected = self.connected_device == name
                status_label = ctk.CTkLabel(
                    row,
                    text="Tilkoblet" if is_connected else "",
                    text_color="green" if is_connected else "white",
                    font=("Century Gothic", 14)
                )
                status_label.pack(side="left", padx=(5, 20))

                # Koble til/fra-knapp
                btn = ctk.CTkButton(
                    row,
                    text="Koble fra" if is_connected else "Koble til",
                    font=("Century Gothic", 16),
                    width=90,
                    height=30,
                    fg_color=popup_button_color,
                    hover_color=button_hover_color,
                    text_color="white",
                    corner_radius=button_corner,
                    command=lambda n=name: self.toggle_connection(n)
                )
                btn.pack(side="right", padx=(10, 15))

                self.device_widgets[name] = {
                    "button": btn,
                    "status": status_label
                }

        # Om ingen enheter blir funnet
        else:
            no_devices_label = ctk.CTkLabel(
                self.popup.bottom,
                text="Ingen enheter funnet",
                text_color="white",
                font=("Century Gothic", 16)
            )
            no_devices_label.pack(pady=10)

        self.update_button.configure(text="Søk", state="normal")   # Gjør det mulig å trykke på søke-knappen igjen

    def toggle_connection(self, name):

        # Finner tilhørende status på enheten som ble trykket på
        widgets = self.device_widgets.get(name)
        if not widgets:
            return
        
        button = widgets["button"]
        status_label = widgets["status"]

        # Hvis enheter allerede er koblet til. Funksjon for å koble fra enhet
        if self.connected_device == name:
            success = bluetooth_dbus.disconnect_from_device(name)

            # Kobler fra enhet og endrer knapp
            if success:
                self.connected_device = None
                button.configure(text="Koble til")
                status_label.configure(text="", text_color="white")
                self.connection_status.configure(text="Ingen kontroller tilkoblet", text_color="white")
                print("Koblet fra")

            else:
                print("Klarte ikke koble fra")

        # Hvis enheten ikke er koblet til. Funksjon til å koble til enhet
        else:
            success = bluetooth_dbus.connect_to_device(name)

            # Kobler til enhet og endrer knapp
            if success:
                self.connected_device = name
                button.configure(text="Koble fra")
                status_label.configure(text="Tilkoblet", text_color="green")
                self.connection_status.configure(text=f"Kontroller: Tilkoblet \n\n {name}", text_color="white")
                print("Tilkobling fullført")

            else:
                self.connection_status.configure(text=f"Ingen kontroller tilkoblet", text_color="red")
                self.log_error(f"Klarte ikke koble til {name}")     # Logger feilmelding
                print("Tilkobling mislykkes")

        #self.start_update()

#--------------------SENSORDATA-------------------------  

    # Leser CAN-data kontinuerlig i bakgrunnen
    def get_sensor_data(self):
            no_data_logged = False  # Unngå spam av samme feilmelding
            can_error_logged = False    # Unngå spam av CAN-feilmelding

            # Så lenge programmet kjører blir data hentet fra CAN-bus
            while self.running:
                val = receive_sensor_data() # Funksjonen som leser CAN-melding

                # Sjekker om CAN-bus er inaktiv
                if val == "CAN_INACTIVE":
                    self.sensor_value = "--"
                    if not can_error_logged:
                        self.log_error("CAN-bus er ikke aktiv. Sjekk at can0 er oppe. ")    # Logger feilmelding
                        can_error_logged = True
                    continue

                # Sjekker om data er mottatt
                if val is not None:
                    self.sensor_value = val # Lagrer mottatt sensordata
                    no_data_logged = False  # Tilbakestill hvis data er ok

                # Hvis CAN-bus er aktiv men får ikke inn data
                else:
                    self.sensor_value = "__"
                    if not no_data_logged:
                        self.log_error("Ingen data mottatt fra sensor (CAN).")  # Logger feilmelding
                        no_data_logged = True

    # Viser sensorstatus i GUI
    def update_sensor_display(self):
        if isinstance(self.sensor_value, dict):
            text = "\n".join(
            [f"{key}: {'Aktiv' if val else 'Inaktiv'}" for key, val in self.sensor_value.items()]
        )
        else:
            text = "Sensorverdi: --"

        self.sensor_value_label.configure(text=text)
        self.after(500, self.update_sensor_display)

#--------------------ANDRE FUNKSJONER-------------------------  
    # Funksjon: Avslutte programmet (ESC)
    def exit_fullscreen(self, event=None):
        self.destroy()

    def on_closing(self):
        self.running = False
        self.destroy()
       
    def run_can_script(self):
        #subprocess.Popen(["bash", "../Rasspberry-PI-4-scripts/meny_velger.sh"])
        print("Knappen ble trykket")
        CanMenuWindow(self)

    # Funksjon: Åpne kontrollpanel
    def open_control_panel(self):
        self.front_frame.destroy()

        self.top_section()
        self.bot_section()

        self.running = True
        self.sensor_value = "__"
        threading.Thread(target=self.get_sensor_data, daemon=True).start()
        self.update_sensor_display()

        self.log_error("Test: Kontrollpanelet ble åpnet.")


    # Logger feilmeldinger
    def log_error(self, message: str):
        self.error_textbox.configure(state="normal")
        self.error_textbox.insert("end", f"{message}\n")
        self.error_textbox.see("end")   # Scroller til bunnen
        self.error_textbox.configure(state="disabled")

# Starter GUI-applikasjon
app = ProwlTechApp()
app.protocol("WM_DELETE_WINDOW", app.on_closing)
app.mainloop()