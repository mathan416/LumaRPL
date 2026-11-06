screen.clear("black")
local previous=screen.font("compact")
local label="LumaRPL API"
local w,h=screen.measure(label)
screen.text(math.floor((320-w)/2),20,label,"cyan")
screen.rect(8,16,304,h+8,"gray",false)
screen.font(previous)
screen.show()
keyboard.wait()
