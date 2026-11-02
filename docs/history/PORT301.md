# LumaRPL port 30.1 — direct number-base conversion

From the calculator, Ctrl+Y opens Bases. Source editors keep Ctrl+Y for Redo.

Enter `#FFD2h` and press Enter. Type `Bin` and press Enter: the value displays as binary `1111111111010010`. `Oct` shows `177722`, `Dec` shows `65490`, and `Hex` returns to `FFD2`. Display grouping and subscript base markers follow calculator settings.

On LumaRPL, Bin/Oct/Dec/Hex also make a fixed-base integer at the top of the stack follow the selected display base. The integer's payload is copied exactly, including large integers. Fixed-base values below the top retain their explicit representation. Generic based integers elsewhere continue to follow the global base, as before. These commands leave ordinary decimal values unchanged; use RealToBinary first. BinaryToReal returns a based value to an ordinary integer.

In the usual three-row Bases menu, Tab selects the row containing Base, Bin, Oct, Dec and Hex. F2 selects Bin, F3 Oct, F4 Dec and F5 Hex. Esc returns one menu level. The sixth soft key may be a page-navigation key. Always follow the displayed labels if you changed menu appearance or page.

The calculator reference now includes Binary, octal, decimal and hex, and the four base commands have worked examples. Ctrl+A (Command Catalogue), Ctrl+R (Advanced RPL settings) and the editor shortcuts remain available.

For the currently deployed port 30, the equivalent workaround is `BinaryToReal RealToBinary Bin` after entering a fixed-base number. Port 30.1 removes that extra round trip for the four display-base commands.

This update preserves calculator, heap, stack, framebuffer and workspace reservations. It does not contain the proposed implicit-graph/optimization performance changes. Deployment and physical acceptance are separate actions.
