import customtkinter as ctk
from PIL import Image
import threading
import bluetooth_dbus
import subprocess
import sys
import os
script_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "Rasspberry-PI-4-scripts"))
sys.path.append(script_dir)
from get_can_data import receive_sensor_data

from popup_window import PopupWindow
from assets import info_icon, bluetooth_icon, bolt_icon, can_icon, cross_icon, loading_icon, menu_icon, prowltech_logo, usn_logo, sensor_icon, signal_icon, temp_icon, update_icon, warning_icon, start_icon



# Setter mørkt tema for hele GUI-et
ctk.set_appearance_mode("dark")

# Definerer farger brukt i designet for konsistens og enkel endring
text_color = "#BDBDBD"
background_color = "#140520"
top_panel_color = "#2F0B63"
error_section = "#311C42"
frame_color = "#583092"
frame_border_color = "#3A0C7D"

button_color = "#4C1A92"
button_border_color = "black"
button_hover_color = "#6C578A"

popup_background_color = "#200F2D"
popup_top_color = "#3A2557"
popup_button_color = "#6C3DAF"

border_size = 9

# Hovedklasse for GUI-et
class ProwlTechApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("ProwlTech Kontrollpanel")           # Setter vindutittel
        self.geometry("800x480")                        # Setter størrelse 
        #self.resizable(False,False)                     # Hindrer at vinduet kan skaleres
        #self.attributes("-fullscreen", True)            # Fullskjerm på Raspberry Pi
        self.bind("<Escape>", self.exit_fullscreen)     # ESC lukker programmet
        #self.config(cursor="none")                      # Skjuler musepeker når GUI er i gang

        self.bluetooth_devices = []                      # Liste for bluetooth-enheter
        self.device_menu = None 

        # Konfigurerer rutenett(grid) for layout
        self.grid_rowconfigure(0, minsize=130)
        self.grid_rowconfigure(1, weight=1)
        self.grid_rowconfigure(2, weight=1)
        self.grid_rowconfigure(3, weight=1)

        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)
        self.grid_columnconfigure(2, weight=1)
        self.grid_columnconfigure(3, weight=1)

        # Funksjoner som bygger seksjon
        self.front_page()

    # Startside
    def front_page(self):
        self.front_frame = ctk.CTkFrame(self, fg_color="#CDB9FC")
        self.front_frame.grid(row=0, column=0, rowspan=4, columnspan=4, sticky="nsew")

        # Bilde av ProwlTech-logo
        self.logo_label = ctk.CTkLabel(self.front_frame, image=prowltech_logo, text="")
        self.logo_label.place(relx=0.5, rely=0.45, anchor="center")

        # Start-knapp
        self.start_button = ctk.CTkButton(
            self.front_frame,
            text="Start",
            font=("Century Gothic", 20),
            width=150,
            height=40,
            fg_color="#6F48A6",
            hover_color=button_hover_color,
            text_color="white",
            image=start_icon,
            compound="right",
            corner_radius=10,
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
            fg_color="#6F48A6",
            hover_color=button_hover_color,
            text_color="white",
            image=info_icon,
            compound="right",
            corner_radius=10,
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
            fg_color="#6F48A6",
            hover_color=button_hover_color,
            text_color="white",
            image=can_icon,
            compound="right",
            corner_radius=10,
            command=self.run_can_script
        )
        self.can_bus_button.place(relx=0.2, rely=0.9, anchor="center")

    def run_can_script(self):
        subprocess.Popen(["bash", "../Rasspberry-PI-4-scripts/canbus_meny.sh"])

    # Åpner info-popup
    def open_info_window(self):
        self.popup = PopupWindow(self, title="Informasjon")

        info_label = ctk.CTkLabel(
            self.popup.bottom,
            text="Kort beskrivelse av hvordan bilen fungerer.",
            font=("Century Gothic", 16),
            text_color="white"
        )
        self.popup.add_widget(info_label, padx=30, pady=20)

    # Åpner koble_til_kontroller-popup
    def open_connection_window(self):
        self.popup = PopupWindow(self, title="Enheter")

        # Knapp for å oppdatere søket etter tilgjengelige kontrollere
        self.update_button = ctk.CTkButton(
            self.popup.top,
            text="Oppdater",
            width=120,
            height=40,
            image=update_icon,
            compound="right",  
            font=("Century Gothic", 16),
            fg_color=popup_button_color,
            hover_color=button_hover_color,
            text_color="white",
            corner_radius=10,
            command=self.start_update
        )
        self.update_button.place(relx=0.05, rely=0.5, anchor="w")

        # Prosessbar (skjult når inaktiv)
        self.progress = ctk.CTkProgressBar(self.popup.bottom, progress_color=frame_color)
        self.progress.pack(pady=10, padx=20, fill="x")
        self.progress.configure(mode="indeterminate")
        self.progress.set(0)
        self.progress.pack_forget()

        self.status_label = ctk.CTkLabel(
            self.popup.bottom,
            text="",
            font=("Century Gothic", 16),
            text_color="white"
        )
        self.status_label.pack(pady=(10, 0))

    # Funksjon: Åpne kontrollpanel
    def open_control_panel(self):
        self.front_frame.destroy()

        self.top_section()
        self.mid_section()
        self.bot_section()

        self.running = True
        self.sensor_value = "__"
        threading.Thread(target=self.get_sensor_data, daemon=True).start()
        self.update_sensor_display()

    # Funksjon: Avslutte programmet (ESC)
    def exit_fullscreen(self, event=None):
        self.destroy()

    # Toppseksjon: logo, tittel og knapper
    def top_section(self):
        self.top_frame = ctk.CTkFrame(self, fg_color=top_panel_color, height=130)
        self.top_frame.grid(row=0, column=0, columnspan=4, sticky="nsew")
        self.top_frame.grid_propagate(False)

        # Legger til USN-logo           
        self.logo_label = ctk.CTkLabel(self.top_frame, image=usn_logo, text="")      
        self.logo_label.place(x=720, y=25)                                               

        # Titteltekst i midten av toppseksjonen
        self.title_label = ctk.CTkLabel(
            self.top_frame, 
            text="ProwlTech\nKontrollpanel",
            font=("Helvetica", 40),
            text_color="#E4E4E4",
            justify="center",
            anchor="center"
        )
        self.title_label.place(relx=0.5, rely=0.5, anchor=ctk.CENTER)


        # Knapp for å koble til kontroller (åpner nytt vindu)
        self.connect_button = ctk.CTkButton(
            self.top_frame,
            width=50,
            height=40,
            text="Koble til kontroller",
            image=bluetooth_icon,
            compound="right",                          
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            text_color="white",
            corner_radius=10,
            command=self.open_connection_window      
        )
        self.connect_button.grid(row=0, column=0, padx=40, pady=45)

        # Meny-knapp
        self.menu_button = ctk.CTkButton(
            self.top_frame,
            width=85,
            height=40,
            text="Meny",
            font=("Century Gothic", 16),
            text_color="white",
            image=menu_icon,
            compound="right",
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=10,
            command=self.front_page
        )
        self.menu_button.grid(row=0, column=3, padx=(320, 10), pady=45)

   
    # Oppdaterer søk etter kontrollere
    def start_update(self):
        self.update_button.configure(text="Søker...", state="disabled")
        self.progress.pack(pady=10)
        self.progress.start()
        #self.after(3000, self.finish_update)

        threading.Thread(target=self.scan_and_show).start() # Kjører skanning i egen tråd slik at GUI ikke fryser

    def scan_and_show(self):
        bluetooth_dbus.scan_devices()   # Starter skanning
        devices = bluetooth_dbus.get_devices()
        self.popup.bottom.after(0, lambda: self.show_devices(devices))

    def show_devices(self, devices):
        self.progress.stop() 
        self.progress.pack_forget() 

        # Fjerner gamle synlige enheter om det er noen
        for widget in self.popup.bottom.winfo_children():
                if isinstance(widget, ctk.CTkButton) or (isinstance(widget, ctk.CTkLabel) and widget != self.status_label):
                    widget.destroy()

        if devices:
            for name in devices.values():
                btn = ctk.CTkButton(
                    self.popup.bottom,
                    text=name,
                    fg_color=popup_button_color,
                    hover_color=button_hover_color,
                    text_color="white",
                    corner_radius=10,
                    command=lambda n=name: self.connect_to_device(n)
                )
                btn.pack(pady=5, padx=20, fill="x")

        else:
            no_devices_label = ctk.CTkLabel(
                self.bottom,
                text="Ingen enheter funnet",
                text_color="white",
                font=("Century Gothic", 16)
            )
            no_devices_label.pack(pady=10)

    def connect_to_device(self, name):
        success = bluetooth_dbus.connect_to_device(name)
        if success:
            self.status_label.configure(text=f"Koblet til {name}", text_color="white")
            self.connection_status.configure(text=f"Tilkoblet: {name}", text_color="white")
        else:
            self.status_label.configure(text=f"Kunne ikke koble til {name}", text_color="red")
            self.connection_status.configure(text=f"Ingen kontroller tilkoblet", text_color="red")

    # Henter sensorverdier fra CAN-bus kontinuerlig i bakgrunnen
    def get_sensor_data(self):
        while self.running:
            val = receive_sensor_data() # Funksjonen som leser CAN-melding
            if val is not None:
                self.sensor_value = val
            else:
                self.sensor_value = "__"

    # Oppdaterer sensorverdi med jevne mellomrom
    def update_sensor_display(self):
        if isinstance(self.sensor_value, dict):
            text = "\n".join(
            [f"{key}: {'🟥 Aktiv' if val else '🟩 Inaktiv'}" for key, val in self.sensor_value.items()]
        )
        else:
            text = "Sensorverdi: --"

        self.sensor_value_label.configure(text=text)
        self.after(500, self.update_sensor_display)


    # Midtseksjon: viser batteri, temperatur, tilkobling og sensor
    def mid_section(self):
        self.mid_frame = ctk.CTkFrame(self, fg_color=background_color)
        self.mid_frame.grid(row=1, column=0, rowspan=2, columnspan=4, sticky="nsew")
        self.mid_frame.grid_propagate(False)

        # Batteristatus:
        self.battery_container = ctk.CTkFrame(self.mid_frame, fg_color=background_color)
        self.battery_container.grid(row=1, column=0, rowspan=2, padx=10, pady=20)

        # Titteltekst
        self.battery_label = ctk.CTkLabel(
            self.battery_container, 
            text="BATTERISTATUS  ",
            font=("Century Gothic", 13),
            image=bolt_icon,
            compound="right",
            text_color=text_color,
            justify="left"
        )
        self.battery_label.pack(side="top", anchor="w", padx=15, pady=0)

        # Ramme
        self.battery_frame = ctk.CTkFrame(self.battery_container, height=130, width=130, fg_color=frame_color, corner_radius=25, border_color=frame_border_color, border_width=border_size)
        self.battery_frame.pack(side="top", expand=True, padx=10, pady=0)

        
        # Temperaturmålinger:
        self.temp_container = ctk.CTkFrame(self.mid_frame, fg_color=background_color)
        self.temp_container.grid(row=1, column=1, rowspan=2, padx=10, pady=20)

        # Titteltekst
        self.temp_label = ctk.CTkLabel(
            self.temp_container, 
            text="TEMPERATUR  ",
            font=("Century Gothic", 13),
            image=temp_icon,
            compound="right",
            text_color=text_color,
            justify="left"
        )
        self.temp_label.pack(side="top", anchor="w", padx=15, pady=0)

        # Ramme
        self.temp_frame = ctk.CTkFrame(self.temp_container, height=130, width=130, fg_color=frame_color, corner_radius=25, border_color=frame_border_color, border_width=border_size)
        self.temp_frame.pack(side="top", expand=True, padx=10, pady=0)


        # Tilkoblingsstatus:
        self.connection_containter = ctk.CTkFrame(self.mid_frame, fg_color=background_color)
        self.connection_containter.grid(row=1, column=2, columnspan=2, sticky="sew")

        # Tilkoblingsstatus - Titteltekst
        self.connection_label = ctk.CTkLabel(
            self.connection_containter, 
            text="TILKOBLING  ",
            font=("Century Gothic", 13),
            image=signal_icon,
            compound="right",
            text_color=text_color,
            justify="left"
        )
        self.connection_label.pack(side="top", anchor="w", padx=35, pady=0)

        # Tilkoblingsstatus - Ramme
        self.connection_frame = ctk.CTkFrame(self.connection_containter, height=70, width=300, fg_color=frame_color, corner_radius=20, border_color=frame_border_color, border_width=7)
        self.connection_frame.pack(side="top", expand=True, padx=20, pady=(0, 10))
        self.connection_frame.pack_propagate(False)

        # Tilkoblingsstatus - Innhold
        self.connection_status = ctk.CTkLabel(
            self.connection_frame,
            text="Ingen kontroller tilkoblet",
            font=("Century Gothic", 12),
            text_color="white",
            anchor="center",
            justify="left"
        )
        self.connection_status.pack(expand=True)


        # Sensormåling:
        self.sensor_container = ctk.CTkFrame(self.mid_frame, fg_color=background_color)
        self.sensor_container.grid(row=2, column=2, columnspan=2, sticky="new")

        # Sensormåling - Tittel
        self.sensor_label = ctk.CTkLabel(
            self.sensor_container, 
            text="SENSORMÅLINGER  ",
            font=("Century Gothic", 13),
            image=sensor_icon,
            compound="right",
            text_color=text_color,
            justify="left"
        )
        self.sensor_label.pack(side="top", anchor="w", padx=35, pady=0)

        # Sensormåling - Ramme
        self.sensor_frame = ctk.CTkFrame(self.sensor_container, height=70, width=300, fg_color=frame_color, corner_radius=20, border_color=frame_border_color, border_width=8)
        self.sensor_frame.pack(side="top", expand=True, padx=20, pady=(0, 10))
        self.sensor_frame.pack_propagate(False)

        # Sensormåling - Innhold
        self.sensor_value_label = ctk.CTkLabel(
            self.sensor_frame,
            text="Ingen data",
            font=("Century Gothic", 12),
            text_color="white"
        )
        self.sensor_value_label.pack(expand=True)

    # Bunnseksjon: viser feilmeldinger
    def bot_section(self):  
        self.bot_frame = ctk.CTkFrame(self, fg_color=background_color, height=120)
        self.bot_frame.grid(row=3, column=0, columnspan=4, sticky="nsew")
        self.bot_frame.grid_propagate(False)

        # Feilmeldinger:
        self.error_container = ctk.CTkFrame(self.bot_frame, fg_color=background_color)
        self.error_container.grid(row=3, column=0, columnspan=4, sticky="nsew")

        # Titteltekst
        self.error_label = ctk.CTkLabel(
            self.error_container, 
            text="FEILMELDINGER  ",
            image=warning_icon,
            compound="right", 
            font=("Century Gothic", 14),
            text_color=text_color,
            justify="left"
        )
        self.error_label.pack(side="top", anchor="w", padx=15, pady=0)
       
        # Ramme
        self.error_frame = ctk.CTkFrame(self.error_container, height=80, width=750, fg_color=error_section, corner_radius=30)
        self.error_frame.pack(side="top", expand=True, padx=10, pady=5)

    def on_closing(self):
        self.running = False
        self.destroy()
       

# Starter GUI-applikasjon
app = ProwlTechApp()
app.protocol("WM_DELETE_WINDOW", app.on_closing)
app.mainloop()