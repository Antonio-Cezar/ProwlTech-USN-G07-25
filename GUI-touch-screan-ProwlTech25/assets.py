from PIL import Image
import customtkinter as ctk
import os

# Finn stien til mappen som inneholder denne filen
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Full sti til Image-mappen
IMAGE_DIR = os.path.join(BASE_DIR, "Image")

def load_image(filename, size):
    image_path = os.path.join(IMAGE_DIR, filename)
    return ctk.CTkImage(light_image=Image.open(image_path), size=size)

# Logoer
prowltech_logo = load_image("prowltech_logo2.png", (550, 550))
usn_logo = load_image("USN_logo.png", (60, 70))
usn_logo_sort = load_image("USN_logo_sort.png", (60, 70))

# Bilder
controller_pic = load_image("kontroller.png", (550, 350))

# Ikoner
info_icon = load_image("info.png", (20, 20))
can_icon = load_image("can_bus.png", (20, 20))
bluetooth_icon = load_image("bluetooth.png", (20, 20))
bolt_icon = load_image("bolt.png", (20, 20))
cross_icon = load_image("cross.png", (15, 15))
sensor_icon = load_image("sensor.png", (20, 20))
signal_icon = load_image("signal.png", (15, 15))
temp_icon = load_image("temperature.png", (20, 20))
update_icon = load_image("update.png", (15, 15))
warning_icon = load_image("warning.png", (20, 20))
loading_icon = load_image("loading.png", (20, 20))
start_icon = load_image("start.png", (20, 20))
menu_icon = load_image("menu.png", (20, 20))

