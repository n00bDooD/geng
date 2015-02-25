depth = 0

function mkcolour(val)
	return string.char(27) .. '[' .. tostring(val) .. 'm';
end

function printc(obj, indent)
	if depth > 2 then io.write(indent .. mkcolour(31) .. 'In too deep!' .. mkcolour(0) .. '\n'); return end
	if type(indent) == 'nil' then
		indent = '';
	end

	if type(obj) == 'nil' then
		io.write(indent .. '[nil]');
	elseif type(obj) == 'table' then
		for k, v in pairs(obj) do
			io.write(indent .. '[' .. tostring(k) .. ']' .. ':');
			if type(v) == 'table' then io.write('\n') end
			if type(v) == 'table' then depth = depth + 1 end
			printc(v, indent .. '\t');
			if type(v) == 'table' then depth = depth - 1 end
		end
	elseif type(obj) == 'function' then
		obj()
	else
		io.write(tostring(obj));
	end
	if type(obj) ~= 'table' then io.write('\n') end
end
