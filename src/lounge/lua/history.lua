#!/lounge/bin/janosh -f

function write_history(key, operation, value)
   if key:find("source$") and operation == 'W' then
      fp = io.open('/lounge/history.log', 'a')
      fp:write(os.time(), '|', value, '\n')
      fp:close()
   end
end

Janosh:subscribe("/playlist/items", write_history)
while true do
  Janosh:sleep(1000) -- milliseconds
end
