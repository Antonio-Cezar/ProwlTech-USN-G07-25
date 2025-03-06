import customtkinter as ctk 

# Hovedvindu-oppsett
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("dark-blue") 

#fargestandarder 
background_color = "#0F0418"
top_panel_color = "#37122F"
button_color = "#77277B"
frame_color = "#3D1232"
border_color = "#2F0729"

class ProwlTechApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("ProwlTech Kontrollpanel")   #tittel
        self.geometry("800x480")    #størrelse på vindu 
        self.resizable(False,False)
        #self.iconbitmap("C:/Users/idala/OneDrive - USN/Bilder/gui")

        #grid
        self.grid_rowconfigure(0, minsize=170) #topprad (row=0)
        self.grid_rowconfigure(1, weight=1) #midtrad (row=1)
        self.grid_rowconfigure(2, minsize=150) #bunnrad (row=2)

        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        self.top_bar()  #row=0
        self.status_section()   #row=1
        self.error_section()    #row=2


    #toppseksjon (row=0) med tittel og knapp
    def top_bar(self):
        self.top_frame = ctk.CTkFrame(self, height=70, fg_color=top_panel_color)
        self.top_frame.grid(row=0, columnspan=2, sticky="nsew")
        self.top_frame.grid_propagate(False)

        #grid for toppraden
        self.top_frame.grid_rowconfigure(0, weight=1)
        self.top_frame.grid_columnconfigure(0, weight=0)    #knapp til venstre
        self.top_frame.grid_columnconfigure(1, weight=0)    #tittel i midten
        self.top_frame.grid_columnconfigure(2, weight=0)    #plass til annet 

        #self.rowconfigure(2, minsize=70)

        #tittel
        self.title_label = ctk.CTkLabel(
            self.top_frame, 
            text="ProwlTech\nKontrollpanel",
            font=("Helvetica", 40),
            text_color="white",
            justify="center",
            anchor="center"
        )
        self.title_label.place(relx=0.5, rely=0.5, anchor=ctk.CENTER)

        #knapp (koble til kontroller)
        self.connect_button = ctk.CTkButton(
            self.top_frame,
            width=50,
            height=50,
            text="Koble til kontroller",
            font=("Century Gothic", 18),
            fg_color=button_color,
            text_color="white",
            corner_radius=10,
            command=self.open_connection_window
        )
        self.connect_button.grid(row=0, column=0, padx=20, pady=20, sticky="w")
    
    #pop-up tilkoblingsvindu
    def open_connection_window(self):
        self.popup_panel = ctk.CTkFrame(self, fg_color=background_color, corner_radius=0, border_width=10, border_color=border_color)
        self.popup_panel.place(relx=0.5, rely=0.5, anchor="center", relwidth=0.9, relheight=0.9)

        label = ctk.CTkLabel(self.popup_panel, text="Tilkobling", font=("Century Gothic", 18))
        label.pack(padx=20, pady=20)

        #knapp til å lukke pop-up
        close_button = ctk.CTkButton(
            self.popup_panel,
            text="Lukk",
            command=self.close_connection_popup
        )
        close_button.pack(padx=20, pady=20)

    #funskjon til å lukke pop-up vindu
    def close_connection_popup(self):
        self.popup_panel.destroy()

      
    #midtseksjon med statusinformasjon
    def status_section(self):
        #hele midtseksjonen:
        self.status_frame = ctk.CTkFrame(self, fg_color=background_color)
        self.status_frame.grid(row=1, column=0, columnspan=3, sticky="nsew")
        self.status_frame.grid_propagate(False)

        #grid i midtreseksjon:
        self.status_frame.grid_rowconfigure(0, weight=1)
        self.status_frame.grid_columnconfigure(0, weight=1)  #venstre
        self.status_frame.grid_columnconfigure(1, weight=1) #høyre

        #venstre kolonne: to bokser side om side
        self.left_frame = ctk.CTkFrame(self.status_frame, fg_color=background_color)
        self.left_frame.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)

        #grid i venstre kolonne:
        self.left_frame.grid_rowconfigure(0, weight=1)
        self.left_frame.grid_columnconfigure(0, weight=1)
        self.left_frame.grid_columnconfigure(1, weight=1)

        #batteristatus
        self.battery_frame = ctk.CTkFrame(self.left_frame, fg_color=frame_color, corner_radius=20, border_width=7, border_color=border_color)
        self.battery_frame.grid(row=0, column=0, padx=10, pady=10)
        self.battery_frame.configure(height=170, width=170)
        self.battery_frame.grid_propagate(False)

        #batteristatus: tittel
        self.battery_label = ctk.CTkLabel(
            self.battery_frame, 
            text="Batteristatus",
            font=("Century Gothic", 14, "bold"),
            text_color="white",
            justify="center",
            anchor="center"
        )
        self.battery_label.place(relx=0.5, rely=0.5, anchor=ctk.CENTER)

        #temperaturmålinger
        self.temp_frame = ctk.CTkFrame(self.left_frame, fg_color=frame_color, corner_radius=20, border_width=7, border_color=border_color)
        self.temp_frame.grid(row=0, column=1, padx=10, pady=10)
        self.temp_frame.configure(height=170, width=170)
        self.temp_frame.grid_propagate(False)

        #temperaturmålinger: tittel
        self.temp_label = ctk.CTkLabel(
            self.temp_frame, 
            text="Temperaturmåling",
            font=("Century Gothic", 14, "bold"),
            text_color="white",
            justify="center",
            anchor="center"
        )
        self.temp_label.place(relx=0.5, rely=0.5, anchor=ctk.CENTER)

        #høyre kolonne: to bokser over hverandre
        self.right_frame = ctk.CTkFrame(self.status_frame, fg_color=background_color)
        self.right_frame.grid(row=0, column=1, sticky="nsew", padx=5, pady=5)

        #grid i høyre kolonne
        self.right_frame.grid_rowconfigure(0, weight=1)
        self.right_frame.grid_rowconfigure(1, weight=1)
        self.right_frame.grid_columnconfigure(0, weight=1)

        #tilkoblingsstatus
        self.connection_frame = ctk.CTkFrame(self.right_frame, fg_color=frame_color, corner_radius=20, border_width=7, border_color=border_color)
        self.connection_frame.grid(row=0, column=0, padx=5, pady=5)
        self.connection_frame.configure(height=80, width=300)
        self.connection_frame.grid_propagate(False)

        #tilkoblingsstatus: tittel
        self.connect_label = ctk.CTkLabel(
            self.connection_frame, 
            text="Tilkoblingsstatus",
            font=("Century Gothic", 14, "bold"),
            text_color="white",
            justify="center",
            anchor="center"
        )
        self.connect_label.place(relx=0.5, rely=0.5, anchor=ctk.CENTER)

        #sensormålinger
        self.sensor_frame = ctk.CTkFrame(self.right_frame, fg_color=frame_color, corner_radius=20, border_width=7, border_color=border_color)
        self.sensor_frame.grid(row=1, column=0, padx=5, pady=5, sticky="n")
        self.sensor_frame.configure(height=80, width=300)
        self.sensor_frame.grid_propagate(False)

        #sensormålinger: tittel
        self.sensor_label = ctk.CTkLabel(
            self.sensor_frame, 
            text="Sensordata",
            font=("Century Gothic", 14, "bold"),
            text_color="white",
            justify="center",
            anchor="center"
        )
        self.sensor_label.place(relx=0.5, rely=0.5, anchor=ctk.CENTER)


    #bunnseksjon med feilmeldinger 
    def error_section(self):
        self.error_container = ctk.CTkFrame(self, fg_color=background_color, height=50)
        self.error_container.grid(row=2, column=0, columnspan=2, sticky="nsew")
        self.error_container.grid_propagate(False)

        self.error_container.grid_rowconfigure(0, weight=1)
        self.error_container.grid_columnconfigure(0, weight=1)

        self.error_frame = ctk.CTkFrame(self.error_container, fg_color="#1F1627", corner_radius=20)
        self.error_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

        self.error_label = ctk.CTkLabel(
            self.error_frame,
            text="Feilmeldinger:", 
            font=("Century Gothic", 16),
            text_color="white",
            justify="left",
            anchor="w"
        )
        self.error_label.pack(fill="both", padx=10, pady=10)



# Start GUI
app = ProwlTechApp()
app.mainloop()
