#-*- coding: utf-8 -*-
# Make sure your gevent version is >= 1.0
import gevent
from gevent import monkey; monkey.patch_all()
from gevent import sleep

from gevent.queue import Queue

from bottle import get, post, request, response, template
from bottle import GeventServer, run

import paho.mqtt.client as mqtt
import threading, time, json

host = "192.168.0.31"
port = 8008
subscriptions = []

class MqttConnector(threading.Thread):
    def __init__(self, host, port):
        threading.Thread.__init__(self)
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.host = host
        self.port = port
        self.client.connect(self.host, self.port, 60)
        self.running = True

    def run(self):
        try:
            while self.running:
                self.client.loop_forever()

        except (KeyboardInterrupt, SystemExit): #when you press ctrl+c
            print "\nKilling Thread..."
            self.running = False
        except StopIteration:
            self.client = None

    def on_connect(self, client, userdata, flags, rc):
        print("Connected with result code" + str(rc))

        #client.subscribe("$SYS/#")
        client.subscribe("events")

    def on_message(self, client, userdata, msg):
        print(msg.topic+" "+str(msg.payload))
        def notify():
            js = json.JSONDecoder()
            print(msg.payload)
            pl = js.decode(str(msg.payload))
            pl["published_at"] = time.time() * 1000
            for sub in subscriptions[:]:
                sub.put(json.dumps(pl))
    
        gevent.spawn(notify)

    def publish(self, topic, msg):
        self.client.publish(topic, msg)


# SSE "protocol" is described here: http://mzl.la/UPFyxY
class ServerSentEvent(object):

    def __init__(self, data):
        self.data = data
        self.event = None
        self.id = None
        self.desc_map = {
            self.data : "data",
            self.event : "event",
            self.id : "id"
        }

    def encode(self):
        if not self.data:
            return ""
        lines = ["%s: %s" % (v, k) 
                 for k, v in self.desc_map.iteritems() if k]
        
        return "%s\n\n" % "\n".join(lines)

@get("/publish")
def publish():
    #Dummy data - pick up from request for real data
    def notify():
        msg = str(time.time())
        for sub in subscriptions[:]:
            sub.put(msg)
    
    gevent.spawn(notify)
    
    return "OK"

@post("/led")
def controlled():
    l = request.body.read()
    mqc.publish("led", l)

@get("/events")
def getevents():

    response.content_type  = 'text/event-stream'
    response.cache_control = 'no-cache'

    # Set client-side auto-reconnect timeout, ms.
    yield 'retry: 100\n\n'

    q = Queue()
    subscriptions.append(q)
    try:
        while True:
            result = q.get()
            ev = ServerSentEvent(str(result))
            yield ev.encode()
    except GeneratorExit: # Or maybe use flask signals
        subscriptions.remove(q)

@get("/ssechart")
def ssechart():
    return template("ssechart", host=host, port=port)
    #return template("debug")

@get("/")
def index():
    return template("index")

if __name__ == "__main__":
    mqc = MqttConnector(host, port=1883)
    mqc.daemon = True
    mqc.start()

    run(host=host, port=port, server=GeventServer)
