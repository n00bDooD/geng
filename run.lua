#!/usr/bin/env lua5.1

L = require('linenoise')
socket = require('socket')

L.historyload('~/.gengdbg_history')

-- os.execute('./geng 1>geng.stdout 2>geng.stderr &')

-- os.execute('sleep 0.5')

local client = socket.tcp()

res, err = client:connect('127.0.0.1', 4321)
if res == nil then
	io.stderr:write(err .. '\n')
	os.execute('killall -9 geng')
	os.exit(-1)
end

while true do
	local debuginfo, err = client:receive()
	if debuginfo == nil then
		if err == 'closed' then
			io.write('Debuggee closed the connection\n')
			os.exit()
		elseif err == 'timeout' then
			io.write('Timed out while waiting for debuggee\n')
		end
	end
	io.write(debuginfo .. '\n')

	local resultresponse = ''
	while resultresponse ~= 'continuing' do
		local line = L.linenoise('gengdbg> ')

		L.historyadd(line)
		L.historysave('~/.gengdbg_history')
	
		client:send(line .. '\n')

		resultresponse, err = client:receive()
		if resultresponse ~= nil and resultresponse ~= "" then
			io.write(resultresponse .. '\n')
		end
	end
end

