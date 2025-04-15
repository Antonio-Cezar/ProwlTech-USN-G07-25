from PIL import Image
import customtkinter as ctk

def load_image(path, size):
    return ctk.CTkImage(light_image=Image.open(path), size=size)

# Logoer
prowltech_logo = load_image("Image/prowltech_logo2.png", (550, 550))
usn_logo = load_image("Image/USN_logo.png", (60, 70))

# Ikoner
info_icon = load_image("Image/info.png", (20, 20))
can_icon = load_image("Image/can_bus.png", (20, 20))
bluetooth_icon = load_image("Image/bluetooth.png", (20, 20))
bolt_icon = load_image("Image/bolt.png", (20, 20))
cross_icon = load_image("Image/cross.png", (20, 20))
menu_icon = load_image("Image/menu.png", (20, 20))
cross_icon = load_image("Image/cross.png", (15, 15))
sensor_icon = load_image("Image/sensor.png", (20, 20))
signal_icon = load_image("Image/signal.png", (15, 15))
temp_icon = load_image("Image/temperature.png", (20, 20))
update_icon = load_image("Image/update.png", (15, 15))
warning_icon = load_image("Image/warning.png", (20, 20))
loading_icon = load_image("Image/loading.png", (20, 20))
start_icon = load_image("Image/start.png", (20, 20))