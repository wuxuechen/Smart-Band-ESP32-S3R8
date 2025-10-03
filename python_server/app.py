import os

from flask import Flask, render_template, request, redirect, Response, send_file
import sqlite3
import requests
import threading
import json
from datetime import datetime

app = Flask(__name__)

# Global database connection
db_conn = None
# Example gait data (in practice you'd replace this with real sensor values)
gait_data = {
    "user_id": 1,
    "username": "Alice",
    "stride": 0.75,
    "cadence": 120,
    "pace": 6.5,
    "steps": 3500
}

DB_FILE = "gait_data.db"


def init_db():
    conn = sqlite3.connect(DB_FILE)
    cur = conn.cursor()
    cur.execute("""
        CREATE TABLE IF NOT EXISTS gait_metrics (
            user_id INTEGER,
            username TEXT,
            date TEXT,
            stride REAL,
            cadence REAL,
            pace REAL,
            steps INTEGER,
            PRIMARY KEY(user_id, date)
        )
    """)
    conn.commit()
    conn.close()


def init_db():
    conn = sqlite3.connect(DB_FILE)
    cur = conn.cursor()
    cur.execute("""
        CREATE TABLE IF NOT EXISTS gait_metrics (
            user_id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT,
            date TEXT,
            stride REAL,
            cadence REAL,
            pace REAL,
            steps INTEGER,
            UNIQUE(username, date)  -- prevent duplicate username/date
        )
    """)
    conn.commit()
    conn.close()


def save_gait_to_db(data):
    conn = sqlite3.connect(DB_FILE)
    cur = conn.cursor()
    today = datetime.now().strftime("%Y-%m-%d")

    # Try update first (by username + date)
    cur.execute("""
        UPDATE gait_metrics
        SET stride=?, cadence=?, pace=?, steps=?
        WHERE username=? AND date=?
    """, (data["stride"], data["cadence"], data["pace"], data["steps"],
          data["username"], today))

    if cur.rowcount == 0:
        cur.execute("""
            INSERT INTO gait_metrics (username, date, stride, cadence, pace, steps)
            VALUES (?, ?, ?, ?, ?, ?)
        """, (data["username"], today, data["stride"], data["cadence"], data["pace"], data["steps"]))

    conn.commit()
    conn.close()


weather_data = {}

# Lock for thread-safe access
weather_lock = threading.Lock()


# Priority function for weatherDesc
def get_priority(desc):
    desc = desc.lower()
    if "rain" in desc:
        return 0
    elif "cloud" in desc:
        return 1
    elif "wind" in desc:
        return 2
    else:
        return 3  # sun/clear


# Parse raw wttr.in JSON to minimal structure
def parse_weather(raw_json):
    result = {}

    # Current condition
    current = raw_json["current_condition"][0]
    result["current_condition"] = {
        "temp_C": current["temp_C"],
        "weatherDesc": current["weatherDesc"][0]["value"]
    }

    # Today
    today = raw_json["weather"][0]
    hourly_desc = [h["weatherDesc"][0]["value"] for h in today["hourly"]]
    main_desc = sorted(hourly_desc, key=get_priority)[0]
    result["today"] = {
        "max_temp_C": today["maxtempC"],
        "min_temp_C": today["mintempC"],
        "weatherDesc": main_desc
    }

    # Future days
    future_days = []
    for day in raw_json["weather"][1:]:
        hourly = day["hourly"]
        # Find highest and lowest temp (hourly)
        high = max(hourly, key=lambda h: int(h["tempC"]))
        low = min(hourly, key=lambda h: int(h["tempC"]))
        # Main weather description based on priority
        day_descs = [h["weatherDesc"][0]["value"] for h in hourly]
        main_day_desc = sorted(day_descs, key=get_priority)[0]

        future_days.append({
            "date": day["date"],
            "max_temp_C": day["maxtempC"],
            "min_temp_C": day["mintempC"],
            "weatherDesc": main_day_desc
        })

    result["future_days"] = future_days
    return result


# Background weather fetcher
def fetch_weather():
    global weather_data
    try:
        r = requests.get("https://wttr.in/Manila?format=j1", timeout=10)
        r.raise_for_status()
        raw = r.json()
        parsed = parse_weather(raw)
        print(parsed)
        with weather_lock:
            weather_data = parsed
        print("Weather updated successfully.")

    except requests.RequestException as e:
        print("Weather fetch error:", e)

    # Schedule next fetch in 5 minutes (300 seconds)
    threading.Timer(300, fetch_weather).start()


@app.route("/weather")
def weather():
    with weather_lock:
        if weather_data:
            return Response(json.dumps(weather_data), mimetype="application/json")
        else:
            return {"error": "Weather data not ready"}, 503


@app.route("/gait", methods=["POST"])
def gait():
    try:
        data = request.json
        print(data)
        if not data:
            return {"error": "Invalid JSON"}, 400

        # expected fields (no user_id): username, stride, cadence, pace, steps
        required = ["username", "stride", "cadence", "pace", "steps"]
        for field in required:
            if field not in data:
                return {"error": f"Missing field {field}"}, 400

        # Save or update gait data
        save_gait_to_db(data)

        # Get today's date
        today = datetime.now().strftime("%Y-%m-%d")

        # Fetch top 10 for today only (by steps)
        conn = sqlite3.connect(DB_FILE)
        cur = conn.cursor()
        cur.execute("""
            SELECT username, steps
            FROM gait_metrics
            WHERE date = ?
            ORDER BY steps DESC
            LIMIT 9
        """, (today,))
        rows = cur.fetchall()
        conn.close()

        top9 = []
        for row in rows:
            top9.append({
                "username": row[0],
                "steps": row[1]
            })

        # Safely read weather
        with weather_lock:
            current_weather = weather_data if weather_data else "NA"
        v_serial = last_bin()[:-16]
        version2return = v_serial[:2] + '.' + v_serial[2:6] + '.' + v_serial[6:]

        # Return both gait and weather together
        return {"status": "success", "top9": top9, "weather": current_weather, "version": version2return}, 200

    except Exception as e:
        return {"error": str(e)}, 500


def last_bin():
    last_file = ''
    for file in os.listdir('.'):
        if file.endswith('.bin'):
            if last_file == '':
                last_file = file
            else:
                if file[:-16]>last_file[:-16]:
                    last_file = file
    return last_file


@app.route("/TUPSmartBand.bin", methods=["GET"])
def serve_firmware():
    return send_file(last_bin(), mimetype="application/octet-stream")

if __name__ == "__main__":
    init_db()
    fetch_weather()
    app.run(host="127.0.0.1", port=5000, threaded=True, debug=True)
