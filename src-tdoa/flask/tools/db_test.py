# This script inserts a new coordinate location into the db every 10th of a second.

import sqlite3
import time
import random

def create_connection(db_file):
    """ connect to db """
    conn = None
    try:
        conn = sqlite3.connect(db_file)
        return conn
    except sqlite3.Error as e:
        print(e)
    return conn

def insert_coordinate(conn, x, y):
    """ insert new coordinate into coordinates table """
    sql = ''' INSERT INTO coordinates(x, y)
              VALUES(?,?) '''
    cur = conn.cursor()
    cur.execute(sql, (x, y))
    conn.commit()
    return cur.lastrowid

def main():
    database = "../cid.db"

    # connect to db
    conn = create_connection(database)
    
    if conn is not None:
        try:
            while True:
                # gen random coordinates
                x = random.randint(0, 960)  # temp ranges for display area
                y = random.randint(0, 750)  

                # insert new oordinate
                insert_coordinate(conn, x, y)
                
                # print to console for confirmation
                print(f"Inserted coordinates ({x}, {y})")

                time.sleep(0.1)
        except KeyboardInterrupt:
            print("Stopped by user.")
        finally:
            conn.close()
    else:
        print("Error! Cannot create the database connection.")

if __name__ == '__main__':
    main()
