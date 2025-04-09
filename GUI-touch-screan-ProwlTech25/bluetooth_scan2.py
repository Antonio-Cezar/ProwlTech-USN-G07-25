import asyncio
from bleak import BleakScanner

async def scan_devices():
    print("Søker etter enheter...\n")
    devices = await BleakScanner.discover(timeout=10.0)

    found_name = False
    for d in devices:
        if d.name:
            print(f"{d.name} ({d.address})")
            found_name = True

    if not found_name:
        print("Fant ingen enheter")

asyncio.run(scan_devices())

    
