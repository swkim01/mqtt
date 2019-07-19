#-*- coding: utf-8 -*-
from bottle import route, get, request, template, response, static_file
from bottle import run
#import json

host="<HOST IP>"
port=8008
wsport=9001

@route('/mqttws31.js')
def mqttws31():
    return static_file("mqttws31.js", root=".")

@get('/mqttwschart')
def dht22chart():
    return template("mqttwschart", host=host, port=wsport)

@get('/')
def index():
    return template("mqttwsindex", host=host, port=wsport)

if __name__ == '__main__':
    run(host=host, port=port)
