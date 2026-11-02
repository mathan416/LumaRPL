print("Hello from Lua on PicoCalc")
local total = 0
for i = 1, 100 do total = total + i end
print("Sum 1..100:", total)
print("Lua square root:", math.sqrt(81))
print("C99 hex float:", string.format("%a", 0.5))
print("DB48X exact fraction:", calc.eval("1/3"))
print("DB48X power:", calc.eval("2^10"))
