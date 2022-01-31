import serial, sys, time
import divideCoords

ser = serial.Serial('/dev/ttyUSB0', 500000, timeout=1)
ser.reset_input_buffer()

time.sleep(1)


def writeToSerial(data):
    # Checks if data is a string
    if not isinstance(data, str):
        data = str(data)

    # Checks if data ends with newline
    if '\n' not in data:
        data = data + '\n'

    ser.write(data.encode('UTF-8'))


def waitForResponse():

    # Loops while no input
    while ser.inWaiting() < 1:
        pass

    input = ser.readline()
    return input.decode('UTF-8')


def playTrack(track_name):
    path = "media/" + track_name           # Add 'media/' + 

    # Get the current coordinates
    writeToSerial('c get coords\n')
    time.sleep(0.01)
    input = waitForResponse()
    input_temp = input.split(' ')

    # Checking if response is correct
    while len(input_temp) < 2:
        if "ready" in input:
            print("Stopping playback")
            sys.exit()
        print(input)
        writeToSerial("c get coords")
        input = waitForResponse()
        input_temp = input.split(' ')

    coords = [float(input_temp[0]), float(input_temp[1])]       # [theta, r]

    with open(path) as f:
        for line in f:
            if '#' in line or len(line) < 4:
                continue
            
            th_str, r_str = line.split(' ')
            th = float(th_str)
            r = float(r_str)

            # If distance between point is too far (0.03 units)
            tooFar = divideCoords.checkDistance(coords, [th, r], 0.01)
            if tooFar:
                split_coords = divideCoords.divideBy(coords, [th, r], 0.03)
                if not split_coords == 0:
                    for i in split_coords:
                        th_out = str(i[0])
                        r_out = str(i[1])
                        output = th_out + ' ' + r_out

                        writeToSerial(output)
                        input = waitForResponse()
                        input_temp = input.split(' ')

                        # Checking if response is correct
                        while len(input_temp) < 2:
                            if "ready" in input:
                                print("Stopping playback")
                                sys.exit()
                            print(input)
                            writeToSerial("c get coords")
                            input = waitForResponse()
                            input_temp = input.split(' ')
                        
                        coords = [float(input_temp[0]), float(input_temp[1])]

            # line includes newline
            writeToSerial(line)
            
            input = waitForResponse()

             # Checking if response is correct
            while len(input_temp) < 2:
                if "ready" in input:
                    print("Stopping playback")
                    sys.exit()
                print(input)
                writeToSerial("c get coords")
                input = waitForResponse()
                input_temp = input.split(' ')

            input_temp = input.split(' ')
            coords = [float(input_temp[0]), float(input_temp[1])]

    time.sleep(0.1)
    writeToSerial("c stop")
    time.sleep(0.1)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        name = sys.argv[1]
        playTrack(name)
    else:
        playTrack('/tracks/wormhole.thr')