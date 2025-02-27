import customtkinter as ctk 

# Hovedvindu-oppsett
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue") 

root = ctk.CTk()

root.geometry("800x480")    #størrelse på vindu 
root.title("ProwlTech Kontrollpanel")   #tittel

root.grid_columnconfigure(0, weight=1)
      
def button():
    print("button pressed")

button = ctk.CTkButton(root, text="my button", command=button)
button.grid(row=0, column=0, padx=20, pady=20, sticky="ew")

# Start GUI
root.mainloop()
