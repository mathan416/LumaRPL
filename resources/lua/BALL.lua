-- Compact canvas text, 16-pixel line spacing.
local x, y = 160, 160
while true do
  if keyboard.down("left") then x = math.max(10, x - 4) end
  if keyboard.down("right") then x = math.min(309, x + 4) end
  if keyboard.down("up") then y = math.max(35, y - 4) end
  if keyboard.down("down") then y = math.min(280, y + 4) end
  if keyboard.poll() == "q" then break end
  screen.clear("black")
  screen.text(8, 8, "Arrows move | Q finish | Esc stop", "white")
  screen.circle(x, y, 10, "cyan", true)
  screen.show()
  system.sleep(16)
end
print("Animation finished.")
