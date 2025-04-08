import asyncio
from bleak import BleakScanner

async def scan_devices():
    print("Søker etter enheter...\n")
    devices = await BleakScanner.discover(timeout=5.0)
    for d in devices:
        print(f"{d.name} ({d.address})")

if __name__ == "__main__":
    asyncio.run(scan_devices())

    
