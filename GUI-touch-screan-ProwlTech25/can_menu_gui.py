import customtkinter as ctk
from popup_window import PopupWindow
import subprocess
import os 

SCRIPT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "Raspberry-PI-4-scripts"))
VESC_SCRIPT = os.path.join(SCRIPT_DIR, "vesc_cmd.py")

class CanMenuWindow(ctk.CTkToplevel):
    def __init__(self, master):
        super().__init__(master)

        self.title("CAN-meny")
        self.geometry("800x480")
        self.configure(fg_color="#200F2D")
        self.resizable(False, False)

        # Sørg for at vinduet åpnes som et popup-vindu foran hoved-GUI
        self.transient(master)      # Koblet opp mot hovedvinduet
        self.focus()    # Legger vinduet foran det andre
        self.lift()     # Legger vinduet foran det andre 
        self.wait_visibility()
        self.grab_set()

    
        # Toppseksjon
        self.top_frame = ctk.CTkFrame(self, fg_color="#3A2557")
        self.top_frame.pack(fill="x")

        self.title_label = ctk.CTkLabel(
            self.top_frame,
            text="Velg Kontrollmeny",
            font=("Century Gothic", 22, "bold"),
            text_color="white"
        )
        self.title_label.pack(pady=20)

        # Bunnseksjon med knapper
        self.button_frame = ctk.CTkFrame(self, fg_color="#200F2D")
        self.button_frame.pack(expand=True)

        # Alternativ 1: CAN-bus meny
        self.can_button = ctk.CTkButton(
            self.button_frame,
            text="1. CAN-bus kontrollmeny",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.open_canbus_meny
        )
        self.can_button.pack(pady=10, padx=20, fill="x")

        # Alternativ 2: Motorkontroller meny
        self.motor_button = ctk.CTkButton(
            self.button_frame,
            text="2. Motorkontroller meny",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.open_motor_meny
        )
        self.motor_button.pack(pady=10, padx=20, fill="x")

        # Alternativ 3: Kontroller meny
        self.controller_button = ctk.CTkButton(
            self.button_frame,
            text="3. Kontroller meny",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.open_controller_menu
        )
        self.controller_button.pack(pady=10, padx=20, fill="x")

        # Avslutt-knapp
        self.exit_button = ctk.CTkButton(
            self.button_frame,
            text="Avslutt",
            font=("Century Gothic", 16),
            fg_color="#333333",
            hover_color="#444444",
            corner_radius=20,
            command=self.destroy
        )
        self.exit_button.pack(pady=(20, 10))

#-------------------Popup-vinduer---------------------------------------------------

    def open_canbus_meny(self):
        print("Åpner CAN-bus meny...")
        self.popup = PopupWindow(self, title="CAN-bus meny")

        # Statusfelt
        self.can_status_label = ctk.CTKLavel(
            self.popup.top,
            text="Henter status...",
            font=("Century Gothic", 16),
            text_color="white"
        )
        self.can_status_label.pack(pady(0, 10))
        self.update_can_status()

        # Start CAN-bus
        self.start_can_button = ctk.CTkButton(
            self.popup.bottom,
            text="Start CAN-bus på nytt",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.restart_canbus
        )
        self.start_can_button.pack(padx=40, pady=10, fill="x")

        # Slå av can0
        self.turn_off_button = ctk.CTkButton(
            self.popup.bottom,
            text="Slå av can0",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.turn_off_can
        )
        self.turn_off_button.pack(padx=40, pady=10, fill="x")

        # Slå på can0
        self.turn_on_button = ctk.CTkButton(
            self.popup.bottom,
            text="Skru på can0",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.turn_on_can
        )
        self.turn_on_button.pack(padx=40, pady=10, fill="x")

        # Vis detaljer
        self.status_button = ctk.CTkButton(
            self.popup.bottom,
            text="Vis CAN-status",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.show_can_status
        )
        self.status_button.pack(padx=40, pady=10, fill="x")

    def open_motor_meny(self):
        print("Åpner motorkontroller meny...")
        self.popup = PopupWindow(self, title="Motorkontroller-meny")

        # Statuslinje
        self.motor_status_label = ctk.CTkLabel(
            self.popup.top,
            text="Henter status...",
            font=("Century Gothic", 16),
            text_color="white"
        )
        self.motor_status_label.pack(pady=(0, 10))
        self.update_motor_status()  # Henter og oppdaterer 

        # Start-knapp
        self.start_button = ctk.CTkButton(
            self.popup.bottom,
            text="Send PÅ-kommando til VESC",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.send_start
        )
        self.start_button.pack(padx=40, pady=10, fill="x")

        # Stopp-knapp
        self.stop_button = ctk.CTkButton(
            self.popup.bottom,
            text="Send AV-kommando til VESC",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.send_stop
        )
        self.stop_button.pack(padx=40, pady=10, fill="x")

        # Temperatur-knapp
        self.temp_button = ctk.CTkButton(
            self.popup.bottom,
            text="Hent temperatur fra VESC",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=self.get_temp
        )
        self.temp_button.pack(padx=40, pady=10, fill="x")

    def open_controller_menu(self):
        print("Åpner kontroller meny...")
        self.popup = PopupWindow(self, title="Kontroller-meny")

        # Start Xbox One-kontroller
        self.xbox_button = ctk.CTkButton(
            self.popup.bottom,
            text="Start Xbox One-kontroller",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#7D4CC3",
            corner_radius=20,
            command=None
        )
        self.xbox_button.pack(pady=20, padx=40, fill="x")

#----------------------------------------DIV. FUNKSJONER---------------------------------------------------
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



