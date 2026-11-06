plot.clear()
plot.window(-4,4,-1,16)
plot.function2d(function(x) return x*x end,
  {colour="cyan",label="Square"})
plot.function2d(function(x)
  if x<=0 then return nil end
  return math.log(x)
end,{colour="orange",label="Log"})
plot.show()
print("Back from graph")
