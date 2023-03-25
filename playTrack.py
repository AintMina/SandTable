import sys, time
import sqlite3
import divideCoords, writeSerial


def playTrack(track_name):
    # Connecting to the database
    connection = sqlite3.connect('/home/pi/sand-table/db.sqlite3')
    cursor = connection.cursor()

    path = "/home/pi/sand-table/media/" + track_name

    # Get the current coordinates
    coords = []
    writeSerial.writeToSerial(19, 0)
    coords.append(float(writeSerial.waitForResponse()))
    writeSerial.writeToSerial(20, 0)
    coords.append(float(writeSerial.waitForResponse()))
    # print(coords)

    # writeSerial.writeToSerial('c get coords\n')
    # time.sleep(0.01)
    # # Waiting for response from arduino
    # input = writeSerial.waitForResponse()
    # input_temp = input.split(' ')

    # # Checking if response is correct
    # while len(input_temp) < 2:
    #     print(input)
    #     writeSerial.writeToSerial("c get coords")
    #     input = writeSerial.waitForResponse()
    #     input_temp = input.split(' ')

    # coords = [float(input_temp[0]), float(input_temp[1])]       # [theta, r]

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
            # if divideCoords.checkDistance(coords, [th, r], 0.01):
            #     split_coords = divideCoords.divideBy(coords, [th, r], 0.03)
            #     if split_coords:
            #         for i in split_coords:
            #             writeSerial.writeToSerial(1, i[0])
            #             writeSerial.writeToSerial(2, i[1])
            #             input_angle = writeSerial.waitForResponse()
            #             input_r = writeSerial.waitForResponse()
            #             coords = [float(input_angle), float(input_r)]
            #             print(coords)

                        # th_out = str(i[0])
                        # r_out = str(i[1])
                        # output = th_out + ' ' + r_out

                        # writeSerial.writeToSerial(output)
                        # input = writeSerial.waitForResponse()
                        # input_temp = input.split(' ')

                        # Checking if response is correct
                        # while len(input_temp) < 2:
                        #     if "ready" in input:
                        #         print("Stopping playback")
                        #         sys.exit()
                        #     print(input)
                        #     writeSerial.writeToSerial("c get coords")
                        #     input = writeSerial.waitForResponse()
                        #     input_temp = input.split(' ')
                        
                        # coords = [float(input_temp[0]), float(input_temp[1])]

            # Writing line to arduino
            # Line includes newline
            writeSerial.writeToSerial(1, th)
            writeSerial.writeToSerial(2, r)
            
            # Arduino returns its current coordinates
            input_angle = writeSerial.waitForResponse()
            input_r = writeSerial.waitForResponse()

             # Checking if response is correct
            while len(input_angle) < 3 or len(input_r) < 3:
                writeSerial.resetBuffer()
                writeSerial.writeToSerial(19, 0)
                input_angle = writeSerial.waitForResponse()
                writeSerial.writeToSerial(20, 0)
                input_r = writeSerial.waitForResponse()

            coords = [float(input_angle), float(input_r)]
            
            # cursor.execute("UPDATE sandtable_settings SET r = ?, theta = ? WHERE id = 0", (coords[0],coords[1]))
            # connection.commit()

    # Delay just in case
    time.sleep(0.1)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        name = sys.argv[1]
        playTrack(name)
    else:
        print("Invalid arguments")