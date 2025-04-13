#!/usr/bin/env python3
import can
import argparse
import struct
import sys

COMM_GET_VALUES = 4

def get_status(bus, can_id, silent=False):
    data = bytes([COMM_GET_VALUES])
    msg = can.Message(arbitration_id=can_id | (1 << 8), data=data, is_extended_id=False)
    try:
        bus.send(msg)
    except Exception as e:
        if not silent:
            print("Feil: kunne ikke sende CAN-melding")
        return 2

    response = bus.recv(timeout=1)

    if not response or len(response.data) < 5:
        if not silent:
            print("Ingen respons fra VESC")
        return 1  # Ikke tilkoblet

    # Parse out duty cycle (assume float starts at byte 1)
    try:
        duty_cycle = struct.unpack('>f', response.data[1:5])[0]
        if not silent:
            print(f"Duty Cycle: {duty_cycle:.3f}")
        return 0 if abs(duty_cycle) > 0.01 else 3  # 0=aktiv, 3=tilkoblet men av
    except Exception:
        return 4  # Parsing-feil

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--can_id', type=int, required=True)
    parser.add_argument('--get_status', action='store_true')
    parser.add_argument('--get_temp', action='store_true')
    parser.add_argument('--duty', type=float)

    args = parser.parse_args()

    try:
        bus = can.interface.Bus(channel='can0', bustype='socketcan')
    except Exception as e:
        sys.exit(2)

    if args.get_status:
        code = get_status(bus, args.can_id, silent=True)
        sys.exit(code)

    if args.get_temp:
        get_status(bus, args.can_id, silent=False)

    if args.duty is not None:
        data = struct.pack('>Bf', 0, args.duty)
        msg = can.Message(arbitration_id=args.can_id | (1 << 8), data=data, is_extended_id=False)
        bus.send(msg)
        print(f"Sent duty {args.duty:.2f} to VESC")

main()
