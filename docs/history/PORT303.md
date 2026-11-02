# LumaRPL port 30.3 — direct decimal integer conversion

From the calculator, press Ctrl+Y to open Bases. Tab selects the base row: F2 Bin, F3 Oct, F4 Dec and F5 Hex. Typed commands work too.

These all represent the same value:

|Input|Value|
|---|---|
|`#FFD2h`|Hexadecimal|
|`#177722o`|Octal|
|`#1111111111010010b`|Binary|
|`65490`|Ordinary decimal integer|

Enter any one and press Enter, then type Bin, Oct, Dec or Hex and press Enter. The top integer follows the selected base without changing its value. For example, `42 Bin` displays binary 101010; Hex then shows 2A; Oct shows 52; Dec shows 42. BinaryToReal restores an ordinary decimal integer object.

Port 30.1/30.2 already support explicit binary, octal, decimal and hex literals. Port 30.3 adds ordinary decimal integers, including large integers, so no RealToBinary step is needed. Negative integers follow the existing RealToBinary word-size/two's-complement behavior; set WordSize deliberately. Decimal fractions such as 1.5 remain unchanged: these are integer-base commands, not fractional-radix formatting.

Only the top stack value is converted. Lower stack entries are preserved. With an empty stack, these commands still select the display base. Source editors keep Ctrl+Y for Redo.

The calculator memory reservations and port 30.2 performance improvements remain. Host verification and physical acceptance are separate; no SD deployment is implied.
