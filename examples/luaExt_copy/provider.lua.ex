t = {}
t.number = 20
t.string = [[
This is a multiline string with
embedded \ within]]
t.func = function() io.write('Hello in func\n') end
t.table = {}
t.table.ay_yo = 42
t.table.stackoverflow = t
t.table.after_the_overflow = 'asfasdf'

keyfunc = function() io.write(math.pow(42, 2) .. '\n') end

t[keyfunc] = 'This is a function as a key.. fuck logix'

t[1] = 'Lets'
t[2] = 'break'
t[3] = 'this'
t[4] = 'copy-function'
t[5] = '!!\n'

t.onetable = {}
t.onetable.foo = "swoo"
t.onetable.bar = "car"
t.onetable.baz = "haz"

t[t.onetable] = true;

t.twotable = t.onetable

m.docopy(t);
