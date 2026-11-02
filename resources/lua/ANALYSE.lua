-- Press F2 in this native view for roots, intersections, extrema and calculus.
plot.clear()
plot.window(-3, 3, -5, 6)
plot.function2d(function(x) return x*x-4 end,
                {colour="cyan", label="x^2 - 4"})
plot.function2d(function(x) return x end,
                {colour="orange", label="x"})
plot.show()
print("Analysis demo finished.")
