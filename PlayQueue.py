import sqlite3
import playTrack


def PlayQ():
    connection = sqlite3.connect('db.sqlite3')
    cursor = connection.cursor()

    # Read table
    rows = cursor.execute("SELECT id, file, track_length, pic FROM sandtable_queue").fetchall()

    while rows:
        file = rows[0][1]
        length = rows[0][2]

        playTrack.playTrack(file)

        # Delete a row
        cursor.execute("DELETE FROM sandtable_queue WHERE id = ?", (rows[0][0],))
        connection.commit()

        # Read table
        rows = cursor.execute("SELECT id, file, track_length, pic FROM sandtable_queue").fetchall()



if __name__ == "__main__":
    PlayQ()
    playTrack.ser.close()