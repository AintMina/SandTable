import serial, time

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
ser.reset_input_buffer()

while True:
    ser.write(b"c home\n")
    break