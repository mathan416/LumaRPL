for _, note in ipairs({440, 554, 659, 880}) do
  sound.beep(note, 120)
  system.sleep(40)
end
print("Four tones played.")
