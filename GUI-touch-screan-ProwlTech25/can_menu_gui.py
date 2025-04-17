import customtkinter as ctk
from popup_window import PopupWindow
import subprocess

class CanMenuWindow(ctk.CTkToplevel):
    def __init__(self, master):
        super().__init__(master)

        self.title("CAN-meny")
        self.geometry("800x480")
        self.configure(fg_color="#200F2D")
        self.resizable(False, False)

        # Sørg for at vinduet åpnes som et popup-vindu foran hoved-GUI
        self.transient(master)      # Koblet opp mot hovedvinduet
        self.grab_set()     # Må lukkes før noe annet
        self.focus()    # Legger vinduet foran det andre
        self.lift()     # Legger vinduet foran det andre 
    

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
        # Her kan du åpne ny popup, vise status, eller kjøre funksjon

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
        status = subprocess.getoutput("python3 /home/prowltech/prowltech-script/vesc_cmd.py --can_id 10 --check_status")
        motor = subprocess.getoutput("python3 /home/prowltech/prowltech-script/vesc_cmd.py --can_id 10 --get_status")
        self.motor_status_label.configure(text=f"VESC-status: {status}\nMotorstatus: {motor}")

    def send_start(self):
        subprocess.Popen(["python3", "vesc_cmd.py", "--can_id", "10", "--duty", "0.10"])
        self.motor_status_label.configure(text="Start-kommando sendt")

    def send_stop(self):
        subprocess.Popen(["python3", "vesc_cmd.py", "--can_id", "10", "--duty", "0.00"])
        self.motor_status_label.configure(text="Stop-kommando sendt")

    def get_temp(self):
        output = subprocess.getoutput("python3 vesc_cmd.py --can_id 10 --get_temp")
        self.motor_status_label.configure(text=f"Temperatur:\n{output}")




