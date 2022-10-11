import sys, time
import divideCoords, writeSerial


def playTrack(track_name):
    path = "/home/pi/sand-table/media/" + track_name

    # Get the current coordinates
    writeSerial.writeToSerial('c get coords\n')
    time.sleep(0.01)
    # Waiting for response from arduino
    input = writeSerial.waitForResponse()
    input_temp = input.split(' ')

    # Checking if response is correct
    while len(input_temp) < 2:
        print(input)
        writeSerial.writeToSerial("c get coords")
        input = writeSerial.waitForResponse()
        input_temp = input.split(' ')

    coords = [float(input_temp[0]), float(input_temp[1])]       # [theta, r]

    # Opening the .thr file
    with open(path) as f:
        # Iterating lines
        for line in f:
            # If line is comment or line does not contain anything then continue to next line
            if '#' in line or len(line) < 4 or "/" in line:
                continue
            
            # Getting theta and rho from the line and changin them to float values
            th_str, r_str = line.split(' ')
            th = float(th_str)
            r = float(r_str)

            # If distance between point is too far (0.03 units)
            if divideCoords.checkDistance(coords, [th, r], 0.01):
                split_coords = divideCoords.divideBy(coords, [th, r], 0.03)
                if split_coords:
                    for i in split_coords:
                        th_out = str(i[0])
                        r_out = str(i[1])
                        output = th_out + ' ' + r_out

                        writeSerial.writeToSerial(output)
                        input = writeSerial.waitForResponse()
                        input_temp = input.split(' ')

                        # Checking if response is correct
                        while len(input_temp) < 2:
                            if "ready" in input:
                                print("Stopping playback")
                                sys.exit()
                            print(input)
                            writeSerial.writeToSerial("c get coords")
                            input = writeSerial.waitForResponse()
                            input_temp = input.split(' ')
                        
                        coords = [float(input_temp[0]), float(input_temp[1])]

            # Writing line to arduino
            # Line includes newline
            writeSerial.writeToSerial(line)
            
            # Arduino returns its current coordinates
            input = writeSerial.waitForResponse()

             # Checking if response is correct
            while len(input_temp) < 2:
                if "ready" in input:
                    print("Stopping playback")
                    sys.exit()
                print(input)
                writeSerial.writeToSerial("c get coords")
                input = writeSerial.waitForResponse()
                input_temp = input.split(' ')

            input_temp = input.split(' ')
            coords = [float(input_temp[0]), float(input_temp[1])]

    # Delay just in case
    time.sleep(0.1)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        name = sys.argv[1]
        playTrack(name)
    else:
        playTrack('/tracks/wormhole.thr')