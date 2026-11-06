print(calc.eval("1/3"))
local ok,message=pcall(function() return calc.eval("'X'") end)
print(ok,type(message))
