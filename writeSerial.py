import serial, time


def writeToSerial(data):
    ser = serial.Serial('/dev/ttyUSB0', 57600, timeout=1)
    ser.reset_input_buffer()

    time.sleep(1)
    ser.readline()

    # Checks if data is a string
    if not isinstance(data, str):
        data = str(data)

    # Checks if data ends with newline
    if '\n' not in data:
        data = data + '\n'

    ser.write(data.encode('UTF-8'))