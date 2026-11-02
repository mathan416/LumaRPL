-- Native Plot 2D: arrows trace/select, F1 table, F4 pan, +/- zoom.
plot.clear()
plot.window(-6.3, 6.3, -2, 2)
plot.function2d(function(x) return math.sin(x) end,
                {colour="cyan", label="sin(x) [radians]"})
plot.function2d(function(x) return math.cos(x) end,
                {colour="orange", label="cos(x) [radians]"})
plot.show()
print("Returned from native Plot 2D")
