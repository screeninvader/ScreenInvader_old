#!/lounge/bin/janosh -f

function receive(handle, message)
   if message == 'setup' then
      print("new client")
      Janosh:wsSend(handle, janosh_get({"/."}))
   else
      print("received", message)
      janosh_request(JSON:decode(message))
   end
end

function push(key, value,op)
   print('push updates')
   Janosh:wsBroadcast(janosh_get({"/."}))
end

Janosh:wsOpen(8082)
Janosh:wsOnReceive(receive)
Janosh:subscribe("/", push)

while true do
   Janosh:sleep(1000) -- milliseconds
end
