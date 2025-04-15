import customtkinter as ctk

class CanMenuWindow(ctk.CTkToplevel):
    def __init__(self, master):
        super().__init__(master)

        self.title("CAN-meny")
        self.geometry("500x400")
        self.configure(fg_color="#200F2D")
        self.resizable(False, False)

        # Sørg for at vinduet åpnes som et popup-vindu foran hoved-GUI
        self.transient(master)
        self.grab_set()
        self.focus()
        self.lift()
    

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
            command=self.open_kontroller_meny
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

    def open_canbus_meny(self):
        print("Åpner CAN-bus meny...")
        # Her kan du åpne ny popup, vise status, eller kjøre funksjon

    def open_motor_meny(self):
        print("Åpner motorkontroller meny...")
        # Samme som over

    def open_kontroller_meny(self):
        print("Åpner kontroller meny...")
        # Samme som over
