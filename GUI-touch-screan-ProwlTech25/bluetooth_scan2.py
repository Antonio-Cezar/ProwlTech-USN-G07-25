import asyncio
from bleak import BleakScanner

async def scan_devices():
    print("Søker etter enheter...\n")
    devices = await BleakScanner.discover(timeout=10.0)

    name_filter = ["Controller"]

    found_any = False
    for d in devices:
        if d.name:
            if any(filter_word in d.name.lower() for filter_word in name_filter):
                print(f"{d.name} ({d.address})")
                found_any = True

    if not found_any:
        print("Fant ingen enheter")

if __name__ == "__main__":
    asyncio.run(scan_devices())

    
