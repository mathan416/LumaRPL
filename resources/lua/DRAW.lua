-- LumaRPL: compact text is the default; screen.font("legacy") restores old metrics.
screen.clear("black")
screen.line(0, 0, 319, 319, "cyan")
screen.rect(20, 35, 110, 70, "orange", false)
screen.circle(220, 110, 40, "green", true)
local title = "Lua colour graphics"
local width = screen.measure(title)
screen.text((screen.width - width) / 2, 220, title, "white")
screen.text(20, 245, "320 x 320 pixels", "yellow")
screen.show()
print("Lines, rectangles, circles and text drawn.")
