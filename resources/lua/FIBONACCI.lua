local a, b = 0, 1
for n = 1, 20 do
  print(n, a)
  a, b = b, a + b
end
