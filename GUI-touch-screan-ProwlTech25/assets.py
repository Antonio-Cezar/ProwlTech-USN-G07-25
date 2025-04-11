from PIL import Image
import customtkinter as ctk

def load_image(path, size):
    return ctk.CTkImage(light_image=Image.open(path), size=size)

# Logoer
prowltech_logo = load_image("Image/prowltech_logo2.png", (550, 550))

# Ikoner
info_icon = load_image("Image/info.png", (20, 20))