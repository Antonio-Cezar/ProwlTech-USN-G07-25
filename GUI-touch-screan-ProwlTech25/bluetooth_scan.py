import subprocess
import time
import threading

device_dict = {}    # Liste for å lagre navn og MAC-adresser

# Holde styr på scanning-status
scanning = False
stop_scan_flag = False

# Kjør bluetoothctl-kommandoer
def run_bluetoothctl_commands(commands):
    process = subprocess.Popen(
        ['bluetoothctl'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    output, _ = process.communicate(commands)
    return output

# Starter skanning i bakgrunnen
def start_scan(device_filter, update_callback, status_callback):
    # update: oppdaterer GUI med nye enheter. status: oppdaterer status i GUI

    global device_dict, scanning, stop_scan_flag

    # Tilbakestiller og klargjør
    device_dict.clear()
    scanning = True
    stop_scan_flag = False

    # Starter skanning
    run_bluetoothctl_commands("scan on\n")
    status_callback("Søker etter enheter...")

    while not stop_scan_flag:
        # Oversikt over kjente enheter
        output = run_bluetoothctl_commands("devices\n")
        new_found = False

        for line in output.split("\n"):
            if line.startswith("Device"):
                parts = line.split(" ", 2)
                if len(parts) == 3:
                    mac, name = parts[1], parts[2]

                    # Filtrer
                    if any(keyword in name.lower() for keyword in device_filter):
                        if name not in device_dict:
                            device_dict[name] = macnew_found = True


        # Oppdater GUI
        if new_found:
            update_callback()

        time.sleep(2)

    run_bluetoothctl_commands("scan off\n")
    scanning = False
    status_callback("Søk avbrutt")

# GUI kan stoppe skanning
def stop_scan():
    global stop_scan_flag
    stop_scan_flag = True

# Returnerer navnene
def get_devices(device_name):
    mac = device_dict.get(device_name)
    if not mac:
        return False 
    
    # Pair, trust og connect
    run_bluetoothctl_commands(f"pair {mac}\n")
    time.sleep(1)
    run_bluetoothctl_commands(f"trust {mac}\n")
    time.sleep(1)
    run_bluetoothctl_commands(f"connect {mac}\n")
    return True


