import sqlite3
import signal, time
import divideCoords, writeSerial


state = 1
next_track = 0
stop = 0
did = 0


# Connecting to the database
connection = sqlite3.connect('/home/pi/sand-table/db.sqlite3')
cursor = connection.cursor()


def signal_handler(signum, frame):
    global state, next_track
    if signum == signal.SIGUSR1:
        if state == 1:
            print("Paused")
            state = 0
        elif state == 0:
            print("Playing")
            state = 1
    elif signum == signal.SIGUSR2:
        next_track = 1
    elif signum == signal.SIGTERM:
        global stop
        # Stop the script
        print('Stopping.')
        stop = 1
        # exit()


# Set up the signal handlers
signal.signal(signal.SIGUSR1, signal_handler)
signal.signal(signal.SIGUSR2, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)


def playTrack(track_name):
    global state, next_track, did, stop
    path = "/home/pi/sand-table/media/" + track_name

    print("Starting")

    # Get the current coordinates
    writeSerial.resetBuffer()
    coords = []
    writeSerial.writeToSerial(19, 0)
    coords.append(float(writeSerial.waitForResponse()))
    writeSerial.writeToSerial(20, 0)
    coords.append(float(writeSerial.waitForResponse()))

    counter = 0

    # Opening the .thr file
    with open(path) as f:
        # Iterating lines
        for line in f:
            writeSerial.resetBuffer()

            # while paused
            while state == 0:
                time.sleep(0.1)
                if not did:
                    cursor.execute("UPDATE sandtable_settings SET r = ?, theta = ? WHERE id = 0", (coords[1],coords[0]))
                    did = 1
                # if kill signal
                if stop == 1:
                    cursor.execute("UPDATE sandtable_settings SET r = ?, theta = ? WHERE id = 0", (coords[1],coords[0]))
                    exit()
            
            # if next track signal
            if next_track == 1:
                next_track = 0
                return coords
            
            # if kill signal
            if stop == 1:
                cursor.execute("UPDATE sandtable_settings SET r = ?, theta = ? WHERE id = 0", (coords[1],coords[0]))
                exit()
            
            # If line is comment or line does not contain anything then continue to next line
            if '#' in line or len(line) < 4 or "/" in line:
                continue
            
            # Getting theta and rho from the line and changin them to float values
            th_str, r_str = line.split(' ')
            th = float(th_str)
            r = float(r_str)

            if r > 1.0:
                print("R Too big")

            # If distance between point is too far (0.03 units)
            if divideCoords.checkDistance(coords, [th, r], 0.01):
                split_coords = divideCoords.divideBy(coords, [th, r], 0.03)
                if split_coords:
                    for i in split_coords:
                        writeSerial.resetBuffer()
                        writeSerial.writeToSerial(1, i[0])
                        writeSerial.writeToSerial(2, i[1])
                        input_angle = writeSerial.waitForResponse()
                        input_r = writeSerial.waitForResponse()

                        # Checking if angle and rho switched
                        if len(input_angle) > 3 and len(input_r) > 3 and (abs(r - float(input_angle)) < 0.01 or abs(th - float(input_r)) < 0.01):
                            writeSerial.resetBuffer()
                            time.sleep(0.01)
                            writeSerial.writeToSerial(19, 0)
                            input_angle = writeSerial.waitForResponse()
                            writeSerial.writeToSerial(20, 0)
                            input_r = writeSerial.waitForResponse()

                        # Checking if response is correct
                        while len(input_angle) < 3 or len(input_r) < 3 or float(input_r) > 1.0: # or abs(r - float(input_angle)) < 0.001 or abs(th - float(input_r)) < 0.001:
                            writeSerial.resetBuffer()
                            time.sleep(0.01)
                            writeSerial.writeToSerial(19, 0)
                            input_angle = writeSerial.waitForResponse()
                            writeSerial.writeToSerial(20, 0)
                            input_r = writeSerial.waitForResponse()

                        coords = [float(input_angle), float(input_r)]

            # Writing line to arduino
            # Line includes newline
            writeSerial.writeToSerial(1, th)
            writeSerial.writeToSerial(2, r)
            
            # Arduino returns its current coordinates
            input_angle = writeSerial.waitForResponse()
            input_r = writeSerial.waitForResponse()

            # Checking if angle and rho switched
            if  len(input_angle) > 3 and len(input_r) > 3 and (abs(r - float(input_angle)) < 0.01 or abs(th - float(input_r)) < 0.01):
                writeSerial.resetBuffer()
                time.sleep(0.01)
                writeSerial.writeToSerial(19, 0)
                input_angle = writeSerial.waitForResponse()
                writeSerial.writeToSerial(20, 0)
                input_r = writeSerial.waitForResponse()

            # Checking if response is correct
            while len(input_angle) < 3 or len(input_r) < 3 or float(input_r) > 1.0: # or abs(r - float(input_angle)) < 0.001 or abs(th - float(input_r)) < 0.001:
                writeSerial.resetBuffer()
                time.sleep(0.01)
                writeSerial.writeToSerial(19, 0)
                input_angle = writeSerial.waitForResponse()
                writeSerial.writeToSerial(20, 0)
                input_r = writeSerial.waitForResponse()


            coords = [float(input_angle), float(input_r)]
            # print(coords)

            # updating coordinates
            # if counter > 100:
            #     cursor.execute("UPDATE sandtable_settings SET r = ?, theta = ? WHERE id = 0", (coords[1],coords[0]))
            #     counter = 0

            # counter += 1

    # Delay just in case
    time.sleep(0.1)
    return coords


def PlayQ():
    global state, connection, cursor
    state = 1

    # Reading the queue table
    rows = cursor.execute("SELECT id, file, track_length, pic FROM sandtable_queue").fetchall()

    # While there are entries in the table
    while rows:
        file = rows[0][1]
        length = rows[0][2]

        # print(file)

        # Playing the track in the first index
        coords = playTrack(file)
        cursor.execute("UPDATE sandtable_settings SET r = ?, theta = ? WHERE id = 0", (coords[1],coords[0]))
        connection.commit()

        # Deleting the first track that was just played
        cursor.execute("DELETE FROM sandtable_queue WHERE id = ?", (rows[0][0],))
        connection.commit()

        # Reading the new table
        rows = cursor.execute("SELECT id, file, track_length, pic FROM sandtable_queue").fetchall()



if __name__ == "__main__":
    PlayQ()