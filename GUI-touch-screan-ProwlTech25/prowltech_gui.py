import customtkinter as ctk 

# Hovedvindu-oppsett
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("dark-blue") 

class ProwlTechApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("ProwlTech Kontrollpanel")   #tittel
        self.geometry("800x480")    #størrelse på vindu 
        self.resizable(False,False)

        #grid
        self.grid_rowconfigure(0, weight=1) #topprad (row=0)
        self.grid_rowconfigure(1, weight=0) #midtrad (row=1)
        self.grid_rowconfigure(2, weight=1) #bunnrad (row=2)

        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        self.top_bar()  #row=0
        self.status_section()   #row=1
        self.error_section()    #row=2



    def top_bar(self):
        self.top_frame = ctk.CTkFrame(self, width=10, height=150, fg_color="#4C0643")
        self.top_frame.grid(row=0, columnspan=2, sticky="nsew")

        #grid for toppraden
        self.top_frame.grid_rowconfigure(0, weight=1)
        self.top_frame.grid_columnconfigure(0, weight=0)    #knapp til venstre
        self.top_frame.grid_columnconfigure(1, weight=1)    #tittel i midten
        self.top_frame.grid_columnconfigure(2, weight=0)    #plass til annet 

        #tittel
        self.title_label = ctk.CTkLabel(
            self.top_frame, 
            text="ProwlTech\nKontrollpanel",
            font=("Century Gothic", 30, "bold"),
            text_color="white",
            justify="center",
            anchor="center"
        )
        self.title_label.place(relx=0.5, rely=0.5, anchor=ctk.CENTER)               #grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

        #knapp (koble til kontroller)
        self.connect_button = ctk.CTkButton(
            self.top_frame,
            text="Koble til kontroller",
            fg_color="#6f1c7c",
            text_color="white",
            corner_radius=10
        )
        self.connect_button.grid(row=0, column=0, padx=10, pady=10, sticky="w")

        #dummy-widget for å balansere layouten
        #self.spacer = ctk.CTkFrame(self, fg_color="#4C0643")
        #self.spacer.grid(row=0, column=2, sticky="nsew")

    def status_section(self):
        self.status_frame = ctk.CTkFrame(self, width=10, height=200, fg_color="#29043C")
        self.status_frame.grid(row=1, column=0, columnspan=2, sticky="nsew")

    def error_section(self):
        self.error_frame = ctk.CTkFrame(self, width=10, height=100, fg_color="#29043C")
        self.error_frame.grid(row=2, columnspan=2, sticky="nsew")



# Start GUI
app = ProwlTechApp()
app.mainloop()
