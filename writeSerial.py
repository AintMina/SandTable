import serial, time

ser = serial.Serial('/dev/ttyUSB0', 500000, timeout=1)
ser.reset_input_buffer()

time.sleep(1)
ser.readline()

def writeToSerial(data):
    try:

        # Checks if data is a string
        if not isinstance(data, str):
            data = str(data)

        # Checks if data ends with newline
        if '\n' not in data:
            data = data + '\n'

        ser.write(data.encode('UTF-8'))
    except:
        return False