import sqlite3
import playTrack


def PlayQ():

    # Connecting to the database
    connection = sqlite3.connect('db.sqlite3')
    cursor = connection.cursor()

    # Reading the queue table
    rows = cursor.execute("SELECT id, file, track_length, pic FROM sandtable_queue").fetchall()

    # While there are entries in the table
    while rows:
        file = rows[0][1]
        length = rows[0][2]

        # Playing the track in the first index
        playTrack.playTrack(file)

        # Deleting the first track that was just played
        cursor.execute("DELETE FROM sandtable_queue WHERE id = ?", (rows[0][0],))
        connection.commit()

        # Reading the new table
        rows = cursor.execute("SELECT id, file, track_length, pic FROM sandtable_queue").fetchall()



if __name__ == "__main__":
    PlayQ()
    playTrack.ser.close()