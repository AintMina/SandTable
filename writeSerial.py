import serial, time


def startSerial():
    # Try to start serial
    try:
        global ser
        ser = serial.Serial('/dev/ttyUSB0', 500000, timeout=1)
        ser.reset_input_buffer()

        time.sleep(1)
        ser.readline()
    except:
        print("Failed to start serial connection")


def writeToSerial(data):
    try:

        # Check if serial is not open.... NOPE
        try:
            ser
        except:
            startSerial()

        # Checks if data is a string
        if not isinstance(data, str):
            data = str(data)

        # Checks if data ends with newline
        if '\n' not in data:
            data = data + '\n'

        ser.write(data.encode('UTF-8'))
    except:
        print("Failed to write to serial")


def waitForResponse():

    # Loops while no input
    while ser.inWaiting() < 1:
        pass

    input = ser.readline()
    return input.decode('UTF-8')


startSerial()