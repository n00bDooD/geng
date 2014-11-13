local a = vector.new(-1, 1);
io.write('negate: ', a:negate():tostring(), '\n');

local b = vector.new(1, 1);
io.write('normalize: ', a:normalize():tostring(), '\n');

io.write('length: ', b:length(), '\n');
io.write('lengthsqr: ', b:lengthsqr(), '\n');
io.write('#: ', #a , '\n');

local d = vector.new(28, 28)
io.write('rotate: ', d:rotate(a):tostring(), '\n');
io.write('unrotate: ', d:rotate(a):unrotate(a):tostring(), '\n');

local c = vector.new(1,1)
local e = vector.new(10,10)
io.write('lerp: ', c:lerp(e, 0.5):tostring(), '\n');
io.write('lerpconst: ', c:lerpconst(e, 0.5):tostring(), '\n');
io.write('slerp: ', c:slerp(e, 0.5):tostring(), '\n');
io.write('slerpconst: ', c:slerpconst(e, 0.5):tostring(), '\n');

io.write('clamp: ', vector.new(2, 2):clamp(1, 5):tostring(), '\n');

io.write('add: ', (vector.new(5,5) + vector.new(10,10)):tostring(), '\n')
io.write('sub: ', (vector.new(5,5) - vector.new(10,10)):tostring(), '\n')
io.write('mul: ', (vector.new(5,5) * 10):tostring(), '\n')

io.write('eq: ', tostring(vector.new(5,5) == vector.new(10,10)), '\n')
io.write('eq: ', tostring(vector.new(5,5) == vector.new(5,5)), '\n')

