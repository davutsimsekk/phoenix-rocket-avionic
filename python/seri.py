import serial
import struct

ser = serial.Serial('COM3', 9600)  # Replace 'COM3' with your actual serial port

while True:
    data = ser.read(12)  # Read 12 bytes (3 floats * 4 bytes/float)

    if len(data) == 12: # Check if we received 12 bytes. If not, don't try to unpack, could be an incomplete message
        try:
            altitude, pressure, temperature = struct.unpack('fff', data)
            print(f"Altitude: {altitude}, Pressure: {pressure}, Temperature: {temperature}")
        except struct.error as e:
            print(f"Error unpacking data: {e}. Received data: {data}")
    else:
        print(f"Received incomplete data: {len(data)} bytes, expected 12.")