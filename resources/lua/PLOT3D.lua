-- Same native surface renderer as Graphs -> 3D surfaces.
-- Arrows rotate, +/- zoom, F1 resets the camera, Esc returns.
plot.clear()
plot.surface(function(x, y) return x*x - y*y end,
             {xmin=-3, xmax=3, ymin=-3, ymax=3})
plot.show()
print("Returned from native 3D surface")
