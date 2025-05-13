from flask import Flask, jsonify
import subprocess

app = Flask(__name__)

# Funksjon som sjekker om kontrolleren er tilkoblet via Bluetooth
def er_kontroller_tilkoblet():
    try:
        # Kjører kommando som lister aktive Bluetooth-tilkoblinger
        output = subprocess.check_output("hcitool con", shell=True).decode()
        return "ACL" in output  # Returnerer True hvis en enhet er koblet til
    except:
        return False  # Hvis noe går galt, antar vi at ingen er koblet til

@app.route('/status')
def status():
    return jsonify({
        "kontroller_tilkoblet": er_kontroller_tilkoblet(),  # Sann/usann
    })

if __name__ == '__main__':
    # Kjører Flask-serveren og gjør den tilgjengelig for andre enheter i nettverket
    app.run(host='0.0.0.0', port=5000)
