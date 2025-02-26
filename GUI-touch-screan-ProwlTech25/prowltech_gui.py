import customtkinter as ctk

# Hovedvindu-oppsett
ctk.set_appearance_mode("dark")  # Mørk modus
ctk.set_default_color_theme("blue")  # Fargetema

class ProwlTechApp(ctk.CTk):
    def __init__(self):  # OBS: Det var en feil i "__init__", du hadde "_init_"!
        super().__init__()

        self.geometry("800x480")
        self.title("ProwlTech Kontrollpanel")

        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1)

        # Toppseksjon
        top_frame = ctk.CTkFrame(self, fg_color="#4E1942", width=780, height=100)  # Sett en fast størrelse
        top_frame.grid(row=0, column=0, sticky="nsew", padx=10, pady=10)

        title = ctk.CTkLabel(top_frame, text="ProwlTech\nKontrollpanel", 
                             font=("Century Gothic", 24, "bold"), text_color="white")
        title.pack(pady=10)

        # Legg til en midtre seksjon for testing
        mid_frame = ctk.CTkFrame(self, fg_color="#5A2C6D", width=780, height=200)
        mid_frame.grid(row=1, column=0, sticky="nsew", padx=10, pady=10)

        mid_label = ctk.CTkLabel(mid_frame, text="Midtseksjon", font=("Century Gothic", 18), text_color="white")
        mid_label.pack(pady=10)


# Start GUI
app = ProwlTechApp()
app.mainloop()
