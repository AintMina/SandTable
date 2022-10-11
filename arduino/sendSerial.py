import serial, time

ser = serial.Serial('/dev/ttyUSB0', 500000, timeout=1)
ser.reset_input_buffer()
time.sleep(1)
ser.readline()

while True:
    ser.write(b"lt colorFade\n")
    break