led = 2
dht22 = 4

-- wifi.setmode(wifi.STATION)
-- wifi.sta.config("<SSID>","<PASSWORD>")

gpio.mode(led,gpio.OUTPUT)

m_dis={}
function dispatch(m,t,pl)
   if pl~=nil and m_dis[t] then
      m_dis[t](m,pl)
   end
end
function ledcontrol(m,pl)
   print("led: "..pl)
   if pl == 'ON' then
      gpio.write(led, gpio.HIGH)
   else
      gpio.write(led, gpio.LOW)
   end
end
m_dis["led"]=ledcontrol

m=mqtt.Client()
m:on("connect",function(m)
   print("connection "..node.heap())
   m:subscribe("led",0,function(m) print("sub done") end)
end)
m:on("offline", function(conn)
   print("disconnect to broker...")
   print(node.heap())
end)
m:on("message",dispatch)

m:connect("192.168.0.31",1883,0,1)

tmr.alarm(0,10000,1,function()
   s,t,h,td,hd=dht.read(dht22)
   local json = "{" .. "\"temperature\": " .. (t) .. ", "
   json = json .. "\"humidity\": " .. (h) .. " }"
   print("dht: "..json)

   m:publish("events", json,0,0)
end)
