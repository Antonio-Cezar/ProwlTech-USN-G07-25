"""
popup_window.py

Definerer en gjenbrukbar klasse for popup-vinduer i GUI-et.

Den inneholder en toppseksjon, bunnseksjon, tittel og en knapp for å lukke vinduet.

"""

import customtkinter as ctk
from assets import cross_icon

# Definerer farger for enkel endring
popup_background_color = "#200F2D"
popup_top_color = "#3A2557"
popup_button_color = "#6C3DAF"
button_hover_color = "#6C578A"

class PopupWindow:
    def __init__(self, master, title="Popup", on_close=None):
        self.master = master
        self.on_close = on_close

        self.panel = ctk.CTkFrame(master, fg_color=popup_background_color, corner_radius=0)
        self.panel.place(relx=0.5, rely=0.5, anchor="center", relwidth=0.85, relheight=0.85)
        self.panel.grid_propagate(False)

        # Grid-oppsett i popup-vindu
        self.panel.grid_rowconfigure(0, minsize=80)
        self.panel.grid_rowconfigure(1, weight=0)
        self.panel.grid_columnconfigure(0, weight=1)

        # Toppseksjon
        self.top = ctk.CTkFrame(self.panel, fg_color=popup_top_color, height=80)
        self.top.grid(row=0, column=0, sticky="nsew")
        self.top.grid_columnconfigure(0, weight=1)
        self.top.grid_columnconfigure(1, weight=1)
        self.top.grid_columnconfigure(2, weight=1)
        self.top.grid_propagate(False)

        # Bunnseksjon
        self.bottom = ctk.CTkFrame(self.panel, fg_color=popup_background_color)
        self.bottom.grid(row=1, column=0, sticky="nsew")
        self.bottom.grid_rowconfigure(0, weight=1)
        self.bottom.grid_columnconfigure(0, weight=1)

        # Titteltekst
        self.label = ctk.CTkLabel(
            self.top, 
            text=title, 
            font=("Century Gothic", 24), 
            text_color="white"
            )
        self.label.place(relx=0.5, rely=0.5, anchor=ctk.CENTER)

        # Knapp for å lukke popup
        self.close_button = ctk.CTkButton(
            self.top,
            text="Lukk",
            image=cross_icon,
            compound="right",
            font=("Century Gothic", 16),
            fg_color="#6C3DAF",
            hover_color="#6C578A",
            text_color="white",
            corner_radius=40,
            width=120,
            height=40,
            command=self.close
        )
        self.close_button.place(relx=0.95, rely=0.5, anchor="e")

    # Funksjon for å lukke popup
    def close(self):
        self.panel.destroy()
        if self.on_close:
            self.on_close()