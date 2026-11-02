LumaRPL colour sprites
16x16 indexed RGB332 pixels plus a separate transparency mask.
Programming > Sprite editor: F1 Save, F2 Colour, F4 Tools, F5 Back.
Space/Enter paints; Backspace erases. CHIP-8 mode toggles a monochrome mask.
Tools > Open sprite loads .sprite files; Export CHIP-8 mask creates assembly.
Lua: assert(screen.sprite("DEMO.sprite", 40, 60)); screen.show()
The API returns true on success, or nil,error; integer positions clip to screen.
Use a plain filename ending .sprite; paths outside /db48x/sprites are rejected.
Wire format (304 bytes): LUMASPR + NUL (8), version 1, width 16, height 16,
reserved 0, 256 RGB332 bytes, 32 opacity-mask bytes (LSB first, row-major),
CRC32 IEEE little-endian of the preceding 300 bytes. Unknown versions rejected.
