from flask import Flask, jsonify, render_template
import sqlite3
import time

app = Flask(__name__)

def get_coordinates():
    """ fetch the latest coordinates from DB. """
    conn = sqlite3.connect('cid.db')
    cursor = conn.cursor()
    cursor.execute("SELECT x, y FROM coordinates ORDER BY id DESC LIMIT 1")
    coord = cursor.fetchone()
    conn.close()
    return {'x': coord[0], 'y': coord[1]} if coord else {'x': 0, 'y': 0}

@app.route('/coordinates')
def coordinates():
    """ get latest coordinates. """
    time.sleep(1)
    return jsonify(get_coordinates())

@app.route('/')
def index():
    """ servive HTML """
    return render_template('index.html')

if __name__ == '__main__':
    app.run(port=8081)
    #app.run(debug=True)