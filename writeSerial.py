import serial, time
import struct, sys


def split_float(f):
	# Convert a float to a bytes object
	float_value = f
	bytes_value = struct.pack('f', float_value)

	# Convert the bytes object to a list of integers
	int_values = [b for b in bytes_value]

	return int_values


def split_int(d):
	# Convert a float to a bytes object
	value = d
	bytes_value = struct.pack('i', value)

	# Convert the bytes object to a list of integers
	int_values = [b for b in bytes_value]

	return int_values


def startSerial():
    # Try to start serial
    try:
        global ser
        ser = serial.Serial('/dev/ttyACM0', 115200, timeout=0.0002)
        ser.reset_input_buffer()

        time.sleep(1)
    except:
        print("Failed to start serial connection")


def resetBuffer():
    ser.reset_input_buffer()


def writeToSerial(command, data):
    try:

        # Check if serial is not open
        try:
            ser
        except:
            startSerial()

        byte = []
        time.sleep(0.01)

        # Checks if data is a float or int
        if isinstance(data, float):
            byte = split_float(data)
        elif isinstance(data, int):
            byte = split_int(data)
        
        b = [command, len(byte)] + byte
        # print(b)

        ser.write(bytes(b))
    except:
        print("Failed to write to serial")


def waitForResponse():

    # Loops while no input
    while ser.inWaiting() < 1:
        pass

    input = ser.readline()
    return input.decode('UTF-8')


def readSerial():

    # Loops while no input
    while ser.inWaiting() < 1:
        pass

    input = ser.readline()
    return input.decode('UTF-8')


startSerial()