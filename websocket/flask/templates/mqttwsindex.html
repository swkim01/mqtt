<html>
<head>
  <script src="mqttws31.js"></script>
  <script src="//code.jquery.com/jquery-1.11.1.min.js"></script>
  <script>
    var host="{{host}}";
    var port="{{port}}";
    var wsClient=null;
    var clientId = "clientId";
    var mqttconnected = false;

    function wsConn() {
        wsClient = new Paho.MQTT.Client(host, Number(port), clientId);
        var options = { useSSL: false, onSuccess: this.onConnect, onFailure: this.onFail };
        wsClient.onConnectionLost = onConnectionLost;

        wsClient.connect(options);
    }

    function onConnect() {
        console.log("[@.@] connecting...");
        mqttconnected = true;
    }
    function onFail() {
        console.log("[@.@] connect failed");
        mqttconnected = false;
    }
    function onConnectionLost(responseObject) {
        if (responseObject.errorCode !== 0) {
            mqttconnected = false;
            console.log("onConnectionLost:"+responseObject.errorMessage);
        }
    }

    $(document).ready(function() {
        wsConn();
    });
    
    var ledTopic = "led";
    var value = 0;
    function ledControl() {
      value = !value;
      var send =(value != 0 ? "ON":"OFF");
      var msg = new Paho.MQTT.Message(send);
      msg.qos = 2;
      msg.destinationName = ledTopic;
      wsClient.send(msg);
    }

    function dhtChart() {
       window.location.href='/mqttwschart';
    }
  </script>
</head>
<body>
<h1>MQTT Websocket 제어</h1>
<h2>LED 제어</h2>
<input type='button' onClick="ledControl()" value="ON/OFF" />
<h2>온도/습도 모니터링</h2>
<input type='button' onClick="dhtChart()" value="모니터링" />
</body>
</html>
