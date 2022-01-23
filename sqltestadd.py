import sqlite3

connection = sqlite3.connect('db.sqlite3')
cursor = connection.cursor()


# Read table
rows = cursor.execute("SELECT id, track_length FROM sandtable_queue").fetchall()

# track = rows[0][0]
# length = rows[0][1]
print(rows)


# Delete a row
cursor.execute("DELETE FROM sandtable_queue WHERE id = ?", (rows[0][0],))
connection.commit()


# Inser data into a table
#cursor.execute("INSERT INTO sandtable_queue VALUES (?, ?)", (track, length))
# Needed to save data to sql
#connection.commit()


# Create table
#cursor.execute("CREATE TABLE sandtable_queue (file TEXT, length INTEGER)")


# Delete table
#cursor.execute("DROP TABLE sandtable_queue")