#-*- coding: utf-8 -*-
from flask import Flask, render_template
#import json

host="<HOST IP>"
port=8008
wsport=9001

app = Flask(__name__)

@app.route('/mqttws31.js')
def mqttws31():
    return app.send_static_file("mqttws31.js")

@app.route('/mqttwschart')
def dht22chart():
    return render_template("mqttwschart.html", host=host, port=wsport)

@app.route('/')
def index():
    return render_template("mqttwsindex.html", host=host, port=wsport)

if __name__ == '__main__':
    app.run(host=host, port=port)
