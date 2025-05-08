"""

can_menu_gui.py

Inneholder implementasjon av GUI for CAN-bus-menyen hentet fra 'meny_velger.sh'

"""


import customtkinter as ctk
from popup_window import PopupWindow
import subprocess
import os 

# Setter opp sti til de eksterne skriptene som skal brukes
SCRIPT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "Raspberry-PI-4-scripts"))
VESC_SCRIPT = os.path.join(SCRIPT_DIR, "vesc_cmd.py")

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

border_size = 10
corner = 30
container_text_size = 14
button_corner = 40

#-------------------CAN-Meny-vinduet som opprettes---------------------------------------------------
class CanMenuWindow(ctk.CTkToplevel):
    def __init__(self, master):
        super().__init__(master)

        # Grunnleggende vinduoppsett
        self.title("CAN-meny")
        self.geometry("800x480")
        self.configure(fg_color="#0D0D1F")
        self.resizable(False, False)

        # Sørger for at vinduet vises foran hovedvinduet
        self.transient(master)    
        self.focus() 
        self.lift()    
        self.wait_visibility()
        self.grab_set()

        # Toppseksjon med tittel
        self.top_frame = ctk.CTkFrame(self, fg_color=top_panel_color)
        self.top_frame.pack(fill="x")

        # Tittel
        self.title_label = ctk.CTkLabel(
            self.top_frame,
            text="Velg Kontrollmeny",
            font=("Century Gothic", 22, "bold"),
            text_color="white"
        )
        self.title_label.pack(pady=20)

        # Bunnseksjon med knapper
        self.button_frame = ctk.CTkFrame(self, fg_color=background_color)
        self.button_frame.pack(expand=True)

        # Statusfelt for CAN
        self.can_status_label = ctk.CTkLabel(
            self.button_frame,
            text="Henter status...",
            font=("Century Gothic", 16),
            text_color="white"
        )
        self.can_status_label.pack(padx=20, pady=10)
        self.update_can_status()

        # Åpner popup: CAN-bus-meny
        self.can_button = ctk.CTkButton(
            self.button_frame,
            text="Åpne CAN-bus-kontrollmeny",
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=20,
            command=self.open_canbus_meny
        )
        self.can_button.pack(pady=10, padx=20, fill="x")

        # Åpner popup: Kontroller-meny
        self.controller_button = ctk.CTkButton(
            self.button_frame,
            text="Åpne kontroller-meny",
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=20,
            command=self.open_controller_menu
        )
        self.controller_button.pack(pady=10, padx=20, fill="x")

        # Knapp for å lukke vinduet
        self.exit_button = ctk.CTkButton(
            self.button_frame,
            text="Avslutt",
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=20,
            command=self.destroy
        )
        self.exit_button.pack(pady=(20, 10))

#-------------------CAN-kontroll-vindu---------------------------------------------------
    def open_canbus_meny(self):
        print("Åpner CAN-bus meny...")
        self.popup = PopupWindow(self, title="CAN-bus meny")

        # Statusfelt
        self.can_status_label = ctk.CTkLabel(
            self.popup.bottom,
            text="Henter status...",
            font=("Century Gothic", 16),
            text_color="white"
        )
        self.can_status_label.pack(padx=40, pady=10)
        self.update_can_status()

        # Knapp: Start CAN på nytt
        self.start_can_button = ctk.CTkButton(
            self.popup.bottom,
            text="Start CAN-bus på nytt",
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=20,
            command=self.restart_canbus
        )
        self.start_can_button.pack(padx=40, pady=10, fill="x")

        # Knapp: Slå av can0
        self.turn_off_button = ctk.CTkButton(
            self.popup.bottom,
            text="Slå av can0",
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=20,
            command=self.turn_off_can
        )
        self.turn_off_button.pack(padx=40, pady=10, fill="x")

        # Knapp: Slå på can0
        self.turn_on_button = ctk.CTkButton(
            self.popup.bottom,
            text="Skru på can0",
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=20,
            command=self.turn_on_can
        )
        self.turn_on_button.pack(padx=40, pady=10, fill="x")

        # Knapp: Vis detaljer
        self.status_button = ctk.CTkButton(
            self.popup.bottom,
            text="Vis CAN-status",
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=20,
            command=self.show_can_status
        )
        self.status_button.pack(padx=40, pady=10, fill="x")

        # Knapp: Vis CAN-dump
        self.candump_button = ctk.CTkButton(
            self.popup.bottom,
            text="Vis CAN-dump (trafikk)",
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=20,
            command=self.start_candump
        )
        self.candump_button.pack(padx=40, pady=10, fill="x")

 #-------------------Kontroller-meny-vindu--------------------------------------------------- 
    def open_controller_menu(self):
        print("Åpner kontroller meny...")
        self.popup = PopupWindow(self, title="Kontroller-meny")

        # Knapp: Start Xbox One-kontroller
        self.xbox_button = ctk.CTkButton(
            self.popup.bottom,
            text="Start Xbox One-kontroller",
            font=("Century Gothic", 16),
            fg_color=button_color,
            hover_color=button_hover_color,
            corner_radius=20,
            command=None
        )
        self.xbox_button.pack(pady=20, padx=40, fill="x")

#----------------------------------------Funksjoner---------------------------------------------------
    def update_motor_status(self):
        status = subprocess.getoutput(f"python3 {VESC_SCRIPT} --can_id 10 --check_status")
        motor = subprocess.getoutput(f"python3 {VESC_SCRIPT} --can_id 10 --get_status")
        self.motor_status_label.configure(text=f"VESC-status: {status}\nMotorstatus: {motor}")


    def send_start(self):
        subprocess.Popen(["python3", VESC_SCRIPT, "--can_id", "10", "--duty", "0.10"])
        self.motor_status_label.configure(text="Start-kommando sendt")


    def send_stop(self):
        subprocess.Popen(["python3", VESC_SCRIPT, "--can_id", "10", "--duty", "0.00"])
        self.motor_status_label.configure(text="Stop-kommando sendt")

    def get_temp(self):
        output = subprocess.getoutput(f"python3 {VESC_SCRIPT} --can_id 10 --get_temp")
        self.motor_status_label.configure(text=f"Temperatur:\n{output}")


    def update_can_status(self):
        output = subprocess.getoutput("ip link show can0")
        status = "AKTIV" if "state UP" in output else "INAKTIV"
        self.can_status_label.configure(text=f"CAN0 status: {status}")

    def restart_canbus(self):
        subprocess.call("sudo ip link set can0 down", shell=True)
        subprocess.call("sudo ip link set can0 type can bitrate 125000", shell=True)
        subprocess.call("sudo ip link set can0 up", shell=True)
        self.update_can_status()

    def turn_off_can(self):
        subprocess.call("sudo ip link set can0 down", shell=True)
        self.update_can_status()

    def turn_on_can(self):
        subprocess.call("sudo ip link set can0 type can bitrate 125000", shell=True)
        subprocess.call("sudo ip link set can0 up", shell=True)
        self.update_can_status()

    def show_can_status(self):
        output = subprocess.getoutput("ip -details link show can0")
        self.can_status_label.configure(text=f"Detaljert status:\n{output}")

    # Starter candump i nytt terminalvindu
    def start_candump(self):
        try:
            subprocess.Popen([
                "lxterminal", "-e", "bash -c 'candump can0; exec bash'"
            ])
        except FileNotFoundError:
            print("Fant ikke lxterminal eller candump.")


