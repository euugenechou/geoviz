from flask import Flask, render_template, request
import os, json

app = Flask(__name__)
app.config["key"] = os.getenv("GMAPSKEY")

# Home route to display main page.
@app.route("/", methods=["GET"])
def index():
    context = { "key": app.config["key"] }
    return render_template("index.html", context=context)

# Data route to send bounds of Google Maps JS api to.
@app.route("/data", methods=["POST"])
def data():
    bounds = json.loads(request.form["data"])
    for k, v in bounds.items():
        print(f"{k}: {v}")
    return "200", "OK"

if __name__ == "__main__":
    app.run(debug=True)
