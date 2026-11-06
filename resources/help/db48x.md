# LumaRPL command reference

Browse commands by subject, or use Find. These pages use the same arrows and Back key as the rest of Help.

## AlgebraConfiguration

Return the calculator directory used for algebra configuration.

You can also enter: Ⓓ, CASDir.

## AlgebraVariable

Recall the current algebra variable.

You can also enter: ⓧ, VX, RclVX.

## Apply

Apply a supplied symbolic transformation to an expression.

## Collect

Collect matching terms in an algebraic expression.

Try it
'X+X' COLLECT

You should get: '2·X'

## Derivative

Differentiate an expression with respect to a quoted variable. Example: 'X^2' 'X' Derivative gives 2*X.

You can also enter: ∂.

Try it
'X^3' 'X' Derivative

You should get: '3·X²'

## Equation

Use the supplied equation definition in the engine equation/library context.

You can also enter: Eq.

## EvalEq

Evaluate the current engine equation.

You can also enter: EvaluateEquation.

## Expand

Expand products and powers in an algebraic expression.

You can also enter: Expan.

Try it
'(X+1)^2' EXPAND

You should get: 'X²+2·X+1'

## FoldConstants

Combine the constant parts of an expression.

## FromPolynomial

Convert a polynomial object back to an algebraic expression.

You can also enter: Polynomial→, Poly→.

## Integrate

Numerically integrate an expression over the supplied variable and bounds. Home > Math & graphs > Calculus provides a guided definite integral.

You can also enter: ∫.

Try it
Rad 0 1 'X^2' 'X' Integrate

You should get: ¹/₃

## Isolate

Rearrange an equation to isolate the supplied variable. Some equations cannot be isolated by the engine.

You can also enter: Isol.

Try it
'X+2=5' 'X' ISOL

You should get: 'X=3'

## MatchDown

Apply a symbolic rewrite while traversing the expression downward.

You can also enter: ↓Match.

## MatchUp

Apply a symbolic rewrite while traversing the expression upward.

You can also enter: ↑Match.

## MultipleEquationsSolver

Solve several equations numerically using the supplied variables and guesses. Ctrl+W > Equation system is the guided alternative.

You can also enter: MRoot.

## MultipleVariablesSolver

Solve using the supplied equation, variables and starting values through the engine multi-variable solver.

You can also enter: MSlv.

## NextEq

Select the next equation in the engine equation set.

You can also enter: NextEquation.

## PartFrac

Decompose a rational expression into partial fractions where supported.

You can also enter: PartialFractions.

## PCoef

Return polynomial coefficients using the engine's coefficient order.

You can also enter: PolynomialCoefficients.

Try it
CompatiblePolynomials
[ 2 -3 4 -5 ] PCoef

You should get: [ 1 2 -25 -26 120 ]
Run from the calculator stack. This example uses the current calculation settings.

## PEval

Evaluate a polynomial using the supplied argument.

You can also enter: PolynomialEvaluation.

Try it
[ 1 -5 6 ] 2 PEval

You should get: 0
Run from the calculator stack. This example uses the current calculation settings.

## Primitive

Find a symbolic antiderivative. The arbitrary constant is implicit.

You can also enter: Risch.

Try it
'X^2' 'X' Primitive

You should get: 'X³÷3'

## Product

Evaluate a product over an expression, variable and bounds.

You can also enter: ∏.

## PRoot

Find roots of the supplied polynomial using the polynomial solver.

You can also enter: PolynomialRoots.

Try it
[ 1 2 -25 -26 120 ] PRoot

You should get: [ -5 -3 2 4 ]
Run from the calculator stack. This example uses the current calculation settings.

## RcEq

Recall the current engine equation.

You can also enter: RecallEquation.

## ReorderTerms

Put expression terms into the engine's canonical order.

## Root

Find a local numerical solution. Supply an expression, quoted variable and initial guess. Ctrl+W offers the guided solver.

Try it
Rad 16 Precision 'COS(X)-X' 'X' 1 Root

You should get: X=0.73908 51332 15
Finds one local solution; it does not list every possible root.

## Simplify

Simplify an expression using the current algebra rules.

## StEq

Store the supplied equation as the engine's current equation.

You can also enter: StoreEquation.

## StoreAlgebraVariable

Set the algebra variable from the supplied name.

You can also enter: Storeⓧ, StoVX.

## Subst

Substitute the supplied replacement into an expression without assigning a calculator variable.

You can also enter: Substitute.

Try it
'X+2' 'X=3' SUBST

You should get: '3+2'

## Sum

Evaluate a summation from an expression, variable and bounds.

You can also enter: Σ.

## ToPolynomial

Convert an expression to the engine polynomial representation.

You can also enter: →Polynomial, →Poly.

## TrigSin

Rewrite trigonometric expressions in terms of sine where supported.

You can also enter: TRIGSIN.

## Zeros

Find zeros using the engine symbolic/polynomial operation. A result is subject to the expression and solver limitations.

## ASR

Shift a based integer right while preserving its sign bit.

You can also enter: ArithmeticShiftRight.

## ASRB

Shift a based integer right by one byte while preserving its sign bit.

You can also enter: ArithmeticShiftRightByte.

## ASRC

Shift a based integer right by the supplied count while preserving its sign.

You can also enter: ArithmeticShiftRightCount.

## ClearBit

Clear the requested bit in a based integer.

Try it
#Fh 0 ClearBit

You should get: #E₁₆

## CountBits

Count the set bits of an integer.

Try it
#Fh CountBits

You should get: 4

## Equiv

Test logical equivalence of the supplied operands.

You can also enter: equiv.

Try it
True True Equiv

You should get: True

## Excludes

Test the engine logical exclusion relation for the supplied operands.

You can also enter: excludes.

Try it
True True Excludes

You should get: False

## FirstBitSet

Find the first set bit using the engine's bit-position convention.

Try it
#8h FirstBitSet

You should get: 3

## FlipBit

Toggle the requested bit in a based integer.

Try it
#0h 2 FlipBit

You should get: #4₁₆

## Implies

Test logical implication from the first operand to the second.

You can also enter: implies.

Try it
True False Implies

You should get: False

## LastBitSet

Find the last set bit using the engine's bit-position convention.

Try it
#8h LastBitSet

You should get: 3

## NAnd

Return NOT-AND for the supplied operands.

You can also enter: nand.

Try it
True True NAnd

You should get: False

## NOr

Return NOT-OR for the supplied operands.

You can also enter: nor.

Try it
False False NOr

You should get: True

## RecallWordSize

Put the current based-integer word size on the stack.

You can also enter: rcws.

## RL

Rotate the bits of a based integer left within WordSize.

You can also enter: RotateLeft.

Try it
#1h RL

You should get: #2₁₆

## RLB

Rotate a based integer left by one byte.

You can also enter: RotateLeftByte.

## RLC

Rotate a based integer left by the supplied count.

You can also enter: RotateLeftCount.

## RR

Rotate the bits of a based integer right within WordSize.

You can also enter: RotateRight.

Try it
#2h RR

You should get: #1₁₆

## RRB

Rotate a based integer right by one byte.

You can also enter: RotateRightByte.

## RRC

Rotate a based integer right by the supplied count.

You can also enter: RotateRightCount.

## SetBit

Set the requested bit in a based integer.

Try it
#0h 3 SetBit

You should get: #8₁₆

## SL

Shift a based integer left, filling vacated bits with zero.

You can also enter: ShiftLeft.

Try it
#1h SL

You should get: #2₁₆

## SLB

Shift a based integer left by one byte.

You can also enter: ShiftLeftByte.

Try it
#1h SLB

You should get: #100₁₆

## SLC

Shift a based integer left by the supplied count.

You can also enter: ShiftLeftCount.

## SR

Shift a based integer right, filling vacated bits with zero.

You can also enter: ShiftRight.

Try it
#8h SR

You should get: #4₁₆

## SRB

Shift a based integer right by one byte.

You can also enter: ShiftRightByte.

Try it
#100h SRB

You should get: #1₁₆

## SRC

Shift a based integer right by the supplied count.

You can also enter: ShiftRightCount.

## EditorBegin

Move to the start of calculator entry.

## EditorClear

Delete selected calculator-entry text. With no selection, nothing is deleted.

## EditorCopy

Copy selected calculator-entry text to the calculator clipboard.

## EditorCut

Cut selected calculator-entry text to the calculator clipboard.

## EditorEnd

Move to the end of calculator entry.

## EditorFlip

Exchange the calculator selection anchor and cursor so you can extend the other end of the selection.

## EditorHistory

Recall an entry from calculator command history.

## EditorHistoryBack

Move through calculator command history in the opposite direction.

## EditorPaste

Insert the calculator clipboard contents into calculator entry.

## EditorReplace

Replace the active calculator search selection with clipboard text and continue searching. An active search, selection and clipboard are needed. For an SD source file, use Ctrl+R.

## EditorSearch

Start or repeat a search within calculator entry. In an SD source file, use Ctrl+F.

## EditorSelect

Mark a selection anchor in calculator entry. Moving the cursor then changes the selection. In an SD source file, use Ctrl+K to mark text instead.

## EditorWordLeft

Move the calculator-entry cursor one word left.

## EditorWordRight

Move the calculator-entry cursor one word right.

## UIAlgebraic

Invoke the engine algebraic-entry key. Normally type an expression in single quotes, such as 'X^2+1'.

## UIAlpha

Enable uppercase letters on the engine virtual keypad. You can already type letters directly on PicoCalc; use physical Shift/Caps Lock to change their case.

## UIBackspace

Delete to the left of the calculator-entry cursor.

## UIDecimal

Insert the decimal separator chosen in calculator settings at the calculator-entry cursor.

## UIDelete

Delete to the right of the calculator-entry cursor.

## UIDown

Move down in calculator entry or open its editor menu, depending on the current entry state.

## UIEnter

Submit the current calculator entry through the engine. The physical Enter key is the ordinary way to do this.

## UIExit

Invoke the calculator-entry cancel operation. Physical Esc also handles PicoCalc menu Back navigation.

## UILeft

Move left in calculator entry, or open the interactive stack when the entry state calls for it.

## UILeftShift

Select the engine's left-shift command plane for virtual-key automation. For ordinary PicoCalc soft menus, press Tab to select the row you can see.

## UILowercase

Enable lowercase letters on the engine virtual keypad. For normal PicoCalc typing, use physical Shift/Caps Lock.

## UINoAlpha

Leave the engine virtual keypad's alphabetic mode. Physical QWERTY letters remain available.

## UINoShift

Select the engine's unshifted command plane. It does not change physical Caps Lock. Tab selects the displayed normal menu row.

## UIRight

Move right in calculator entry, or begin object editing when the entry state calls for it.

## UIRightShift

Select the engine's right-shift command plane for virtual-key automation. Use Tab for everyday menu-row selection; numerical bit shifts are different commands.

## UIText

Invoke the calculator text-entry key. Normally type RPL text inside double quotes, such as "Hello".

## UIUp

Move up in calculator entry or recall entry history, depending on the current entry state.

## AlarmMenu

Open the calculator alarm soft menu. Type AlarmMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
AlarmMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## AlgebraMenu

Open the calculator algebra soft menu. Type AlgebraMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
AlgebraMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## AnglesMenu

Open the calculator angles soft menu. Type AnglesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
AnglesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ArithmeticMenu

Open the calculator arithmetic soft menu. Type ArithmeticMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
ArithmeticMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## BasesMenu

Open the calculator bases soft menu. Type BasesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
BasesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## CalculationMenu

Open the calculator calculation soft menu. Type CalculationMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
CalculationMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## CharactersMenu

Open the calculator characters soft menu. Type CharactersMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
CharactersMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## CircularMenu

Open the calculator circular soft menu. Type CircularMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
CircularMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ClearThingsMenu

Open the calculator clear things soft menu. Type ClearThingsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
ClearThingsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## CompareMenu

Open the calculator compare soft menu. Type CompareMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
CompareMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ComplexMenu

Open the calculator complex soft menu. Type ComplexMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
ComplexMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ConstantsMenu

Open the calculator constants soft menu. Type ConstantsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: ConLib, ConstantsLibrary.

Try it
ConstantsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## DateMenu

Open the calculator date soft menu. Type DateMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
DateMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## DebugMenu

Open the calculator debug soft menu. Type DebugMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
DebugMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## DifferentialSolverMenu

Open the calculator differential solver soft menu. Type DifferentialSolverMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
DifferentialSolverMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## DisplayModesMenu

Open the calculator display modes soft menu. Type DisplayModesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
DisplayModesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## EditMenu

Open the calculator edit soft menu. Type EditMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
EditMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## EquationsMenu

Open the calculator equations soft menu. Type EquationsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: EqnLib, EquationsLibrary.

Try it
EquationsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ExpLogIdentitiesMenu

Open the calculator exp log identities soft menu. Type ExpLogIdentitiesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
ExpLogIdentitiesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## FilesMenu

Open the calculator files soft menu. Type FilesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
FilesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## FlagsMenu

Open the calculator flags soft menu. Type FlagsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
FlagsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## FractionsMenu

Open the calculator fractions soft menu. Type FractionsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
FractionsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## GraphicsMenu

Open the calculator graphics soft menu. Type GraphicsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
GraphicsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## HyperbolicMenu

Open the calculator hyperbolic soft menu. Type HyperbolicMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
HyperbolicMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## IntegrationMenu

Open the calculator integration soft menu. Type IntegrationMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: DifferentiationMenu.

Try it
IntegrationMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## IOMenu

Open the calculator io soft menu. Type IOMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
IOMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## Library

Open the calculator library soft menu. Type Library and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
Library

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## LinearSolverMenu

Open the calculator linear solver soft menu. Type LinearSolverMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
LinearSolverMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ListMenu

Open the calculator list soft menu. Type ListMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
ListMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## LoopsMenu

Open the calculator loops soft menu. Type LoopsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
LoopsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## MainMenu

Open the calculator main soft menu. Type MainMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
MainMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## MathMenu

Open the calculator math soft menu. Type MathMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
MathMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## MathModesMenu

Open the calculator math modes soft menu. Type MathModesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
MathModesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## MatrixMenu

Open the calculator matrix soft menu. Type MatrixMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
MatrixMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## MemoryMenu

Open the calculator memory soft menu. Type MemoryMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
MemoryMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ModesMenu

Open the calculator modes soft menu. Type ModesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
ModesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## MultiSolverMenu

Open the calculator multi solver soft menu. Type MultiSolverMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
MultiSolverMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## NumbersMenu

Open the calculator numbers soft menu. Type NumbersMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
NumbersMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## NumericalSolverMenu

Open the calculator numerical solver soft menu. Type NumericalSolverMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
NumericalSolverMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ObjectMenu

Open the calculator object soft menu. Type ObjectMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
ObjectMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## PartsMenu

Open the calculator parts soft menu. Type PartsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
PartsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## PlotMenu

Open the calculator plot soft menu. Type PlotMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
PlotMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## PolynomialsMenu

Open the calculator polynomials soft menu. Type PolynomialsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
PolynomialsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## PolynomialSolverMenu

Open the calculator polynomial solver soft menu. Type PolynomialSolverMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
PolynomialSolverMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## PowersMenu

Open the calculator powers soft menu. Type PowersMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: ExpLogMenu.

Try it
PowersMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## PrintingMenu

Open the calculator printing soft menu. Type PrintingMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
PrintingMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ProbabilitiesMenu

Open the calculator probabilities soft menu. Type ProbabilitiesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
ProbabilitiesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ProgramMenu

Open the calculator program soft menu. Type ProgramMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
ProgramMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## RangeMenu

Open the calculator range soft menu. Type RangeMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
RangeMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## RealMenu

Open the calculator real soft menu. Type RealMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
RealMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## SeparatorModesMenu

Open the calculator separator modes soft menu. Type SeparatorModesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
SeparatorModesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## SignalProcessingMenu

Open the calculator signal processing soft menu. Type SignalProcessingMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
SignalProcessingMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## SolverMenu

Open the calculator solver soft menu. Type SolverMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: MSolvr.

Try it
SolverMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## StackMenu

Open the calculator stack soft menu. Type StackMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
StackMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## StatisticsMenu

Open the calculator statistics soft menu. Type StatisticsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
StatisticsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## SymbolicMenu

Open the calculator symbolic soft menu. Type SymbolicMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
SymbolicMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## SymbolicSolverMenu

Open the calculator symbolic solver soft menu. Type SymbolicSolverMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
SymbolicSolverMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## TestsMenu

Open the calculator tests soft menu. Type TestsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
TestsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## TextMenu

Open the calculator text soft menu. Type TextMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
TextMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## TimeMenu

Open the calculator time soft menu. Type TimeMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
TimeMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## TrigIdentitiesMenu

Open the calculator trig identities soft menu. Type TrigIdentitiesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
TrigIdentitiesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## UnitsConversionsMenu

Open the calculator units conversions soft menu. Type UnitsConversionsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
UnitsConversionsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## UnitsMenu

Open the calculator units soft menu. Type UnitsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
UnitsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## UserInterfaceModesMenu

Open the calculator user interface modes soft menu. Type UserInterfaceModesMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
UserInterfaceModesMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## UserModeMenu

Open the calculator user mode soft menu. Type UserModeMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
UserModeMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## VectorMenu

Open the calculator vector soft menu. Type VectorMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

Try it
VectorMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## AllEquationVariables

AllEquationVariables includes all equation variables in the engine solver view. CurrentEquationVariables limits it to the current equation.

Try it
AllEquationVariables

Applies the choice described above without a stack argument. Enter CurrentEquationVariables to choose its opposite. Save the session to keep this preference.

## AllSolutions

PrincipalSolution prefers a principal symbolic solution. AllSolutions asks for supported solution families. This does not make a numerical root search exhaustive.

Try it
AllSolutions

Applies the choice described above without a stack argument. Enter PrincipalSolution to choose its opposite. Save the session to keep this preference.

## AlphaForeground

The drawing pattern for alpha foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by AlphaForeground. Read the current value with 'AlphaForeground' RCL.

Try it
'AlphaForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter AlphaForeground. Save the session to keep it after restart.

## AngleMode

The trigonometric angle unit. Choose Deg, Rad, Grad or PiRadians; those names need no stack argument.

To change it, enter a value followed by AngleMode. Read the current value with 'AngleMode' RCL.

Try it
'AngleMode' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter AngleMode. Save the session to keep it after restart.

## AutomaticConstants

ExplicitConstants requires constant references to be marked explicitly. AutomaticConstants allows the engine to recognize them automatically.

Try it
AutomaticConstants

Applies the choice described above without a stack argument. Enter ExplicitConstants to choose its opposite. Save the session to keep this preference.

## AutomaticTemporariesCleanup

GCTemporariesCleanup reclaims engine temporaries during garbage collection. AutomaticTemporariesCleanup uses the automatic cleanup policy.

Try it
AutomaticTemporariesCleanup

Applies the choice described above without a stack argument. Enter GCTemporariesCleanup to choose its opposite. Save the session to keep this preference.

## AutomaticXLibs

ExplicitXLibs requires explicit library references. AutomaticXLibs allows automatic library lookup.

Try it
AutomaticXLibs

Applies the choice described above without a stack argument. Enter ExplicitXLibs to choose its opposite. Save the session to keep this preference.

## AutoScaleStack

AutoScaleStack can resize a stack object's rendering to fit. NoStackAutoScale keeps the selected scale.

Try it
AutoScaleStack

Applies the choice described above without a stack argument. Enter NoStackAutoScale to choose its opposite. Save the session to keep this preference.

## AutoSimplify

AutoSimplify simplifies expressions automatically. NoAutoSimplify lets you retain their unsimplified structure for further symbolic work.

Try it
AutoSimplify

Applies the choice described above without a stack argument. Enter NoAutoSimplify to choose its opposite. Save the session to keep this preference.

## Background

The drawing pattern for background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by Background. Read the current value with 'Background' RCL.

Try it
'Background' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter Background. Save the session to keep it after restart.

## Base

The radix used for generic based integers, from 2 to 36. Bin, Oct, Dec and Hex choose common bases without a numeric argument; on LumaRPL those commands also convert an ordinary decimal integer or a fixed-base integer at the top to the selected display base. Ctrl+Y opens Bases. The allowed values are 2 to 36.

To change it, enter a value followed by Base. Read the current value with 'Base' RCL.

Try it
16 Base 'Base' RCL

You should get: 16
Changes this calculator setting. Save the session to retain it after restart.

## BasedDotOrComma

Separate groups of based-integer digits with dots or commas.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
BasedDotOrComma

Applies the display or angle choice described above.

## BasedSeparatorCommand

The separator between groups of based-integer digits. Choose BasedSpaces, BasedDotOrComma, BasedTicks or BasedUnderscore.

To change it, enter a value followed by BasedSeparatorCommand. Read the current value with 'BasedSeparatorCommand' RCL.

Try it
'BasedSeparatorCommand' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter BasedSeparatorCommand. Save the session to keep it after restart.

## BasedSpaces

Separate groups of based-integer digits with spaces.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
BasedSpaces

Applies the display or angle choice described above.

## BasedSpacing

How based-integer digits are grouped for reading. The allowed values are 0 to 15.

To change it, enter a value followed by BasedSpacing. Read the current value with 'BasedSpacing' RCL.

Try it
'BasedSpacing' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter BasedSpacing. Save the session to keep it after restart.

## BasedTicks

Separate groups of based-integer digits with ticks.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
BasedTicks

Applies the display or angle choice described above.

## BasedUnderscore

Separate groups of based-integer digits with underscores.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
BasedUnderscore

Applies the display or angle choice described above.

## BatteryBackground

The drawing pattern for battery background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by BatteryBackground. Read the current value with 'BatteryBackground' RCL.

Try it
'BatteryBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter BatteryBackground. Save the session to keep it after restart.

## BatteryForeground

The drawing pattern for battery foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by BatteryForeground. Read the current value with 'BatteryForeground' RCL.

Try it
'BatteryForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter BatteryForeground. Save the session to keep it after restart.

## BatteryLevelForeground

The drawing pattern for battery level foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by BatteryLevelForeground. Read the current value with 'BatteryLevelForeground' RCL.

Try it
'BatteryLevelForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter BatteryLevelForeground. Save the session to keep it after restart.

## BatteryRefresh

The engine battery polling interval in milliseconds.

To change it, enter a value followed by BatteryRefresh. Read the current value with 'BatteryRefresh' RCL.

Try it
'BatteryRefresh' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter BatteryRefresh. Save the session to keep it after restart.

## BeepOff

BeepOn allows calculator sounds. BeepOff makes them quiet. The Beep soft key toggles this choice without needing a stack value.

Try it
BeepOff

Applies the choice described above without a stack argument. Enter BeepOn to choose its opposite. Save the session to keep this preference.

## BeepOn

BeepOn allows calculator sounds. BeepOff makes them quiet. The Beep soft key toggles this choice without needing a stack value.

Try it
BeepOn

Applies the choice described above without a stack argument. Enter BeepOff to choose its opposite. Save the session to keep this preference.

## BigFractions

BigFractions gives fractions larger typography. SmallFractions uses the compact fraction style.

Try it
BigFractions

Applies the choice described above without a stack argument. Enter SmallFractions to choose its opposite. Save the session to keep this preference.

## Bin

Show based integers in base 2. On LumaRPL this also makes a fixed-base integer at the top follow the selected display base, without changing its value. Fixed-base values below the top retain their representation. Ordinary decimal integers at the top convert directly too; decimal fractions remain unchanged. Negative integers use the current WordSize, as with RealToBinary. Ctrl+Y opens Bases from the calculator.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Binary.

Try it
65490 Bin

You should get: #1111 1111 1101 0010₂
The value is 65490 in decimal. Decimal integers and explicit binary, octal or hexadecimal integers convert directly. Decimal fractions are not converted.

## BitwiseLogicForIntegers

TruthLogicForIntegers treats integers as truth values in logical operations. BitwiseLogicForIntegers operates on their bits.

Try it
BitwiseLogicForIntegers

Applies the choice described above without a stack argument. Enter TruthLogicForIntegers to choose its opposite. Save the session to keep this preference.

## BusyIndicatorRefresh

The engine busy-indicator refresh interval in milliseconds.

To change it, enter a value followed by BusyIndicatorRefresh. Read the current value with 'BusyIndicatorRefresh' RCL.

Try it
'BusyIndicatorRefresh' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter BusyIndicatorRefresh. Save the session to keep it after restart.

## Capitalized

Choose capitalized rendering for calculator command names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: CapitalizedCommands.

Try it
Capitalized

Applies the display or angle choice described above.

## CapitalizedNames

Choose capitalized rendering for symbolic names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: CapitalizedSymbols.

Try it
CapitalizedNames

Applies the display or angle choice described above.

## CarefulEvaluation

CarefulEvaluation uses the engine's careful evaluation policy. LazyEvaluation selects its lazy policy; use this only when your RPL program relies on that distinction.

Try it
CarefulEvaluation

Applies the choice described above without a stack argument. Enter LazyEvaluation to choose its opposite. Save the session to keep this preference.

## ChargingForeground

The drawing pattern for charging foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by ChargingForeground. Read the current value with 'ChargingForeground' RCL.

Try it
'ChargingForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ChargingForeground. Save the session to keep it after restart.

## ClassicExponent

ClassicExponent uses conventional source-style exponents. FancyExponent uses the engine's typeset exponent form.

Try it
ClassicExponent

Applies the choice described above without a stack argument. Enter FancyExponent to choose its opposite. Save the session to keep this preference.

## CommandBackground

The drawing pattern for command background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by CommandBackground. Read the current value with 'CommandBackground' RCL.

Try it
'CommandBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CommandBackground. Save the session to keep it after restart.

## CommandDisplayMode

How calculator command names are capitalized or expanded. Use the named command-display choices rather than raw IDs.

To change it, enter a value followed by CommandDisplayMode. Read the current value with 'CommandDisplayMode' RCL.

Try it
'CommandDisplayMode' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CommandDisplayMode. Save the session to keep it after restart.

## CommandForeground

The drawing pattern for command foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by CommandForeground. Read the current value with 'CommandForeground' RCL.

Try it
'CommandForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CommandForeground. Save the session to keep it after restart.

## CompatibleBasedNumbers

CompatibleBasedNumbers uses older based-number notation. ModernBasedNumbers uses the engine's modern notation; the integer value is unchanged.

Try it
CompatibleBasedNumbers

Applies the choice described above without a stack argument. Enter ModernBasedNumbers to choose its opposite. Save the session to keep this preference.

## CompatibleGROBs

CompatibleGROBs uses the compatibility graphics-object representation. PackedBitmaps uses packed bitmaps.

Try it
CompatibleGROBs

Applies the choice described above without a stack argument. Enter PackedBitmaps to choose its opposite. Save the session to keep this preference.

## CompatiblePolynomials

CompatiblePolynomials selects older polynomial conventions. NewStylePolynomials selects DB48X conventions; choose the one expected by your RPL code.

Try it
CompatiblePolynomials

Applies the choice described above without a stack argument. Enter NewStylePolynomials to choose its opposite. Save the session to keep this preference.

## CompatibleTypes

DetailedTypes reports the engine's detailed object types. CompatibleTypes uses compatibility type numbering.

Try it
CompatibleTypes

Applies the choice described above without a stack argument. Enter DetailedTypes to choose its opposite. Save the session to keep this preference.

## ComplexIAfterImaginary

ComplexIBeforeImaginary puts i before its coefficient. ComplexIAfterImaginary puts i after it, as in 2i.

Try it
ComplexIAfterImaginary

Applies the choice described above without a stack argument. Enter ComplexIBeforeImaginary to choose its opposite. Save the session to keep this preference.

## ComplexIBeforeImaginary

ComplexIBeforeImaginary puts i before its coefficient. ComplexIAfterImaginary puts i after it, as in 2i.

Try it
ComplexIBeforeImaginary

Applies the choice described above without a stack argument. Enter ComplexIAfterImaginary to choose its opposite. Save the session to keep this preference.

## ComplexResults

ComplexResults allows operations to return complex values. RealResults keeps real-only behavior where the operation supports that choice.

Try it
ComplexResults

Applies the choice described above without a stack argument. Enter RealResults to choose its opposite. Save the session to keep this preference.

## CurrentEquationVariables

AllEquationVariables includes all equation variables in the engine solver view. CurrentEquationVariables limits it to the current equation.

Try it
CurrentEquationVariables

Applies the choice described above without a stack argument. Enter AllEquationVariables to choose its opposite. Save the session to keep this preference.

## CurrentFitSums

LinearFitSums uses statistical sums for a linear fit. CurrentFitSums uses sums for the currently selected regression model.

Try it
CurrentFitSums

Applies the choice described above without a stack argument. Enter LinearFitSums to choose its opposite. Save the session to keep this preference.

## CursorAlphaBackground

The drawing pattern for cursor alpha background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by CursorAlphaBackground. Read the current value with 'CursorAlphaBackground' RCL.

Try it
'CursorAlphaBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CursorAlphaBackground. Save the session to keep it after restart.

## CursorAlphaBorder

The drawing pattern for cursor alpha border. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by CursorAlphaBorder. Read the current value with 'CursorAlphaBorder' RCL.

Try it
'CursorAlphaBorder' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CursorAlphaBorder. Save the session to keep it after restart.

## CursorAlphaForeground

The drawing pattern for cursor alpha foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by CursorAlphaForeground. Read the current value with 'CursorAlphaForeground' RCL.

Try it
'CursorAlphaForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CursorAlphaForeground. Save the session to keep it after restart.

## CursorBackground

The drawing pattern for cursor background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by CursorBackground. Read the current value with 'CursorBackground' RCL.

Try it
'CursorBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CursorBackground. Save the session to keep it after restart.

## CursorBlinkRate

The calculator-entry cursor blink interval in milliseconds. The allowed values are 10 to 10000.

To change it, enter a value followed by CursorBlinkRate. Read the current value with 'CursorBlinkRate' RCL.

Try it
'CursorBlinkRate' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CursorBlinkRate. Save the session to keep it after restart.

## CursorBorder

The drawing pattern for cursor border. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by CursorBorder. Read the current value with 'CursorBorder' RCL.

Try it
'CursorBorder' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CursorBorder. Save the session to keep it after restart.

## CursorFont

The built-in font used for the single-line calculator cursor. Font IDs 0-12 are choices, not pixel heights. This does not resize native Help, menus or source editing.

To change it, enter a value followed by CursorFont. Read the current value with 'CursorFont' RCL.

Try it
'CursorFont' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CursorFont. Save the session to keep it after restart.

## CursorForeground

The drawing pattern for cursor foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by CursorForeground. Read the current value with 'CursorForeground' RCL.

Try it
'CursorForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CursorForeground. Save the session to keep it after restart.

## CursorSelBackground

The drawing pattern for cursor sel background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by CursorSelBackground. Read the current value with 'CursorSelBackground' RCL.

Try it
'CursorSelBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CursorSelBackground. Save the session to keep it after restart.

## CurveFilling

CurveFilling fills engine plotted curves where supported. NoCurveFilling leaves them unfilled.

Try it
CurveFilling

Applies the choice described above without a stack argument. Enter NoCurveFilling to choose its opposite. Save the session to keep this preference.

## CustomHeaderRefresh

The engine custom-header refresh interval in milliseconds.

To change it, enter a value followed by CustomHeaderRefresh. Read the current value with 'CustomHeaderRefresh' RCL.

Try it
'CustomHeaderRefresh' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter CustomHeaderRefresh. Save the session to keep it after restart.

## DateDash

Use dashes between the parts of displayed dates.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
DateDash

Applies the display or angle choice described above.

## DateDot

Use dots between the parts of displayed dates.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
DateDot

Applies the display or angle choice described above.

## DateForeground

The drawing pattern for date foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by DateForeground. Read the current value with 'DateForeground' RCL.

Try it
'DateForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter DateForeground. Save the session to keep it after restart.

## DateSeparatorCommand

The separator in displayed dates. Choose DateSlash, DateDash, DateDot or DateSpace.

To change it, enter a value followed by DateSeparatorCommand. Read the current value with 'DateSeparatorCommand' RCL.

Try it
'DateSeparatorCommand' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter DateSeparatorCommand. Save the session to keep it after restart.

## DateSlash

Use slashes between the parts of displayed dates.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
DateSlash

Applies the display or angle choice described above.

## DateSpace

Use spaces between the parts of displayed dates.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
DateSpace

Applies the display or angle choice described above.

## DayBeforeMonth

MonthBeforeDay places month before day. DayBeforeMonth places day before month.

Try it
DayBeforeMonth

Applies the choice described above without a stack argument. Enter MonthBeforeDay to choose its opposite. Save the session to keep this preference.

## DebugOnError

DebugOnError halts an RPL program for debugging when it errors. KillOnError terminates the program instead.

Try it
DebugOnError

Applies the choice described above without a stack argument. Enter KillOnError to choose its opposite. Save the session to keep this preference.

## Dec

Show based integers in base 10. On LumaRPL this also makes a fixed-base integer at the top follow the selected display base, without changing its value. Fixed-base values below the top retain their representation. Ordinary decimal integers at the top convert directly too; decimal fractions remain unchanged. Negative integers use the current WordSize, as with RealToBinary. Ctrl+Y opens Bases from the calculator.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Decimal.

Try it
#177722o Dec

You should get: #6 5490₁₀
The value is 65490 in decimal. Decimal integers and explicit binary, octal or hexadecimal integers convert directly. Decimal fractions are not converted.

## DecimalComma

DecimalComma makes comma the decimal separator. DecimalDot uses a dot. Enter numbers using the selected convention.

Try it
DecimalComma

Applies the choice described above without a stack argument. Enter DecimalDot to choose its opposite. Save the session to keep this preference.

## DecimalDot

DecimalComma makes comma the decimal separator. DecimalDot uses a dot. Enter numbers using the selected convention.

Try it
DecimalDot

Applies the choice described above without a stack argument. Enter DecimalComma to choose its opposite. Save the session to keep this preference.

## Deg

Use degrees for angles: a full turn is 360.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Degrees.

Try it
Deg

Applies the display or angle choice described above.

## DetailedTypes

DetailedTypes reports the engine's detailed object types. CompatibleTypes uses compatibility type numbering.

Try it
DetailedTypes

Applies the choice described above without a stack argument. Enter CompatibleTypes to choose its opposite. Save the session to keep this preference.

## DisplayDigits

The digit count used by the current display style. Fix counts decimal places; the other styles use their documented digit convention.

To change it, enter a value followed by DisplayDigits. Read the current value with 'DisplayDigits' RCL.

Try it
3 DisplayDigits 'DisplayDigits' RCL

You should get: 3
Changes this calculator setting. Save the session to retain it after restart.

## DisplayMode

The number display style. Choose Std, Fix, Sci, Eng or Sig instead of entering a raw mode ID.

To change it, enter a value followed by DisplayMode. Read the current value with 'DisplayMode' RCL.

Try it
'DisplayMode' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter DisplayMode. Save the session to keep it after restart.

## DistinguishSymbolCase

DistinguishSymbolCase treats letter case as significant in the relevant symbol comparisons. IgnoreSymbolCase ignores that difference.

Try it
DistinguishSymbolCase

Applies the choice described above without a stack argument. Enter IgnoreSymbolCase to choose its opposite. Save the session to keep this preference.

## DMCPDisplayRefresh

SoftwareDisplayRefresh and DMCPDisplayRefresh are retained engine compatibility choices. PicoCalc uses its own dirty-row LCD driver in either case.

Try it
DMCPDisplayRefresh

Applies the choice described above without a stack argument. Enter SoftwareDisplayRefresh to choose its opposite. Save the session to keep this preference.

## DrawPlotAxes

DrawPlotAxes includes axes in engine RPL plots. NoPlotAxes omits them. The native graph tools draw their own axes.

Try it
DrawPlotAxes

Applies the choice described above without a stack argument. Enter NoPlotAxes to choose its opposite. Save the session to keep this preference.

## EchelonFormKeepLastColumn

EchelonFormKeepLastColumn keeps the last column separate during echelon processing. EchelonFormReduceLastColumn reduces it with the others.

Try it
EchelonFormKeepLastColumn

Applies the choice described above without a stack argument. Enter EchelonFormReduceLastColumn to choose its opposite. Save the session to keep this preference.

## EchelonFormReduceLastColumn

EchelonFormKeepLastColumn keeps the last column separate during echelon processing. EchelonFormReduceLastColumn reduces it with the others.

Try it
EchelonFormReduceLastColumn

Applies the choice described above without a stack argument. Enter EchelonFormKeepLastColumn to choose its opposite. Save the session to keep this preference.

## EditorBackground

The drawing pattern for editor background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by EditorBackground. Read the current value with 'EditorBackground' RCL.

Try it
'EditorBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter EditorBackground. Save the session to keep it after restart.

## EditorFont

The built-in font used for single-line calculator entry. Font IDs 0-12 are choices, not pixel heights. This does not resize native Help, menus or source editing.

To change it, enter a value followed by EditorFont. Read the current value with 'EditorFont' RCL.

Try it
'EditorFont' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter EditorFont. Save the session to keep it after restart.

## EditorForeground

The drawing pattern for editor foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by EditorForeground. Read the current value with 'EditorForeground' RCL.

Try it
'EditorForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter EditorForeground. Save the session to keep it after restart.

## EditorLineForeground

The drawing pattern for editor line foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by EditorLineForeground. Read the current value with 'EditorLineForeground' RCL.

Try it
'EditorLineForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter EditorLineForeground. Save the session to keep it after restart.

## EditorWrapColumn

Where calculator program entry wraps. The native SD source editor has its own layout.

To change it, enter a value followed by EditorWrapColumn. Read the current value with 'EditorWrapColumn' RCL.

Try it
'EditorWrapColumn' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter EditorWrapColumn. Save the session to keep it after restart.

## Eng

Show numbers in engineering notation, with exponents in multiples of three and the supplied digit count.

Put the digit count on the stack first. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: EngineeringDisplay.

Try it
3 Eng

Shows three digits using this display style.

## ErrorBackground

The drawing pattern for error background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by ErrorBackground. Read the current value with 'ErrorBackground' RCL.

Try it
'ErrorBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ErrorBackground. Save the session to keep it after restart.

## ErrorBeepDuration

How long an error tone lasts, in milliseconds. The allowed values are 0 to 500.

To change it, enter a value followed by ErrorBeepDuration. Read the current value with 'ErrorBeepDuration' RCL.

Try it
'ErrorBeepDuration' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ErrorBeepDuration. Save the session to keep it after restart.

## ErrorBeepFrequency

The error tone's frequency in Hz. BeepOn/BeepOff separately controls whether calculator sounds are enabled. The allowed values are 0 to 25000.

To change it, enter a value followed by ErrorBeepFrequency. Read the current value with 'ErrorBeepFrequency' RCL.

Try it
'ErrorBeepFrequency' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ErrorBeepFrequency. Save the session to keep it after restart.

## ErrorBorder

The drawing pattern for error border. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by ErrorBorder. Read the current value with 'ErrorBorder' RCL.

Try it
'ErrorBorder' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ErrorBorder. Save the session to keep it after restart.

## ErrorForeground

The drawing pattern for error foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by ErrorForeground. Read the current value with 'ErrorForeground' RCL.

Try it
'ErrorForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ErrorForeground. Save the session to keep it after restart.

## ExitClearsMenu

ExitKeepsMenu and ExitClearsMenu are retained compatibility settings. On PicoCalc, Esc goes back one calculator menu level and clears the menu at the root in either case.

Try it
ExitClearsMenu

Applies the choice described above without a stack argument. Enter ExitKeepsMenu to choose its opposite. Save the session to keep this preference.

## ExitKeepsMenu

ExitKeepsMenu and ExitClearsMenu are retained compatibility settings. On PicoCalc, Esc goes back one calculator menu level and clears the menu at the root in either case.

Try it
ExitKeepsMenu

Applies the choice described above without a stack argument. Enter ExitClearsMenu to choose its opposite. Save the session to keep this preference.

## ExplicitConstants

ExplicitConstants requires constant references to be marked explicitly. AutomaticConstants allows the engine to recognize them automatically.

Try it
ExplicitConstants

Applies the choice described above without a stack argument. Enter AutomaticConstants to choose its opposite. Save the session to keep this preference.

## ExplicitWildcards

ExplicitWildcards requires rewrite wildcards to be marked explicitly. ImplicitWildcards allows the engine's implicit wildcard interpretation.

Try it
ExplicitWildcards

Applies the choice described above without a stack argument. Enter ImplicitWildcards to choose its opposite. Save the session to keep this preference.

## ExplicitXLibs

ExplicitXLibs requires explicit library references. AutomaticXLibs allows automatic library lookup.

Try it
ExplicitXLibs

Applies the choice described above without a stack argument. Enter AutomaticXLibs to choose its opposite. Save the session to keep this preference.

## FancyExponent

ClassicExponent uses conventional source-style exponents. FancyExponent uses the engine's typeset exponent form.

Try it
FancyExponent

Applies the choice described above without a stack argument. Enter ClassicExponent to choose its opposite. Save the session to keep this preference.

## FinalAlgebraResults

StepByStepAlgebraResults retains intermediate algebra results where supported. FinalAlgebraResults keeps the final result.

Try it
FinalAlgebraResults

Applies the choice described above without a stack argument. Enter StepByStepAlgebraResults to choose its opposite. Save the session to keep this preference.

## FinanceRounding

The rounding-digit setting for engine finance calculations. The native Financial calculator uses the conventions in its own Help.

To change it, enter a value followed by FinanceRounding. Read the current value with 'FinanceRounding' RCL.

Try it
'FinanceRounding' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter FinanceRounding. Save the session to keep it after restart.

## Fix

Choose how many digits to show after the decimal point. For example, 3 Fix displays 1.25 as 1.250.

Put the digit count on the stack first. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: FixedDisplay.

Try it
3 Fix

Shows three digits using this display style.

## FixedWidthDigits

FixedWidthDigits gives mathematical digits equal widths. VariableWidthDigits uses proportional digit widths. Native Help keeps its own font.

Try it
FixedWidthDigits

Applies the choice described above without a stack argument. Enter VariableWidthDigits to choose its opposite. Save the session to keep this preference.

## FlatMenus

Use the engine flat-menu layout.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
FlatMenus

Applies the display or angle choice described above.

## FolderCornerForeground

The drawing pattern for folder corner foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by FolderCornerForeground. Read the current value with 'FolderCornerForeground' RCL.

Try it
'FolderCornerForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter FolderCornerForeground. Save the session to keep it after restart.

## Foreground

The drawing pattern for foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by Foreground. Read the current value with 'Foreground' RCL.

Try it
'Foreground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter Foreground. Save the session to keep it after restart.

## FourDigitYear

TwoDigitYear shortens the displayed year to two digits. FourDigitYear shows all four digits.

Try it
FourDigitYear

Applies the choice described above without a stack argument. Enter TwoDigitYear to choose its opposite. Save the session to keep this preference.

## FractionDigits

The digit limit used when converting a value to a fraction, rather than a display-decimal setting.

To change it, enter a value followed by FractionDigits. Read the current value with 'FractionDigits' RCL.

You can also enter: →QDigits.

Try it
'FractionDigits' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter FractionDigits. Save the session to keep it after restart.

## FractionIterations

How many iterations fraction conversion may try.

To change it, enter a value followed by FractionIterations. Read the current value with 'FractionIterations' RCL.

You can also enter: →QIterations.

Try it
'FractionIterations' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter FractionIterations. Save the session to keep it after restart.

## FractionLargestPrime

The prime-search limit used during fraction conversion.

To change it, enter a value followed by FractionLargestPrime. Read the current value with 'FractionLargestPrime' RCL.

You can also enter: →QπMaxPrime.

Try it
'FractionLargestPrime' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter FractionLargestPrime. Save the session to keep it after restart.

## FractionSpacing

How fraction digits are grouped for reading. The allowed values are 0 to 15.

To change it, enter a value followed by FractionSpacing. Read the current value with 'FractionSpacing' RCL.

Try it
'FractionSpacing' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter FractionSpacing. Save the session to keep it after restart.

## GCIconForeground

The drawing pattern for gcicon foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by GCIconForeground. Read the current value with 'GCIconForeground' RCL.

Try it
'GCIconForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter GCIconForeground. Save the session to keep it after restart.

## GCStatsClearAfterRead

GCStatsClearAfterRead resets garbage-collection statistics when read. GCStatsKeepAfterRead leaves them accumulated.

Try it
GCStatsClearAfterRead

Applies the choice described above without a stack argument. Enter GCStatsKeepAfterRead to choose its opposite. Save the session to keep this preference.

## GCStatsKeepAfterRead

GCStatsClearAfterRead resets garbage-collection statistics when read. GCStatsKeepAfterRead leaves them accumulated.

Try it
GCStatsKeepAfterRead

Applies the choice described above without a stack argument. Enter GCStatsClearAfterRead to choose its opposite. Save the session to keep this preference.

## GCTemporariesCleanup

GCTemporariesCleanup reclaims engine temporaries during garbage collection. AutomaticTemporariesCleanup uses the automatic cleanup policy.

Try it
GCTemporariesCleanup

Applies the choice described above without a stack argument. Enter AutomaticTemporariesCleanup to choose its opposite. Save the session to keep this preference.

## Grad

Use grads for angles: a full turn is 400.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Grads.

Try it
Grad

Applies the display or angle choice described above.

## GraphicResultDisplay

TextResultDisplay shows calculator answers as source text. GraphicResultDisplay uses mathematical layout.

Try it
GraphicResultDisplay

Applies the choice described above without a stack argument. Enter TextResultDisplay to choose its opposite. Save the session to keep this preference.

## GraphicStackDisplay

TextStackDisplay shows stack objects as source text. GraphicStackDisplay typesets them as mathematics.

Try it
GraphicStackDisplay

Applies the choice described above without a stack argument. Enter TextStackDisplay to choose its opposite. Save the session to keep this preference.

## GraphingTimeLimit

The engine object-graphing time limit in milliseconds. Native plot sampling uses its own work budgets.

To change it, enter a value followed by GraphingTimeLimit. Read the current value with 'GraphingTimeLimit' RCL.

Try it
'GraphingTimeLimit' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter GraphingTimeLimit. Save the session to keep it after restart.

## GraphRenderingSizeLimit

The engine graphical-rendering size limit for calculator objects.

To change it, enter a value followed by GraphRenderingSizeLimit. Read the current value with 'GraphRenderingSizeLimit' RCL.

Try it
'GraphRenderingSizeLimit' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter GraphRenderingSizeLimit. Save the session to keep it after restart.

## HalfBatteryForeground

The drawing pattern for half battery foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by HalfBatteryForeground. Read the current value with 'HalfBatteryForeground' RCL.

Try it
'HalfBatteryForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter HalfBatteryForeground. Save the session to keep it after restart.

## HaltedIconForeground

The drawing pattern for halted icon foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by HaltedIconForeground. Read the current value with 'HaltedIconForeground' RCL.

Try it
'HaltedIconForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter HaltedIconForeground. Save the session to keep it after restart.

## HardwareFloatingPoint

HardwareFloatingPoint permits hardware floating-point arithmetic where supported. SoftwareFloatingPoint selects software arithmetic.

You can also enter: HFP, HardFP.

Try it
HardwareFloatingPoint

Applies the choice described above without a stack argument. Enter SoftwareFloatingPoint to choose its opposite. Save the session to keep this preference.

## HeaderBackground

The drawing pattern for header background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by HeaderBackground. Read the current value with 'HeaderBackground' RCL.

Try it
'HeaderBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter HeaderBackground. Save the session to keep it after restart.

## HeaderFont

The built-in font used for the saved engine header. Font IDs 0-12 are choices, not pixel heights. The PicoCalc header always uses its compact face, so this saved value does not resize it.

To change it, enter a value followed by HeaderFont. Read the current value with 'HeaderFont' RCL.

Try it
'HeaderFont' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter HeaderFont. Save the session to keep it after restart.

## HeaderForeground

The drawing pattern for header foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by HeaderForeground. Read the current value with 'HeaderForeground' RCL.

Try it
'HeaderForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter HeaderForeground. Save the session to keep it after restart.

## Hex

Show based integers in base 16. On LumaRPL this also makes a fixed-base integer at the top follow the selected display base, without changing its value. Fixed-base values below the top retain their representation. Ordinary decimal integers at the top convert directly too; decimal fractions remain unchanged. Negative integers use the current WordSize, as with RealToBinary. Ctrl+Y opens Bases from the calculator.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Hexadecimal.

Try it
#1111111111010010b Hex

You should get: #FFD2₁₆
The value is 65490 in decimal. Decimal integers and explicit binary, octal or hexadecimal integers convert directly. Decimal fractions are not converted.

## HideAmbiguousNames

HideAmbiguousNames leaves ambiguous names out of relevant engine listings. ShowAmbiguousNames includes them.

Try it
HideAmbiguousNames

Applies the choice described above without a stack argument. Enter ShowAmbiguousNames to choose its opposite. Save the session to keep this preference.

## HideBuiltinCharacters

ShowBuiltinCharacters includes built-in character-library entries. HideBuiltinCharacters hides those entries.

Try it
HideBuiltinCharacters

Applies the choice described above without a stack argument. Enter ShowBuiltinCharacters to choose its opposite. Save the session to keep this preference.

## HideBuiltinConstants

ShowBuiltinConstants includes built-in constants in the library. HideBuiltinConstants hides those entries.

Try it
HideBuiltinConstants

Applies the choice described above without a stack argument. Enter ShowBuiltinConstants to choose its opposite. Save the session to keep this preference.

## HideBuiltinEquations

ShowBuiltinEquations includes built-in equation-library entries. HideBuiltinEquations hides them from that listing.

Try it
HideBuiltinEquations

Applies the choice described above without a stack argument. Enter ShowBuiltinEquations to choose its opposite. Save the session to keep this preference.

## HideBuiltinLibrary

ShowBuiltinLibrary includes built-in program-library entries. HideBuiltinLibrary hides them from that listing.

Try it
HideBuiltinLibrary

Applies the choice described above without a stack argument. Enter ShowBuiltinLibrary to choose its opposite. Save the session to keep this preference.

## HideBuiltinUnits

ShowBuiltinUnits includes built-in units in the unit library. HideBuiltinUnits hides those entries from that listing.

Try it
HideBuiltinUnits

Applies the choice described above without a stack argument. Enter ShowBuiltinUnits to choose its opposite. Save the session to keep this preference.

## HideDate

ShowDate includes the date in the calculator header. HideDate leaves it out.

Try it
HideDate

Applies the choice described above without a stack argument. Enter ShowDate to choose its opposite. Save the session to keep this preference.

## HideDayOfWeek

ShowDayOfWeek includes the weekday in the header date. HideDayOfWeek omits it.

Try it
HideDayOfWeek

Applies the choice described above without a stack argument. Enter ShowDayOfWeek to choose its opposite. Save the session to keep this preference.

## HideEmptyMenu

ShowEmptyMenu leaves an empty calculator menu visible. HideEmptyMenu hides it.

Try it
HideEmptyMenu

Applies the choice described above without a stack argument. Enter ShowEmptyMenu to choose its opposite. Save the session to keep this preference.

## HideEquationBody

ShowEquationBody displays the contents of an equation. HideEquationBody hides the body in the relevant equation display.

Try it
HideEquationBody

Applies the choice described above without a stack argument. Enter ShowEquationBody to choose its opposite. Save the session to keep this preference.

## HideMonthName

ShowMonthName uses a month name in dates. HideMonthName uses a numeric month.

Try it
HideMonthName

Applies the choice described above without a stack argument. Enter ShowMonthName to choose its opposite. Save the session to keep this preference.

## HideSeconds

ShowSeconds includes seconds in the header clock. HideSeconds shows only hours and minutes.

Try it
HideSeconds

Applies the choice described above without a stack argument. Enter ShowSeconds to choose its opposite. Save the session to keep this preference.

## HideTime

ShowTime includes the clock in the calculator header. HideTime leaves it out.

Try it
HideTime

Applies the choice described above without a stack argument. Enter ShowTime to choose its opposite. Save the session to keep this preference.

## HideVoltage

HideVoltage hides the legacy voltage display. ShowVoltage requests it, but PicoCalc cannot supply a measured voltage here. Use Device status > Battery for percentage and charging state.

Try it
HideVoltage

Applies the choice described above without a stack argument. Enter ShowVoltage to choose its opposite. Save the session to keep this preference.

## HorizontalLists

VerticalLists typesets lists down the page. HorizontalLists typesets them across it.

Try it
HorizontalLists

Applies the choice described above without a stack argument. Enter VerticalLists to choose its opposite. Save the session to keep this preference.

## HorizontalProgramRendering

VerticalProgramRendering lays programs out down the page. HorizontalProgramRendering uses horizontal source-style layout.

Try it
HorizontalProgramRendering

Applies the choice described above without a stack argument. Enter VerticalProgramRendering to choose its opposite. Save the session to keep this preference.

## HorizontalVectors

HorizontalVectors displays vectors across a row. VerticalVectors displays them down a column.

Try it
HorizontalVectors

Applies the choice described above without a stack argument. Enter VerticalVectors to choose its opposite. Save the session to keep this preference.

## IgnorePrecisionLoss

ReportPrecisionLoss raises an error when precision is lost. IgnorePrecisionLoss lets the calculation continue without that error.

Try it
IgnorePrecisionLoss

Applies the choice described above without a stack argument. Enter ReportPrecisionLoss to choose its opposite. Save the session to keep this preference.

## IgnoreSymbolCase

DistinguishSymbolCase treats letter case as significant in the relevant symbol comparisons. IgnoreSymbolCase ignores that difference.

Try it
IgnoreSymbolCase

Applies the choice described above without a stack argument. Enter DistinguishSymbolCase to choose its opposite. Save the session to keep this preference.

## ImplicitWildcards

ExplicitWildcards requires rewrite wildcards to be marked explicitly. ImplicitWildcards allows the engine's implicit wildcard interpretation.

Try it
ImplicitWildcards

Applies the choice described above without a stack argument. Enter ExplicitWildcards to choose its opposite. Save the session to keep this preference.

## ImproperFractions

ImproperFractions displays the whole numerator over the denominator. MixedFractions separates the integer part from the fractional remainder.

Try it
ImproperFractions

Applies the choice described above without a stack argument. Enter MixedFractions to choose its opposite. Save the session to keep this preference.

## IndexDidNotWrap

IndexWrapped records that an indexed operation wrapped. IndexDidNotWrap clears that state; these are engine indexing flags rather than display choices.

Try it
IndexDidNotWrap

Applies the choice described above without a stack argument. Enter IndexWrapped to choose its opposite. Save the session to keep this preference.

## IndexWrapped

IndexWrapped records that an indexed operation wrapped. IndexDidNotWrap clears that state; these are engine indexing flags rather than display choices.

Try it
IndexWrapped

Applies the choice described above without a stack argument. Enter IndexDidNotWrap to choose its opposite. Save the session to keep this preference.

## InfiniteResultIndicator

InfiniteResultIndicator shows the infinite-result indication. NoInfiniteResult hides it; the separate infinity policy controls whether the result is allowed.

Try it
InfiniteResultIndicator

Applies the choice described above without a stack argument. Enter NoInfiniteResult to choose its opposite. Save the session to keep this preference.

## InfinityError

InfinityValue permits an infinite answer. InfinityError reports it as an error instead.

Try it
InfinityError

Applies the choice described above without a stack argument. Enter InfinityValue to choose its opposite. Save the session to keep this preference.

## InfinityValue

InfinityValue permits an infinite answer. InfinityError reports it as an error instead.

Try it
InfinityValue

Applies the choice described above without a stack argument. Enter InfinityError to choose its opposite. Save the session to keep this preference.

## IntegrationImprecision

The precision allowance used by engine numerical integration.

To change it, enter a value followed by IntegrationImprecision. Read the current value with 'IntegrationImprecision' RCL.

Try it
'IntegrationImprecision' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter IntegrationImprecision. Save the session to keep it after restart.

## IntegrationIterations

How many numerical integration refinements the engine may attempt.

To change it, enter a value followed by IntegrationIterations. Read the current value with 'IntegrationIterations' RCL.

Try it
'IntegrationIterations' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter IntegrationIterations. Save the session to keep it after restart.

## KillOnError

DebugOnError halts an RPL program for debugging when it errors. KillOnError terminates the program instead.

Try it
KillOnError

Applies the choice described above without a stack argument. Enter DebugOnError to choose its opposite. Save the session to keep this preference.

## LaxArrayResizing

LaxArrayResizing permits the engine's relaxed array resizing. StrictArrayResizing requires strict resizing rules.

Try it
LaxArrayResizing

Applies the choice described above without a stack argument. Enter StrictArrayResizing to choose its opposite. Save the session to keep this preference.

## LazyEvaluation

CarefulEvaluation uses the engine's careful evaluation policy. LazyEvaluation selects its lazy policy; use this only when your RPL program relies on that distinction.

Try it
LazyEvaluation

Applies the choice described above without a stack argument. Enter CarefulEvaluation to choose its opposite. Save the session to keep this preference.

## LeadingZero

LeadingZero displays the zero before a fractional decimal. NoLeadingZero omits it, giving forms such as .5.

Try it
LeadingZero

Applies the choice described above without a stack argument. Enter NoLeadingZero to choose its opposite. Save the session to keep this preference.

## LeftShiftBackground

The drawing pattern for left shift background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by LeftShiftBackground. Read the current value with 'LeftShiftBackground' RCL.

Try it
'LeftShiftBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter LeftShiftBackground. Save the session to keep it after restart.

## LeftShiftForeground

The drawing pattern for left shift foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by LeftShiftForeground. Read the current value with 'LeftShiftForeground' RCL.

Try it
'LeftShiftForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter LeftShiftForeground. Save the session to keep it after restart.

## LinearFitSums

LinearFitSums uses statistical sums for a linear fit. CurrentFitSums uses sums for the currently selected regression model.

Try it
LinearFitSums

Applies the choice described above without a stack argument. Enter CurrentFitSums to choose its opposite. Save the session to keep this preference.

## LineWidth

The thickness, in pixels, for engine RPL drawing lines. Native interactive plots have separate controls. The allowed values are 0 to 255.

To change it, enter a value followed by LineWidth. Read the current value with 'LineWidth' RCL.

Try it
2 LineWidth 'LineWidth' RCL

You should get: 2
Changes this calculator setting. Save the session to retain it after restart.

## ListAsData

ListAsProgram treats lists as programs in the applicable evaluation context. ListAsData keeps their data interpretation.

Try it
ListAsData

Applies the choice described above without a stack argument. Enter ListAsProgram to choose its opposite. Save the session to keep this preference.

## ListAsProgram

ListAsProgram treats lists as programs in the applicable evaluation context. ListAsData keeps their data interpretation.

Try it
ListAsProgram

Applies the choice described above without a stack argument. Enter ListAsData to choose its opposite. Save the session to keep this preference.

## ListRecursionDepth

How deeply list operations may process nested lists. Lowering the limit can stop unexpectedly deep input sooner.

To change it, enter a value followed by ListRecursionDepth. Read the current value with 'ListRecursionDepth' RCL.

Try it
'ListRecursionDepth' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ListRecursionDepth. Save the session to keep it after restart.

## LongForm

Choose long form rendering for calculator command names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: LongFormCommands.

Try it
LongForm

Applies the display or angle choice described above.

## LongFormNames

Choose long form rendering for symbolic names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: LongFormSymbols.

Try it
LongFormNames

Applies the display or angle choice described above.

## LowBatteryForeground

The drawing pattern for low battery foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by LowBatteryForeground. Read the current value with 'LowBatteryForeground' RCL.

Try it
'LowBatteryForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter LowBatteryForeground. Save the session to keep it after restart.

## LowerAlphaForeground

The drawing pattern for lower alpha foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by LowerAlphaForeground. Read the current value with 'LowerAlphaForeground' RCL.

Try it
'LowerAlphaForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter LowerAlphaForeground. Save the session to keep it after restart.

## LowerCase

Choose lower case rendering for calculator command names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: LowerCaseCommands.

Try it
LowerCase

Applies the display or angle choice described above.

## LowerCaseNames

Choose lower case rendering for symbolic names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: LowerCaseSymbols.

Try it
LowerCaseNames

Applies the display or angle choice described above.

## MantissaSpacing

How decimal mantissa digits are grouped for reading. The allowed values are 0 to 15.

To change it, enter a value followed by MantissaSpacing. Read the current value with 'MantissaSpacing' RCL.

Try it
'MantissaSpacing' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MantissaSpacing. Save the session to keep it after restart.

## MaxCallRecursion

How deeply RPL programs may call other programs recursively.

To change it, enter a value followed by MaxCallRecursion. Read the current value with 'MaxCallRecursion' RCL.

Try it
'MaxCallRecursion' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxCallRecursion. Save the session to keep it after restart.

## MaxFactorIterations

The work limit for integer factorization.

To change it, enter a value followed by MaxFactorIterations. Read the current value with 'MaxFactorIterations' RCL.

Try it
'MaxFactorIterations' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxFactorIterations. Save the session to keep it after restart.

## MaxFactorsBits

The largest integer bit size accepted by the engine factorization operation.

To change it, enter a value followed by MaxFactorsBits. Read the current value with 'MaxFactorsBits' RCL.

Try it
'MaxFactorsBits' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxFactorsBits. Save the session to keep it after restart.

## MaxFlags

The number of numbered user flags available to RPL programs.

To change it, enter a value followed by MaxFlags. Read the current value with 'MaxFlags' RCL.

Try it
'MaxFlags' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxFlags. Save the session to keep it after restart.

## MaximumDecimalExponent

The allowed decimal exponent magnitude. Changing it does not create more memory for large numbers.

To change it, enter a value followed by MaximumDecimalExponent. Read the current value with 'MaximumDecimalExponent' RCL.

Try it
'MaximumDecimalExponent' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaximumDecimalExponent. Save the session to keep it after restart.

## MaximumShowHeight

The maximum enlarged calculator-object rendering height in pixels.

To change it, enter a value followed by MaximumShowHeight. Read the current value with 'MaximumShowHeight' RCL.

You can also enter: MaxH.

Try it
'MaximumShowHeight' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaximumShowHeight. Save the session to keep it after restart.

## MaximumShowWidth

The maximum enlarged calculator-object rendering width in pixels.

To change it, enter a value followed by MaximumShowWidth. Read the current value with 'MaximumShowWidth' RCL.

You can also enter: MaxW.

Try it
'MaximumShowWidth' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaximumShowWidth. Save the session to keep it after restart.

## MaxLaguerreIterations

The iteration limit for the Laguerre polynomial-root method.

To change it, enter a value followed by MaxLaguerreIterations. Read the current value with 'MaxLaguerreIterations' RCL.

Try it
'MaxLaguerreIterations' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxLaguerreIterations. Save the session to keep it after restart.

## MaxNameChainLength

How many chained name lookups the engine may follow.

To change it, enter a value followed by MaxNameChainLength. Read the current value with 'MaxNameChainLength' RCL.

Try it
'MaxNameChainLength' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxNameChainLength. Save the session to keep it after restart.

## MaxNumberBits

The largest integer bit size the engine allows.

To change it, enter a value followed by MaxNumberBits. Read the current value with 'MaxNumberBits' RCL.

Try it
'MaxNumberBits' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxNumberBits. Save the session to keep it after restart.

## MaxPolynomialDegree

The polynomial degree limit for engine polynomial processing.

To change it, enter a value followed by MaxPolynomialDegree. Read the current value with 'MaxPolynomialDegree' RCL.

Try it
'MaxPolynomialDegree' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxPolynomialDegree. Save the session to keep it after restart.

## MaxRewrites

The iteration limit for symbolic rewriting, so a rewrite cannot continue indefinitely.

To change it, enter a value followed by MaxRewrites. Read the current value with 'MaxRewrites' RCL.

Try it
'MaxRewrites' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxRewrites. Save the session to keep it after restart.

## MaxRootDivisor

The divisor-search limit used by polynomial-root processing.

To change it, enter a value followed by MaxRootDivisor. Read the current value with 'MaxRootDivisor' RCL.

Try it
'MaxRootDivisor' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxRootDivisor. Save the session to keep it after restart.

## MaxStackDepth

The permitted calculator stack depth. Raising this limit does not enlarge the calculator arena.

To change it, enter a value followed by MaxStackDepth. Read the current value with 'MaxStackDepth' RCL.

Try it
'MaxStackDepth' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaxStackDepth. Save the session to keep it after restart.

## MenuAppearance

The calculator soft-menu row layout. Choose SingleRowMenus, ThreeRowsMenus or FlatMenus rather than a raw layout ID.

To change it, enter a value followed by MenuAppearance. Read the current value with 'MenuAppearance' RCL.

Try it
'MenuAppearance' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MenuAppearance. Save the session to keep it after restart.

## MenuBackground

The drawing pattern for menu background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by MenuBackground. Read the current value with 'MenuBackground' RCL.

Try it
'MenuBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MenuBackground. Save the session to keep it after restart.

## MinimumBatteryVoltage

A saved compatibility threshold from the original hardware. PicoCalc uses controller battery percentage for its safety check, not this voltage value.

To change it, enter a value followed by MinimumBatteryVoltage. Read the current value with 'MinimumBatteryVoltage' RCL.

Try it
'MinimumBatteryVoltage' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MinimumBatteryVoltage. Save the session to keep it after restart.

## MinimumSignificantDigits

The minimum significant-digit count used by standard number display.

To change it, enter a value followed by MinimumSignificantDigits. Read the current value with 'MinimumSignificantDigits' RCL.

Try it
'MinimumSignificantDigits' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MinimumSignificantDigits. Save the session to keep it after restart.

## MixedFractions

ImproperFractions displays the whole numerator over the denominator. MixedFractions separates the integer part from the fractional remainder.

Try it
MixedFractions

Applies the choice described above without a stack argument. Enter ImproperFractions to choose its opposite. Save the session to keep this preference.

## ModernBasedNumbers

CompatibleBasedNumbers uses older based-number notation. ModernBasedNumbers uses the engine's modern notation; the integer value is unchanged.

Try it
ModernBasedNumbers

Applies the choice described above without a stack argument. Enter CompatibleBasedNumbers to choose its opposite. Save the session to keep this preference.

## MonthBeforeDay

MonthBeforeDay places month before day. DayBeforeMonth places day before month.

Try it
MonthBeforeDay

Applies the choice described above without a stack argument. Enter DayBeforeMonth to choose its opposite. Save the session to keep this preference.

## MultilineCursorFont

The built-in font used for the multiline calculator cursor. Font IDs 0-12 are choices, not pixel heights. This does not resize native Help, menus or source editing.

To change it, enter a value followed by MultilineCursorFont. Read the current value with 'MultilineCursorFont' RCL.

Try it
'MultilineCursorFont' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MultilineCursorFont. Save the session to keep it after restart.

## MultilineEditorFont

The built-in font used for multiline calculator entry. Font IDs 0-12 are choices, not pixel heights. This does not resize native Help, menus or source editing.

To change it, enter a value followed by MultilineEditorFont. Read the current value with 'MultilineEditorFont' RCL.

Try it
'MultilineEditorFont' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MultilineEditorFont. Save the session to keep it after restart.

## MultiLineResult

SingleLineResult keeps calculator result rendering to one line. MultiLineResult allows it to span several lines.

Try it
MultiLineResult

Applies the choice described above without a stack argument. Enter SingleLineResult to choose its opposite. Save the session to keep this preference.

## MultiLineStack

MultiLineStack lets stack objects occupy several lines. SingleLineStack keeps their single-line presentation.

Try it
MultiLineStack

Applies the choice described above without a stack argument. Enter SingleLineStack to choose its opposite. Save the session to keep this preference.

## NameDisplayMode

How symbolic names are displayed. Use the named symbol-display choices rather than raw IDs.

To change it, enter a value followed by NameDisplayMode. Read the current value with 'NameDisplayMode' RCL.

Try it
'NameDisplayMode' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter NameDisplayMode. Save the session to keep it after restart.

## NeedToClearErrors

NeedToClearErrors requires dismissing a calculator error before continuing. NoNeedToClearErrors relaxes that engine requirement. Esc always dismisses a visible error before navigating menus.

Try it
NeedToClearErrors

Applies the choice described above without a stack argument. Enter NoNeedToClearErrors to choose its opposite. Save the session to keep this preference.

## NegativeUnderflowIndicator

NegativeUnderflowIndicator shows the engine's negative-underflow indication. NoNegativeUnderflow hides that indication.

Try it
NegativeUnderflowIndicator

Applies the choice described above without a stack argument. Enter NoNegativeUnderflow to choose its opposite. Save the session to keep this preference.

## NewStylePolynomials

CompatiblePolynomials selects older polynomial conventions. NewStylePolynomials selects DB48X conventions; choose the one expected by your RPL code.

Try it
NewStylePolynomials

Applies the choice described above without a stack argument. Enter CompatiblePolynomials to choose its opposite. Save the session to keep this preference.

## NoAngleUnits

SetAngleUnits attaches explicit angle units to applicable results. NoAngleUnits leaves those unit annotations off.

Try it
NoAngleUnits

Applies the choice described above without a stack argument. Enter SetAngleUnits to choose its opposite. Save the session to keep this preference.

## NoAutoSimplify

AutoSimplify simplifies expressions automatically. NoAutoSimplify lets you retain their unsimplified structure for further symbolic work.

Try it
NoAutoSimplify

Applies the choice described above without a stack argument. Enter AutoSimplify to choose its opposite. Save the session to keep this preference.

## NoCurveFilling

CurveFilling fills engine plotted curves where supported. NoCurveFilling leaves them unfilled.

Try it
NoCurveFilling

Applies the choice described above without a stack argument. Enter CurveFilling to choose its opposite. Save the session to keep this preference.

## NoErrorForTooManyDigits

TooManyDigitsErrors reports excess entered digits as an error. NoErrorForTooManyDigits lets the engine handle them without that error.

Try it
NoErrorForTooManyDigits

Applies the choice described above without a stack argument. Enter TooManyDigitsErrors to choose its opposite. Save the session to keep this preference.

## NoInfiniteResult

InfiniteResultIndicator shows the infinite-result indication. NoInfiniteResult hides it; the separate infinity policy controls whether the result is allowed.

Try it
NoInfiniteResult

Applies the choice described above without a stack argument. Enter InfiniteResultIndicator to choose its opposite. Save the session to keep this preference.

## NoLastArguments

SaveLastArguments remembers command inputs for later recall. NoLastArguments stops keeping them.

Try it
NoLastArguments

Applies the choice described above without a stack argument. Enter SaveLastArguments to choose its opposite. Save the session to keep this preference.

## NoLeadingZero

LeadingZero displays the zero before a fractional decimal. NoLeadingZero omits it, giving forms such as .5.

Try it
NoLeadingZero

Applies the choice described above without a stack argument. Enter LeadingZero to choose its opposite. Save the session to keep this preference.

## NoNeedToClearErrors

NeedToClearErrors requires dismissing a calculator error before continuing. NoNeedToClearErrors relaxes that engine requirement. Esc always dismisses a visible error before navigating menus.

Try it
NoNeedToClearErrors

Applies the choice described above without a stack argument. Enter NeedToClearErrors to choose its opposite. Save the session to keep this preference.

## NoNegativeUnderflow

NegativeUnderflowIndicator shows the engine's negative-underflow indication. NoNegativeUnderflow hides that indication.

Try it
NoNegativeUnderflow

Applies the choice described above without a stack argument. Enter NegativeUnderflowIndicator to choose its opposite. Save the session to keep this preference.

## NoNumberedVariables

NumberedVariables enables the engine's numbered-variable behavior. NoNumberedVariables disables it. Use ordinary named HOME variables unless your RPL code needs this option.

Try it
NoNumberedVariables

Applies the choice described above without a stack argument. Enter NumberedVariables to choose its opposite. Save the session to keep this preference.

## NoOverflow

OverflowIndicator shows the engine's overflow indication. NoOverflow hides the indication rather than changing the numerical overflow policy.

Try it
NoOverflow

Applies the choice described above without a stack argument. Enter OverflowIndicator to choose its opposite. Save the session to keep this preference.

## NoPlotAxes

DrawPlotAxes includes axes in engine RPL plots. NoPlotAxes omits them. The native graph tools draw their own axes.

Try it
NoPlotAxes

Applies the choice described above without a stack argument. Enter DrawPlotAxes to choose its opposite. Save the session to keep this preference.

## NoPositiveUnderflow

PositiveUnderflowIndicator shows the engine's positive-underflow indication. NoPositiveUnderflow hides it.

Try it
NoPositiveUnderflow

Applies the choice described above without a stack argument. Enter PositiveUnderflowIndicator to choose its opposite. Save the session to keep this preference.

## NoProgramLastArguments

ProgramLastArguments also remembers inputs during program execution. NoProgramLastArguments leaves that saving off inside programs.

Try it
NoProgramLastArguments

Applies the choice described above without a stack argument. Enter ProgramLastArguments to choose its opposite. Save the session to keep this preference.

## NormalPolynomialRender

PrefixPolynomialRender shows polynomial objects in prefix form. NormalPolynomialRender uses their mathematical form.

Try it
NormalPolynomialRender

Applies the choice described above without a stack argument. Enter PrefixPolynomialRender to choose its opposite. Save the session to keep this preference.

## NoStackAutoScale

AutoScaleStack can resize a stack object's rendering to fit. NoStackAutoScale keeps the selected scale.

Try it
NoStackAutoScale

Applies the choice described above without a stack argument. Enter AutoScaleStack to choose its opposite. Save the session to keep this preference.

## NoStackSave

SaveStack keeps a stack snapshot for Undo. NoStackSave avoids that saved copy, so an operation may no longer be undoable.

Try it
NoStackSave

Applies the choice described above without a stack argument. Enter SaveStack to choose its opposite. Save the session to keep this preference.

## NoTrailingDecimal

TrailingDecimal shows the decimal separator where needed to distinguish a decimal value. NoTrailingDecimal omits an unnecessary trailing separator.

Try it
NoTrailingDecimal

Applies the choice described above without a stack argument. Enter TrailingDecimal to choose its opposite. Save the session to keep this preference.

## NoUndefinedResult

UndefinedResultIndicator shows the undefined-result indication. NoUndefinedResult hides it; the separate undefined-result policy controls errors.

Try it
NoUndefinedResult

Applies the choice described above without a stack argument. Enter UndefinedResultIndicator to choose its opposite. Save the session to keep this preference.

## NumberDotOrComma

Separate groups of decimal digits with dots or commas.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
NumberDotOrComma

Applies the display or angle choice described above.

## NumberedVariables

NumberedVariables enables the engine's numbered-variable behavior. NoNumberedVariables disables it. Use ordinary named HOME variables unless your RPL code needs this option.

Try it
NumberedVariables

Applies the choice described above without a stack argument. Enter NoNumberedVariables to choose its opposite. Save the session to keep this preference.

## NumberSeparatorCommand

The separator between groups of decimal digits. Choose NumberSpaces, NumberDotOrComma, NumberTicks or NumberUnderscore.

To change it, enter a value followed by NumberSeparatorCommand. Read the current value with 'NumberSeparatorCommand' RCL.

Try it
'NumberSeparatorCommand' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter NumberSeparatorCommand. Save the session to keep it after restart.

## NumberSpaces

Separate groups of decimal digits with spaces.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
NumberSpaces

Applies the display or angle choice described above.

## NumberTicks

Separate groups of decimal digits with ticks.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
NumberTicks

Applies the display or angle choice described above.

## NumberUnderscore

Separate groups of decimal digits with underscores.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
NumberUnderscore

Applies the display or angle choice described above.

## NumericalConstants

NumericalConstants turns constants into numbers when evaluated. SymbolicConstants keeps names such as pi symbolic where possible.

Try it
NumericalConstants

Applies the choice described above without a stack argument. Enter SymbolicConstants to choose its opposite. Save the session to keep this preference.

## NumericalIntegration

NumericalIntegration selects engine numerical integration. SymbolicIntegration selects symbolic integration where supported.

Try it
NumericalIntegration

Applies the choice described above without a stack argument. Enter SymbolicIntegration to choose its opposite. Save the session to keep this preference.

## NumericalResults

NumericalResults prefers decimal answers. SymbolicResults keeps exact expressions and fractions when the operation supports them.

Try it
NumericalResults

Applies the choice described above without a stack argument. Enter SymbolicResults to choose its opposite. Save the session to keep this preference.

## NumericalSolver

NumericalSolver selects engine numerical equation solving. SymbolicSolver selects symbolic solving. The guided workbench offers these as separate explicit actions.

Try it
NumericalSolver

Applies the choice described above without a stack argument. Enter SymbolicSolver to choose its opposite. Save the session to keep this preference.

## Oct

Show based integers in base 8. On LumaRPL this also makes a fixed-base integer at the top follow the selected display base, without changing its value. Fixed-base values below the top retain their representation. Ordinary decimal integers at the top convert directly too; decimal fractions remain unchanged. Negative integers use the current WordSize, as with RealToBinary. Ctrl+Y opens Bases from the calculator.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Octal.

Try it
#FFD2h Oct

You should get: #17 7722₈
The value is 65490 in decimal. Decimal integers and explicit binary, octal or hexadecimal integers convert directly. Decimal fractions are not converted.

## OnesComplement

OnesComplement interprets fixed-width signed based integers in ones-complement form. TwosComplement selects twos-complement form.

Try it
OnesComplement

Applies the choice described above without a stack argument. Enter TwosComplement to choose its opposite. Save the session to keep this preference.

## OverflowError

OverflowError stops on numerical overflow. OverflowValue allows an overflow value instead.

Try it
OverflowError

Applies the choice described above without a stack argument. Enter OverflowValue to choose its opposite. Save the session to keep this preference.

## OverflowIndicator

OverflowIndicator shows the engine's overflow indication. NoOverflow hides the indication rather than changing the numerical overflow policy.

Try it
OverflowIndicator

Applies the choice described above without a stack argument. Enter NoOverflow to choose its opposite. Save the session to keep this preference.

## OverflowValue

OverflowError stops on numerical overflow. OverflowValue allows an overflow value instead.

Try it
OverflowValue

Applies the choice described above without a stack argument. Enter OverflowError to choose its opposite. Save the session to keep this preference.

## PackedBitmaps

CompatibleGROBs uses the compatibility graphics-object representation. PackedBitmaps uses packed bitmaps.

Try it
PackedBitmaps

Applies the choice described above without a stack argument. Enter CompatibleGROBs to choose its opposite. Save the session to keep this preference.

## PiRadians

Measure angles in multiples of pi radians: 2 is a full turn, and 0.5 is a right angle.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: πradians.

Try it
PiRadians

Applies the display or angle choice described above.

## PlotErrorBackground

The drawing pattern for plot error background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by PlotErrorBackground. Read the current value with 'PlotErrorBackground' RCL.

Try it
'PlotErrorBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter PlotErrorBackground. Save the session to keep it after restart.

## PlotRefreshRate

The refresh interval used by engine plots. It is not the native graph's sample count.

To change it, enter a value followed by PlotRefreshRate. Read the current value with 'PlotRefreshRate' RCL.

Try it
'PlotRefreshRate' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter PlotRefreshRate. Save the session to keep it after restart.

## PositiveUnderflowIndicator

PositiveUnderflowIndicator shows the engine's positive-underflow indication. NoPositiveUnderflow hides it.

Try it
PositiveUnderflowIndicator

Applies the choice described above without a stack argument. Enter NoPositiveUnderflow to choose its opposite. Save the session to keep this preference.

## Precision

How many significant decimal digits calculations use. This affects arithmetic precision; Fix only changes displayed decimal places. For example, enter 16 Precision.

To change it, enter a value followed by Precision. Read the current value with 'Precision' RCL.

You can also enter: Prec.

Try it
16 Precision 'Precision' RCL

You should get: 16
Changes this calculator setting. Save the session to retain it after restart.

## PrefixPolynomialRender

PrefixPolynomialRender shows polynomial objects in prefix form. NormalPolynomialRender uses their mathematical form.

Try it
PrefixPolynomialRender

Applies the choice described above without a stack argument. Enter NormalPolynomialRender to choose its opposite. Save the session to keep this preference.

## PrincipalSolution

PrincipalSolution prefers a principal symbolic solution. AllSolutions asks for supported solution families. This does not make a numerical root search exhaustive.

Try it
PrincipalSolution

Applies the choice described above without a stack argument. Enter AllSolutions to choose its opposite. Save the session to keep this preference.

## ProgramLastArguments

ProgramLastArguments also remembers inputs during program execution. NoProgramLastArguments leaves that saving off inside programs.

Try it
ProgramLastArguments

Applies the choice described above without a stack argument. Enter NoProgramLastArguments to choose its opposite. Save the session to keep this preference.

## PushEvaluatedAssignment

PushEvaluatedAssignment puts the evaluated assignment on the stack. PushOriginalAssignment retains the original assignment expression.

Try it
PushEvaluatedAssignment

Applies the choice described above without a stack argument. Enter PushOriginalAssignment to choose its opposite. Save the session to keep this preference.

## PushOriginalAssignment

PushEvaluatedAssignment puts the evaluated assignment on the stack. PushOriginalAssignment retains the original assignment expression.

Try it
PushOriginalAssignment

Applies the choice described above without a stack argument. Enter PushEvaluatedAssignment to choose its opposite. Save the session to keep this preference.

## Rad

Use radians for angles: a full turn is 2*pi.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Radians.

Try it
Rad

Applies the display or angle choice described above.

## RandomGeneratorBits

The bit count used by the engine random-number generator.

To change it, enter a value followed by RandomGeneratorBits. Read the current value with 'RandomGeneratorBits' RCL.

Try it
'RandomGeneratorBits' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter RandomGeneratorBits. Save the session to keep it after restart.

## RandomGeneratorOrder

The engine random-number generator's order parameter. Keep the default unless your RPL work requires another configuration.

To change it, enter a value followed by RandomGeneratorOrder. Read the current value with 'RandomGeneratorOrder' RCL.

Try it
'RandomGeneratorOrder' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter RandomGeneratorOrder. Save the session to keep it after restart.

## RealResults

ComplexResults allows operations to return complex values. RealResults keeps real-only behavior where the operation supports that choice.

Try it
RealResults

Applies the choice described above without a stack argument. Enter ComplexResults to choose its opposite. Save the session to keep this preference.

## ReportPrecisionLoss

ReportPrecisionLoss raises an error when precision is lost. IgnorePrecisionLoss lets the calculation continue without that error.

Try it
ReportPrecisionLoss

Applies the choice described above without a stack argument. Enter IgnorePrecisionLoss to choose its opposite. Save the session to keep this preference.

## ResultBackground

The drawing pattern for result background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by ResultBackground. Read the current value with 'ResultBackground' RCL.

Try it
'ResultBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ResultBackground. Save the session to keep it after restart.

## ResultFont

The built-in font used for calculator answers. Font IDs 0-12 are choices, not pixel heights. This does not resize native Help, menus or source editing.

To change it, enter a value followed by ResultFont. Read the current value with 'ResultFont' RCL.

Try it
'ResultFont' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ResultFont. Save the session to keep it after restart.

## ResultForeground

The drawing pattern for result foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by ResultForeground. Read the current value with 'ResultForeground' RCL.

Try it
'ResultForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ResultForeground. Save the session to keep it after restart.

## ResultGraphingTimeLimit

The time allowed for graphically rendering a calculator result, in milliseconds.

To change it, enter a value followed by ResultGraphingTimeLimit. Read the current value with 'ResultGraphingTimeLimit' RCL.

Try it
'ResultGraphingTimeLimit' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ResultGraphingTimeLimit. Save the session to keep it after restart.

## RightShiftBackground

The drawing pattern for right shift background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by RightShiftBackground. Read the current value with 'RightShiftBackground' RCL.

Try it
'RightShiftBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter RightShiftBackground. Save the session to keep it after restart.

## RightShiftForeground

The drawing pattern for right shift foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by RightShiftForeground. Read the current value with 'RightShiftForeground' RCL.

Try it
'RightShiftForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter RightShiftForeground. Save the session to keep it after restart.

## RoundedMenus

SquareMenus draws square calculator soft-menu labels. RoundedMenus draws rounded labels. Native Home lists keep their usual layout.

Try it
RoundedMenus

Applies the choice described above without a stack argument. Enter SquareMenus to choose its opposite. Save the session to keep this preference.

## RoundMenuBackground

The drawing pattern for round menu background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by RoundMenuBackground. Read the current value with 'RoundMenuBackground' RCL.

Try it
'RoundMenuBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter RoundMenuBackground. Save the session to keep it after restart.

## RoundMenuForeground

The drawing pattern for round menu foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by RoundMenuForeground. Read the current value with 'RoundMenuForeground' RCL.

Try it
'RoundMenuForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter RoundMenuForeground. Save the session to keep it after restart.

## RunningIconForeground

The drawing pattern for running icon foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by RunningIconForeground. Read the current value with 'RunningIconForeground' RCL.

Try it
'RunningIconForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter RunningIconForeground. Save the session to keep it after restart.

## RunStatsClearAfterRead

RunStatsClearAfterRead resets execution statistics when read. RunStatsKeepAfterRead leaves them accumulated.

Try it
RunStatsClearAfterRead

Applies the choice described above without a stack argument. Enter RunStatsKeepAfterRead to choose its opposite. Save the session to keep this preference.

## RunStatsKeepAfterRead

RunStatsClearAfterRead resets execution statistics when read. RunStatsKeepAfterRead leaves them accumulated.

Try it
RunStatsKeepAfterRead

Applies the choice described above without a stack argument. Enter RunStatsClearAfterRead to choose its opposite. Save the session to keep this preference.

## SaveLastArguments

SaveLastArguments remembers command inputs for later recall. NoLastArguments stops keeping them.

Try it
SaveLastArguments

Applies the choice described above without a stack argument. Enter NoLastArguments to choose its opposite. Save the session to keep this preference.

## SaveStack

SaveStack keeps a stack snapshot for Undo. NoStackSave avoids that saved copy, so an operation may no longer be undoable.

Try it
SaveStack

Applies the choice described above without a stack argument. Enter NoStackSave to choose its opposite. Save the session to keep this preference.

## Sci

Show numbers in scientific notation with the supplied digit count.

Put the digit count on the stack first. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: ScientificDisplay.

Try it
3 Sci

Shows three digits using this display style.

## SearchBackground

The drawing pattern for search background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by SearchBackground. Read the current value with 'SearchBackground' RCL.

Try it
'SearchBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SearchBackground. Save the session to keep it after restart.

## SearchForeground

The drawing pattern for search foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by SearchForeground. Read the current value with 'SearchForeground' RCL.

Try it
'SearchForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SearchForeground. Save the session to keep it after restart.

## SelectedMenuForeground

The drawing pattern for selected menu foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by SelectedMenuForeground. Read the current value with 'SelectedMenuForeground' RCL.

Try it
'SelectedMenuForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SelectedMenuForeground. Save the session to keep it after restart.

## SelectionBackground

The drawing pattern for selection background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by SelectionBackground. Read the current value with 'SelectionBackground' RCL.

Try it
'SelectionBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SelectionBackground. Save the session to keep it after restart.

## SelectionForeground

The drawing pattern for selection foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by SelectionForeground. Read the current value with 'SelectionForeground' RCL.

Try it
'SelectionForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SelectionForeground. Save the session to keep it after restart.

## SetAngleUnits

SetAngleUnits attaches explicit angle units to applicable results. NoAngleUnits leaves those unit annotations off.

Try it
SetAngleUnits

Applies the choice described above without a stack argument. Enter NoAngleUnits to choose its opposite. Save the session to keep this preference.

## ShowAmbiguousNames

HideAmbiguousNames leaves ambiguous names out of relevant engine listings. ShowAmbiguousNames includes them.

Try it
ShowAmbiguousNames

Applies the choice described above without a stack argument. Enter HideAmbiguousNames to choose its opposite. Save the session to keep this preference.

## ShowAsDecimal

ShowAsDecimal displays integers and fractions as decimals. ShowIntegersAndFractions keeps their integer/fraction presentation.

Try it
ShowAsDecimal

Applies the choice described above without a stack argument. Enter ShowIntegersAndFractions to choose its opposite. Save the session to keep this preference.

## ShowBuiltinCharacters

ShowBuiltinCharacters includes built-in character-library entries. HideBuiltinCharacters hides those entries.

Try it
ShowBuiltinCharacters

Applies the choice described above without a stack argument. Enter HideBuiltinCharacters to choose its opposite. Save the session to keep this preference.

## ShowBuiltinConstants

ShowBuiltinConstants includes built-in constants in the library. HideBuiltinConstants hides those entries.

Try it
ShowBuiltinConstants

Applies the choice described above without a stack argument. Enter HideBuiltinConstants to choose its opposite. Save the session to keep this preference.

## ShowBuiltinEquations

ShowBuiltinEquations includes built-in equation-library entries. HideBuiltinEquations hides them from that listing.

Try it
ShowBuiltinEquations

Applies the choice described above without a stack argument. Enter HideBuiltinEquations to choose its opposite. Save the session to keep this preference.

## ShowBuiltinLibrary

ShowBuiltinLibrary includes built-in program-library entries. HideBuiltinLibrary hides them from that listing.

Try it
ShowBuiltinLibrary

Applies the choice described above without a stack argument. Enter HideBuiltinLibrary to choose its opposite. Save the session to keep this preference.

## ShowBuiltinUnits

ShowBuiltinUnits includes built-in units in the unit library. HideBuiltinUnits hides those entries from that listing.

Try it
ShowBuiltinUnits

Applies the choice described above without a stack argument. Enter HideBuiltinUnits to choose its opposite. Save the session to keep this preference.

## ShowDate

ShowDate includes the date in the calculator header. HideDate leaves it out.

Try it
ShowDate

Applies the choice described above without a stack argument. Enter HideDate to choose its opposite. Save the session to keep this preference.

## ShowDayOfWeek

ShowDayOfWeek includes the weekday in the header date. HideDayOfWeek omits it.

Try it
ShowDayOfWeek

Applies the choice described above without a stack argument. Enter HideDayOfWeek to choose its opposite. Save the session to keep this preference.

## ShowEmptyMenu

ShowEmptyMenu leaves an empty calculator menu visible. HideEmptyMenu hides it.

Try it
ShowEmptyMenu

Applies the choice described above without a stack argument. Enter HideEmptyMenu to choose its opposite. Save the session to keep this preference.

## ShowEquationBody

ShowEquationBody displays the contents of an equation. HideEquationBody hides the body in the relevant equation display.

Try it
ShowEquationBody

Applies the choice described above without a stack argument. Enter HideEquationBody to choose its opposite. Save the session to keep this preference.

## ShowIntegersAndFractions

ShowAsDecimal displays integers and fractions as decimals. ShowIntegersAndFractions keeps their integer/fraction presentation.

Try it
ShowIntegersAndFractions

Applies the choice described above without a stack argument. Enter ShowAsDecimal to choose its opposite. Save the session to keep this preference.

## ShowMonthName

ShowMonthName uses a month name in dates. HideMonthName uses a numeric month.

Try it
ShowMonthName

Applies the choice described above without a stack argument. Enter HideMonthName to choose its opposite. Save the session to keep this preference.

## ShowSeconds

ShowSeconds includes seconds in the header clock. HideSeconds shows only hours and minutes.

Try it
ShowSeconds

Applies the choice described above without a stack argument. Enter HideSeconds to choose its opposite. Save the session to keep this preference.

## ShowTime

ShowTime includes the clock in the calculator header. HideTime leaves it out.

Try it
ShowTime

Applies the choice described above without a stack argument. Enter HideTime to choose its opposite. Save the session to keep this preference.

## ShowTimeLimit

How long the engine SHOW operation may spend rendering, in milliseconds.

To change it, enter a value followed by ShowTimeLimit. Read the current value with 'ShowTimeLimit' RCL.

Try it
'ShowTimeLimit' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter ShowTimeLimit. Save the session to keep it after restart.

## ShowVoltage

HideVoltage hides the legacy voltage display. ShowVoltage requests it, but PicoCalc cannot supply a measured voltage here. Use Device status > Battery for percentage and charging state.

Try it
ShowVoltage

Applies the choice described above without a stack argument. Enter HideVoltage to choose its opposite. Save the session to keep this preference.

## Sig

Choose how many significant digits to display.

Put the digit count on the stack first. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: SignificantDisplay.

Try it
3 Sig

Shows three digits using this display style.

## SilentBeepOff

SilentBeepOn enables the engine's silent-beep indication. SilentBeepOff disables that indication.

Try it
SilentBeepOff

Applies the choice described above without a stack argument. Enter SilentBeepOn to choose its opposite. Save the session to keep this preference.

## SilentBeepOn

SilentBeepOn enables the engine's silent-beep indication. SilentBeepOff disables that indication.

Try it
SilentBeepOn

Applies the choice described above without a stack argument. Enter SilentBeepOff to choose its opposite. Save the session to keep this preference.

## SingleLineResult

SingleLineResult keeps calculator result rendering to one line. MultiLineResult allows it to span several lines.

Try it
SingleLineResult

Applies the choice described above without a stack argument. Enter MultiLineResult to choose its opposite. Save the session to keep this preference.

## SingleLineStack

MultiLineStack lets stack objects occupy several lines. SingleLineStack keeps their single-line presentation.

Try it
SingleLineStack

Applies the choice described above without a stack argument. Enter MultiLineStack to choose its opposite. Save the session to keep this preference.

## SingleRowMenus

Show one calculator soft-menu row at a time.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
SingleRowMenus

Applies the display or angle choice described above.

## SkippedMenuBackground

The drawing pattern for skipped menu background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by SkippedMenuBackground. Read the current value with 'SkippedMenuBackground' RCL.

Try it
'SkippedMenuBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SkippedMenuBackground. Save the session to keep it after restart.

## SmallFractions

BigFractions gives fractions larger typography. SmallFractions uses the compact fraction style.

Try it
SmallFractions

Applies the choice described above without a stack argument. Enter BigFractions to choose its opposite. Save the session to keep this preference.

## SoftwareDisplayRefresh

SoftwareDisplayRefresh and DMCPDisplayRefresh are retained engine compatibility choices. PicoCalc uses its own dirty-row LCD driver in either case.

Try it
SoftwareDisplayRefresh

Applies the choice described above without a stack argument. Enter DMCPDisplayRefresh to choose its opposite. Save the session to keep this preference.

## SoftwareFloatingPoint

HardwareFloatingPoint permits hardware floating-point arithmetic where supported. SoftwareFloatingPoint selects software arithmetic.

You can also enter: SFP, SoftFP.

Try it
SoftwareFloatingPoint

Applies the choice described above without a stack argument. Enter HardwareFloatingPoint to choose its opposite. Save the session to keep this preference.

## SolverImprecision

The precision allowance used by the engine root solver.

To change it, enter a value followed by SolverImprecision. Read the current value with 'SolverImprecision' RCL.

Try it
'SolverImprecision' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SolverImprecision. Save the session to keep it after restart.

## SolverIterations

The engine root solver's iteration limit. Guided workbench solving sets its own bounded solver context.

To change it, enter a value followed by SolverIterations. Read the current value with 'SolverIterations' RCL.

Try it
'SolverIterations' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SolverIterations. Save the session to keep it after restart.

## SolverShuffles

The shuffle allowance used by the numerical solver when seeking a solution.

To change it, enter a value followed by SolverShuffles. Read the current value with 'SolverShuffles' RCL.

Try it
'SolverShuffles' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SolverShuffles. Save the session to keep it after restart.

## SquareMenuBackground

The drawing pattern for square menu background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by SquareMenuBackground. Read the current value with 'SquareMenuBackground' RCL.

Try it
'SquareMenuBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SquareMenuBackground. Save the session to keep it after restart.

## SquareMenuForeground

The drawing pattern for square menu foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by SquareMenuForeground. Read the current value with 'SquareMenuForeground' RCL.

Try it
'SquareMenuForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SquareMenuForeground. Save the session to keep it after restart.

## SquareMenus

SquareMenus draws square calculator soft-menu labels. RoundedMenus draws rounded labels. Native Home lists keep their usual layout.

Try it
SquareMenus

Applies the choice described above without a stack argument. Enter RoundedMenus to choose its opposite. Save the session to keep this preference.

## StackBackground

The drawing pattern for stack background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by StackBackground. Read the current value with 'StackBackground' RCL.

Try it
'StackBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter StackBackground. Save the session to keep it after restart.

## StackFont

The built-in font used for values on the calculator stack. Font IDs 0-12 are choices, not pixel heights. This does not resize native Help, menus or source editing.

To change it, enter a value followed by StackFont. Read the current value with 'StackFont' RCL.

Try it
2 StackFont 'StackFont' RCL

You should get: 2
Changes this calculator setting. Save the session to retain it after restart.

## StackForeground

The drawing pattern for stack foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by StackForeground. Read the current value with 'StackForeground' RCL.

Try it
'StackForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter StackForeground. Save the session to keep it after restart.

## StackGraphingTimeLimit

The time allowed for graphically rendering a stack object, in milliseconds.

To change it, enter a value followed by StackGraphingTimeLimit. Read the current value with 'StackGraphingTimeLimit' RCL.

Try it
'StackGraphingTimeLimit' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter StackGraphingTimeLimit. Save the session to keep it after restart.

## StackLevelBackground

The drawing pattern for stack level background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by StackLevelBackground. Read the current value with 'StackLevelBackground' RCL.

Try it
'StackLevelBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter StackLevelBackground. Save the session to keep it after restart.

## StackLevelForeground

The drawing pattern for stack level foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by StackLevelForeground. Read the current value with 'StackLevelForeground' RCL.

Try it
'StackLevelForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter StackLevelForeground. Save the session to keep it after restart.

## StackLineForeground

The drawing pattern for stack line foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by StackLineForeground. Read the current value with 'StackLineForeground' RCL.

Try it
'StackLineForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter StackLineForeground. Save the session to keep it after restart.

## StandardExponent

The exponent threshold at which standard display switches notation.

To change it, enter a value followed by StandardExponent. Read the current value with 'StandardExponent' RCL.

Try it
9 StandardExponent 'StandardExponent' RCL

You should get: 9
Changes this calculator setting. Save the session to retain it after restart.

## StateNameForeground

The drawing pattern for state name foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by StateNameForeground. Read the current value with 'StateNameForeground' RCL.

Try it
'StateNameForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter StateNameForeground. Save the session to keep it after restart.

## StatsPlotBins

How many bins the engine statistical plot uses. The native Data tool has its own plot workflow.

To change it, enter a value followed by StatsPlotBins. Read the current value with 'StatsPlotBins' RCL.

Try it
'StatsPlotBins' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter StatsPlotBins. Save the session to keep it after restart.

## Std

Use standard number display. The calculator chooses a suitable decimal or exponent form.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: StandardDisplay.

Try it
Std

Applies the display or angle choice described above.

## StepByStepAlgebraResults

StepByStepAlgebraResults retains intermediate algebra results where supported. FinalAlgebraResults keeps the final result.

Try it
StepByStepAlgebraResults

Applies the choice described above without a stack argument. Enter FinalAlgebraResults to choose its opposite. Save the session to keep this preference.

## SteppingIconForeground

The drawing pattern for stepping icon foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by SteppingIconForeground. Read the current value with 'SteppingIconForeground' RCL.

Try it
'SteppingIconForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter SteppingIconForeground. Save the session to keep it after restart.

## StoreAtEnd

StoreAtEnd puts new variables at the end of calculator directory order. StoreAtStart puts them at the start.

Try it
StoreAtEnd

Applies the choice described above without a stack argument. Enter StoreAtStart to choose its opposite. Save the session to keep this preference.

## StoreAtStart

StoreAtEnd puts new variables at the end of calculator directory order. StoreAtStart puts them at the start.

Try it
StoreAtStart

Applies the choice described above without a stack argument. Enter StoreAtEnd to choose its opposite. Save the session to keep this preference.

## StrictArrayResizing

LaxArrayResizing permits the engine's relaxed array resizing. StrictArrayResizing requires strict resizing rules.

Try it
StrictArrayResizing

Applies the choice described above without a stack argument. Enter LaxArrayResizing to choose its opposite. Save the session to keep this preference.

## SymbolicConstants

NumericalConstants turns constants into numbers when evaluated. SymbolicConstants keeps names such as pi symbolic where possible.

Try it
SymbolicConstants

Applies the choice described above without a stack argument. Enter NumericalConstants to choose its opposite. Save the session to keep this preference.

## SymbolicIntegration

NumericalIntegration selects engine numerical integration. SymbolicIntegration selects symbolic integration where supported.

Try it
SymbolicIntegration

Applies the choice described above without a stack argument. Enter NumericalIntegration to choose its opposite. Save the session to keep this preference.

## SymbolicResults

NumericalResults prefers decimal answers. SymbolicResults keeps exact expressions and fractions when the operation supports them.

Try it
SymbolicResults

Applies the choice described above without a stack argument. Enter NumericalResults to choose its opposite. Save the session to keep this preference.

## SymbolicSolver

NumericalSolver selects engine numerical equation solving. SymbolicSolver selects symbolic solving. The guided workbench offers these as separate explicit actions.

Try it
SymbolicSolver

Applies the choice described above without a stack argument. Enter NumericalSolver to choose its opposite. Save the session to keep this preference.

## TabWidth

The tab width used in calculator rendering, rather than the SD source editor's indentation setting.

To change it, enter a value followed by TabWidth. Read the current value with 'TabWidth' RCL.

Try it
'TabWidth' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter TabWidth. Save the session to keep it after restart.

## TextRenderingSizeLimit

The engine text-rendering size limit for calculator objects.

To change it, enter a value followed by TextRenderingSizeLimit. Read the current value with 'TextRenderingSizeLimit' RCL.

Try it
'TextRenderingSizeLimit' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter TextRenderingSizeLimit. Save the session to keep it after restart.

## TextResultDisplay

TextResultDisplay shows calculator answers as source text. GraphicResultDisplay uses mathematical layout.

Try it
TextResultDisplay

Applies the choice described above without a stack argument. Enter GraphicResultDisplay to choose its opposite. Save the session to keep this preference.

## TextStackDisplay

TextStackDisplay shows stack objects as source text. GraphicStackDisplay typesets them as mathematics.

Try it
TextStackDisplay

Applies the choice described above without a stack argument. Enter GraphicStackDisplay to choose its opposite. Save the session to keep this preference.

## ThreeRowsMenus

Show the calculator soft-menu rows together.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

Try it
ThreeRowsMenus

Applies the display or angle choice described above.

## Time12H

Time12H uses a twelve-hour clock. Time24H uses hours from 0 through 23.

Try it
Time12H

Applies the choice described above without a stack argument. Enter Time24H to choose its opposite. Save the session to keep this preference.

## Time24H

Time12H uses a twelve-hour clock. Time24H uses hours from 0 through 23.

Try it
Time24H

Applies the choice described above without a stack argument. Enter Time12H to choose its opposite. Save the session to keep this preference.

## TimeForeground

The drawing pattern for time foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by TimeForeground. Read the current value with 'TimeForeground' RCL.

Try it
'TimeForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter TimeForeground. Save the session to keep it after restart.

## TooManyDigitsErrors

TooManyDigitsErrors reports excess entered digits as an error. NoErrorForTooManyDigits lets the engine handle them without that error.

Try it
TooManyDigitsErrors

Applies the choice described above without a stack argument. Enter NoErrorForTooManyDigits to choose its opposite. Save the session to keep this preference.

## TrailingDecimal

TrailingDecimal shows the decimal separator where needed to distinguish a decimal value. NoTrailingDecimal omits an unnecessary trailing separator.

Try it
TrailingDecimal

Applies the choice described above without a stack argument. Enter NoTrailingDecimal to choose its opposite. Save the session to keep this preference.

## TruthLogicForIntegers

TruthLogicForIntegers treats integers as truth values in logical operations. BitwiseLogicForIntegers operates on their bits.

Try it
TruthLogicForIntegers

Applies the choice described above without a stack argument. Enter BitwiseLogicForIntegers to choose its opposite. Save the session to keep this preference.

## TVMPayAtBeginningOfPeriod

TVMPayAtBeginningOfPeriod uses payments at the start of a period. TVMPayAtEndOfPeriod uses payments at its end in the engine TVM solver.

You can also enter: TVMBeg.

Try it
TVMPayAtBeginningOfPeriod

Applies the choice described above without a stack argument. Enter TVMPayAtEndOfPeriod to choose its opposite. Save the session to keep this preference.

## TVMPayAtEndOfPeriod

TVMPayAtBeginningOfPeriod uses payments at the start of a period. TVMPayAtEndOfPeriod uses payments at its end in the engine TVM solver.

You can also enter: TVMEnd.

Try it
TVMPayAtEndOfPeriod

Applies the choice described above without a stack argument. Enter TVMPayAtBeginningOfPeriod to choose its opposite. Save the session to keep this preference.

## TwoDigitYear

TwoDigitYear shortens the displayed year to two digits. FourDigitYear shows all four digits.

Try it
TwoDigitYear

Applies the choice described above without a stack argument. Enter FourDigitYear to choose its opposite. Save the session to keep this preference.

## TwosComplement

OnesComplement interprets fixed-width signed based integers in ones-complement form. TwosComplement selects twos-complement form.

Try it
TwosComplement

Applies the choice described above without a stack argument. Enter OnesComplement to choose its opposite. Save the session to keep this preference.

## UndefinedError

UndefinedValue permits an undefined-result object. UndefinedError stops with an error instead.

Try it
UndefinedError

Applies the choice described above without a stack argument. Enter UndefinedValue to choose its opposite. Save the session to keep this preference.

## UndefinedResultIndicator

UndefinedResultIndicator shows the undefined-result indication. NoUndefinedResult hides it; the separate undefined-result policy controls errors.

Try it
UndefinedResultIndicator

Applies the choice described above without a stack argument. Enter NoUndefinedResult to choose its opposite. Save the session to keep this preference.

## UndefinedValue

UndefinedValue permits an undefined-result object. UndefinedError stops with an error instead.

Try it
UndefinedValue

Applies the choice described above without a stack argument. Enter UndefinedError to choose its opposite. Save the session to keep this preference.

## UnderflowError

UnderflowError stops on numerical underflow. UnderflowValue allows the engine's underflow value to be returned instead.

Try it
UnderflowError

Applies the choice described above without a stack argument. Enter UnderflowValue to choose its opposite. Save the session to keep this preference.

## UnderflowValue

UnderflowError stops on numerical underflow. UnderflowValue allows the engine's underflow value to be returned instead.

Try it
UnderflowValue

Applies the choice described above without a stack argument. Enter UnderflowError to choose its opposite. Save the session to keep this preference.

## UnimplementedForeground

The drawing pattern for unimplemented foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by UnimplementedForeground. Read the current value with 'UnimplementedForeground' RCL.

Try it
'UnimplementedForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter UnimplementedForeground. Save the session to keep it after restart.

## UpperCase

Choose upper case rendering for calculator command names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: UpperCaseCommands.

Try it
UpperCase

Applies the display or angle choice described above.

## UpperCaseNames

Choose upper case rendering for symbolic names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: UpperCaseSymbols.

Try it
UpperCaseNames

Applies the display or angle choice described above.

## UseCrossForMultiplication

UseCrossForMultiplication displays a cross between factors. UseDotForMultiplication displays a dot. You can still type * on the keyboard.

Try it
UseCrossForMultiplication

Applies the choice described above without a stack argument. Enter UseDotForMultiplication to choose its opposite. Save the session to keep this preference.

## UseDotForMultiplication

UseCrossForMultiplication displays a cross between factors. UseDotForMultiplication displays a dot. You can still type * on the keyboard.

Try it
UseDotForMultiplication

Applies the choice described above without a stack argument. Enter UseCrossForMultiplication to choose its opposite. Save the session to keep this preference.

## UserCommandBackground

The drawing pattern for user command background. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by UserCommandBackground. Read the current value with 'UserCommandBackground' RCL.

Try it
'UserCommandBackground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter UserCommandBackground. Save the session to keep it after restart.

## UserCommandBorder

The drawing pattern for user command border. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by UserCommandBorder. Read the current value with 'UserCommandBorder' RCL.

Try it
'UserCommandBorder' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter UserCommandBorder. Save the session to keep it after restart.

## UserCommandForeground

The drawing pattern for user command foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by UserCommandForeground. Read the current value with 'UserCommandForeground' RCL.

Try it
'UserCommandForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter UserCommandForeground. Save the session to keep it after restart.

## UserMode

UserMode uses custom engine key assignments; UserModeOff restores the normal engine map. These assignments address engine virtual keys, not a new physical QWERTY layout.

Try it
UserMode

Applies the choice described above without a stack argument. Enter UserModeOff to choose its opposite. Save the session to keep this preference.

## UserModeLock

UserModeLock keeps user-key mode on until you turn it off. UserModeOnce applies it for one key action.

Try it
UserModeLock

Applies the choice described above without a stack argument. Enter UserModeOnce to choose its opposite. Save the session to keep this preference.

## UserModeOff

UserMode uses custom engine key assignments; UserModeOff restores the normal engine map. These assignments address engine virtual keys, not a new physical QWERTY layout.

Try it
UserModeOff

Applies the choice described above without a stack argument. Enter UserMode to choose its opposite. Save the session to keep this preference.

## UserModeOnce

UserModeLock keeps user-key mode on until you turn it off. UserModeOnce applies it for one key action.

Try it
UserModeOnce

Applies the choice described above without a stack argument. Enter UserModeLock to choose its opposite. Save the session to keep this preference.

## VariableWidthDigits

FixedWidthDigits gives mathematical digits equal widths. VariableWidthDigits uses proportional digit widths. Native Help keeps its own font.

Try it
VariableWidthDigits

Applies the choice described above without a stack argument. Enter FixedWidthDigits to choose its opposite. Save the session to keep this preference.

## VerticalLists

VerticalLists typesets lists down the page. HorizontalLists typesets them across it.

Try it
VerticalLists

Applies the choice described above without a stack argument. Enter HorizontalLists to choose its opposite. Save the session to keep this preference.

## VerticalProgramRendering

VerticalProgramRendering lays programs out down the page. HorizontalProgramRendering uses horizontal source-style layout.

Try it
VerticalProgramRendering

Applies the choice described above without a stack argument. Enter HorizontalProgramRendering to choose its opposite. Save the session to keep this preference.

## VerticalVectors

HorizontalVectors displays vectors across a row. VerticalVectors displays them down a column.

Try it
VerticalVectors

Applies the choice described above without a stack argument. Enter HorizontalVectors to choose its opposite. Save the session to keep this preference.

## VoltageForeground

The drawing pattern for voltage foreground. These are packed RPL pattern values, not RGB332 palette numbers. Choose a theme under Settings > Display if you want coordinated interface colours.

To change it, enter a value followed by VoltageForeground. Read the current value with 'VoltageForeground' RCL.

Try it
'VoltageForeground' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter VoltageForeground. Save the session to keep it after restart.

## WordSize

The bit width used by fixed-width based-number operations. This matters for shifts, rotations and signed interpretation.

To change it, enter a value followed by WordSize. Read the current value with 'WordSize' RCL.

You can also enter: stws.

Try it
32 WordSize 'WordSize' RCL

You should get: 32
Changes this calculator setting. Save the session to retain it after restart.

## XYPlotBins

The bin count used by the engine XY plot.

To change it, enter a value followed by XYPlotBins. Read the current value with 'XYPlotBins' RCL.

Try it
'XYPlotBins' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter XYPlotBins. Save the session to keep it after restart.

## YearFirst

YearFirst puts the year before the other date fields. YearLast puts it after them.

Try it
YearFirst

Applies the choice described above without a stack argument. Enter YearLast to choose its opposite. Save the session to keep this preference.

## YearLast

YearFirst puts the year before the other date fields. YearLast puts it after them.

Try it
YearLast

Applies the choice described above without a stack argument. Enter YearFirst to choose its opposite. Save the session to keep this preference.

## ZeroOverZeroIsError

ZeroOverZeroIsUndefined returns an undefined value for 0/0. ZeroOverZeroIsError reports an error.

Try it
ZeroOverZeroIsError

Applies the choice described above without a stack argument. Enter ZeroOverZeroIsUndefined to choose its opposite. Save the session to keep this preference.

## ZeroOverZeroIsUndefined

ZeroOverZeroIsUndefined returns an undefined value for 0/0. ZeroOverZeroIsError reports an error.

Try it
ZeroOverZeroIsUndefined

Applies the choice described above without a stack argument. Enter ZeroOverZeroIsError to choose its opposite. Save the session to keep this preference.

## ZeroPowerZeroIsOne

ZeroPowerZeroIsUndefined returns undefined for 0^0. ZeroPowerZeroIsOne returns 1.

Try it
ZeroPowerZeroIsOne

Applies the choice described above without a stack argument. Enter ZeroPowerZeroIsUndefined to choose its opposite. Save the session to keep this preference.

## ZeroPowerZeroIsUndefined

ZeroPowerZeroIsUndefined returns undefined for 0^0. ZeroPowerZeroIsOne returns 1.

Try it
ZeroPowerZeroIsUndefined

Applies the choice described above without a stack argument. Enter ZeroPowerZeroIsOne to choose its opposite. Save the session to keep this preference.

## AddColumn

Insert the supplied column into a matrix at the supplied position.

You can also enter: Column+, Col+.

Try it
[[ 1 2 3 ] [ 4 5 6 ]]  @ Input matrix
[ 22 33 ]              @ Column to insert
2                      @ Insertion position
COL+

You should get: [[ 1 22 2 3 ]
  [ 4 33 5 6 ]]
Run from the calculator stack. This example uses the current calculation settings.

## AddRow

Insert the supplied row into a matrix at the supplied position.

You can also enter: Row+.

Try it
[[ 1 2 3 ] [ 4 5 6 ]]  @ Input matrix
[ 22 33 44 ]           @ Row to insert
2                      @ Insertion position
ROW+

You should get: [[ 1 2 3 ]
  [ 22 33 44 ]
  [ 4 5 6 ]]
Run from the calculator stack. This example uses the current calculation settings.

## arg

Return the phase of a complex value in the current angle unit. Zero has no defined direction.

Try it
Deg (0;1) ARG

You should get: 90 °

## ColumnNorm

Find the matrix column norm.

You can also enter: CNrm.

## ColumnsToMatrix

Build a matrix from supplied column vectors.

You can also enter: Columns→, Col→.

Try it
[ 1 2 ]
[ 4 5 ]
[ 7 8 ]
3 ColumnsToMatrix

You should get: [[ 1 4 7 ]
  [ 2 5 8 ]]
Run from the calculator stack. This example uses the current calculation settings.

## ColumnSwap

Exchange two specified matrix columns.

You can also enter: CSwp.

Try it
[[1 2][3 4]] 1 2 ColumnSwap

You should get: [[ 2 1 ]
  [ 4 3 ]]

## conj

Return the complex conjugate.

Try it
(3;4) CONJ

You should get: 3-4ⅈ

## ConstantArray

Build an array filled with a supplied constant.

You can also enter: Con.

Try it
{ 2 2 } 7 CON

You should get: [[ 7 7 ]
  [ 7 7 ]]

## cross

Calculate the cross product of two compatible vectors.

You can also enter: ⨯.

Try it
[1 0 0] [0 1 0] CROSS

You should get: [ 0 0 1 ]

## DeleteColumn

Remove the specified column from a matrix.

You can also enter: Column-, Col-.

Try it
[[1 2][3 4]] 1 DeleteColumn

You should get: [[ 2 ][ 4 ]]
[ 1 3 ]
Leaves the reduced matrix below the removed column.

## DeleteRow

Remove the specified row from a matrix.

You can also enter: Row-.

Try it
[[1 2][3 4]] 1 DeleteRow

You should get: [[ 3 4 ]]
[ 1 2 ]
Leaves the reduced matrix below the removed row.

## det

Find a square matrix's determinant.

Try it
[[1 2][3 4]] DET

You should get: -2

## dot

Calculate the dot product of two vectors.

You can also enter: ⋅.

Try it
[1 2 3] [4 5 6] DOT

You should get: 32

## FromArray

Unpack an array into its values and dimensions.

You can also enter: Arry→, Array→.

Try it
[1 2 3] FromArray

You should get: 1
2
3
{ 3 }
Pushes elements followed by the dimensions.

## FromVector

Unpack a vector into components.

You can also enter: Vector→, V→.

Try it
[ 3 4 ] FromVector Depth ToList

You should get: { 3 4 }
Run from the calculator stack. This example uses the current calculation settings.

## IdentityMatrix

Build a square identity matrix of the supplied size.

You can also enter: Idn.

Try it
2 IDN

You should get: [[ 1 0 ]
  [ 0 1 ]]

## im

Return the imaginary part of a complex value.

Try it
(3;4) IM

You should get: 4

## LU

Factor a matrix into lower and upper triangular factors using the engine pivoting convention.

You can also enter: LUFactorization, LUDecomposition.

## MatrixToColumns

Split a matrix into column vectors.

You can also enter: →Columns, →Col.

Try it
[ 1 2 3 ] MatrixToColumns
4 ToList

You should get: { 1 2 3 3 }
Run from the calculator stack. This example uses the current calculation settings.

## MatrixToRows

Split a matrix into row vectors.

You can also enter: →Rows, →Row.

Try it
[ 1 2 3 ] MatrixToRows
4 ToList

You should get: { 1 2 3 3 }
Run from the calculator stack. This example uses the current calculation settings.

## norm

Find the norm of the supplied vector or matrix.

Try it
[3 4] NORM

You should get: 5.

## PolarToReal

Split a polar complex value into magnitude and phase.

You can also enter: Polarℂ→ℝ, P→R.

## RandomMatrix

Build a matrix of random entries with the supplied dimensions.

You can also enter: RanM, RandomArray.

## re

Return the real part of a complex value.

Try it
(3;4) RE

You should get: 3

## RealToPolar

Combine magnitude and phase into a polar complex value.

You can also enter: ℝ→Polarℂ, R→P.

Try it
5 0 RealToPolar

You should get: 5∡0°

## RealToRectangular

Combine two real components into a rectangular complex value.

You can also enter: ℝ→ℂ, RealToComplex, R→C.

Try it
3 4 RealToRectangular

You should get: 3+4ⅈ

## RectangularToReal

Split a rectangular complex value into real components.

You can also enter: ℂ→ℝ, ComplexToReal, C→R.

Try it
(3;4) RectangularToReal

You should get: re:3
im:4
Pushes the real part, then the imaginary part.

## REF

Reduce a matrix to row echelon form.

You can also enter: RowEchelonForm.

## RowNorm

Find the matrix row norm.

You can also enter: RNrm.

## RowsToMatrix

Build a matrix from supplied row vectors.

You can also enter: Rows→, Row→.

Try it
[ 1 2 ]
[ 4 5 ]
[ 7 8 ]
3 RowsToMatrix

You should get: [[ 1 2 ]
  [ 4 5 ]
  [ 7 8 ]]
Run from the calculator stack. This example uses the current calculation settings.

## RowSwap

Exchange two specified matrix rows.

You can also enter: RSwp.

Try it
[[1 2][3 4]] 1 2 RowSwap

You should get: [[ 3 4 ]
  [ 1 2 ]]

## RREF

Reduce a matrix to reduced row echelon form.

You can also enter: RowReducedEchelonForm.

Try it
[[1 2][3 4]] RREF

You should get: [[ 1 0 ]
  [ 0 1 ]]

## RREFMOD

Modulo row reduction is not implemented in this engine build and reports an error. Use REF or RREF for ordinary row reduction.

You can also enter: RowReducedEchelonFormModulo.

## RREFP

Reduce a matrix and also return its pivot values. The reduced matrix is on top of the stack, with the pivot list below it.

You can also enter: RowReducedEchelonFormPivots, RREFPivots.

Try it
[[2 1][3 4]] RREFP 2 ToList

You should get: {
	{ 10 5 }
	[[ 10 0 ]
	  [ 0 5 ]]
}
Run from the calculator stack. This example uses the current calculation settings.

## To2DVector

Build a two-component vector from supplied values.

You can also enter: →2D, →V2.

Try it
3 4 To2DVector

You should get: [ 3 4 ]
Run from the calculator stack. This example uses the current calculation settings.

## To3DVector

Build a three-component vector from supplied values.

You can also enter: →3D, →V3.

Try it
3 4 5 To3DVector

You should get: [ 3 4 5 ]
Run from the calculator stack. This example uses the current calculation settings.

## ToArray

Build an array from the supplied values and dimensions.

You can also enter: →Arry, →Array.

Try it
1 2 3 { 3 } ToArray

You should get: [ 1 2 3 ]

## ToCylindrical

Convert coordinates to cylindrical form.

You can also enter: →Cylindrical.

## ToPolar

Convert a complex value or coordinate object to polar form.

You can also enter: →Polar.

## ToRectangular

Convert a complex value or coordinate object to rectangular form.

You can also enter: →Rectangular, →Cartesian.

Try it
Deg 5 90 RealToPolar ToRectangular

You should get: 5ⅈ
Run from the calculator stack. This example uses the current calculation settings.

## ToSpherical

Convert coordinates to spherical form.

You can also enter: →Spherical.

## TransConjugate

Transpose a matrix and conjugate its entries.

You can also enter: Trn.

## Transpose

Exchange matrix rows and columns.

You can also enter: Tran.

Try it
[[1 2][3 4]] TRN

You should get: [[ 1 3 ]
  [ 2 4 ]]

## ChronoTime

Return calculator time with hundredths of a second in hms form. This reads the clock; it does not connect to NTP.

## Date

Put the calculator's current date on the stack.

## DateAdd

Add the supplied number of days to a date.

You can also enter: Date+.

## DateFromJulianDayNumber

Convert a Julian day number to a date.

You can also enter: JDN→.

## DateSub

Find the difference between supplied dates.

You can also enter: DDays, Date-, DaysBetweenDates.

Try it
20250101 20240101 DDAYS

You should get: 366 d
Run from the calculator stack. This example uses the current calculation settings.

## DateTime

Return the calculator's date and time.

## DegreesToRadians

Convert a plain degree value to radians.

You can also enter: Degrees→Radians, D→R.

Try it
180 DegreesToRadians

You should get: 3.14159 26535 9
A decimal approximation of pi radians.

## DMSAdd

Add two degrees/minutes/seconds values.

You can also enter: DMS+.

## DMSSub

Subtract degrees/minutes/seconds values.

You can also enter: DMS-.

## FromDMS

Convert degrees/minutes/seconds to decimal degrees.

You can also enter: DMS→.

Try it
1.3 FromDMS

You should get: 1 ¹/₂

## FromHMS

Convert hours/minutes/seconds form to decimal hours.

You can also enter: HMS→.

Try it
1.3 FromHMS

You should get: 1 ¹/₂

## HMSAdd

Add two hours/minutes/seconds values.

You can also enter: HMS+.

## HMSSub

Subtract hours/minutes/seconds values.

You can also enter: HMS-.

## JulianDayNumber

Convert a date to its Julian day number.

You can also enter: JDN.

Try it
20000101.120000 JDN

You should get: 2 451 545
Run from the calculator stack. This example uses the current calculation settings.

## RadiansToDegrees

Convert a plain radian value to degrees.

You can also enter: Radians→Degrees, R→D.

Try it
1 RadiansToDegrees

You should get: 57.29577 95131
One radian is approximately 57.2958 degrees.

## SetDate

Set the calculator date from the supplied date value. Settings > Clock / timezone offers the native clock options.

You can also enter: →Date.

## SetTime

Set the calculator time from the supplied time value. Use actual current time for HTTPS rate updates.

You can also enter: →Time.

## Ticks

Return the engine tick counter for elapsed-time measurements.

## Time

Put the calculator's current time on the stack.

## ToDegrees

Express an angle in degrees.

You can also enter: →Degrees, →Deg.

## ToDMS

Convert an angle to degrees/minutes/seconds form.

You can also enter: →DMS.

Try it
1.5 ToDMS

You should get: 1°30′00″

## ToGrads

Express an angle in grads: 400 grads make a full turn.

You can also enter: →Grads, →Grad.

## ToHMS

Convert hours to hours/minutes/seconds form.

You can also enter: →HMS.

Try it
1.5 ToHMS

You should get: 1:30:00

## ToPiRadians

Express an angle as a multiple of pi radians: 2 makes a full turn.

You can also enter: →PiRadians, →πr.

## ToRadians

Express an angle in radians.

You can also enter: →Radians, →Rad.

## acos

Find the inverse cosine of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: cos⁻¹, arccos.

Try it
Deg 1 ACOS

You should get: 0. °

## acosh

Calculate the inverse hyperbolic cosine for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: cosh⁻¹.

Try it
1 ACOSH

You should get: 0.

## acot

Find the inverse cotangent of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: cot⁻¹, arccot.

Try it
Deg 1 ACOT

You should get: 45. °

## acoth

Calculate the inverse hyperbolic cotangent for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: coth⁻¹.

## acsc

Find the inverse cosecant of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: csc⁻¹, arccsc.

Try it
Deg 1 ACSC

You should get: 90. °

## acsch

Calculate the inverse hyperbolic cosecant for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: csch⁻¹.

## asec

Find the inverse secant of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: sec⁻¹, arcsec.

Try it
Deg 1 ASEC

You should get: 0. °

## asech

Calculate the inverse hyperbolic secant for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: sech⁻¹.

## asin

Find the inverse sine of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: sin⁻¹, arcsin.

Try it
Deg 1 ASIN

You should get: 90. °

## asinh

Calculate the inverse hyperbolic sine for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: sinh⁻¹.

Try it
0 ASINH

You should get: 0.

## atan

Find the inverse tangent of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: tan⁻¹, arctan.

Try it
Deg 0 ATAN

You should get: 0. °

## atan2

Find the angle of a two-component direction using the engine two-argument arctangent; the result follows the current angle mode.

You can also enter: ∠.

## atanh

Calculate the inverse hyperbolic tangent for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: tanh⁻¹.

Try it
0 ATANH

You should get: 0.

## BetaI

Calculate the regularized incomplete beta function. Enter a, b, then x; a and b must be positive and x must be between 0 and 1.

Try it
1 1 0 BetaI

You should get: 0

## comb

Count unordered selections. Supply population size n followed by selection size r.

You can also enter: Combinations.

Try it
5 2 COMB

You should get: 10

## cos

Find the cosine of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

Try it
Deg 0 COS

You should get: 1

## cosh

Calculate the hyperbolic cosine for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

Try it
0 COSH

You should get: 1.

## cot

Find the cotangent of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

Try it
Deg 45 COT

You should get: 1.

## coth

Calculate the hyperbolic cotangent for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

## csc

Find the cosecant of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

Try it
Deg 90 CSC

You should get: 1.

## csch

Calculate the hyperbolic cosecant for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

## erf

Calculate the error function for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted. Complex arguments are not implemented for this function.

Try it
0 ERF

You should get: 0.

## erfc

Calculate the complementary error function for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted. Complex arguments are not implemented for this function.

Try it
0 ERFC

You should get: 1.

## exp

Calculate the exponential e^x for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

Try it
0 EXP

You should get: 1.

## exp10

Calculate the exponential 10^x for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: alog.

Try it
2 ALOG

You should get: 100.

## exp2

Calculate the exponential 2^x for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: alog2.

Try it
3 EXP2

You should get: 8.

## expm1

Calculate the value of e^x-1 for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: expm.

Try it
0 EXPM

You should get: 0.

## fact

Find the factorial. Noninteger arguments follow the engine's gamma-function extension where supported. On the calculator stack, press ! with an empty editor to apply factorial immediately. Typing 6! also calculates immediately, without Enter. Inside programs and quoted algebra, ! remains notation.

You can also enter: x!, factorial, !.

Try it
5 FACT

You should get: 120

## GammaP

Calculate the regularized lower incomplete gamma function. Enter a followed by x, with a>0 and x>=0.

Try it
1 0 GammaP

You should get: 0

## lgamma

Calculate the logarithm of the gamma function for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted. Complex arguments are not implemented for this function.

You can also enter: lnΓ, LnGamma.

Try it
1 LGAMMA

You should get: 0.

## ln

Calculate the natural logarithm for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

Try it
1 LN

You should get: 0.

## ln1p

Calculate the natural logarithm of 1+x for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: lnp1.

Try it
0 LNP1

You should get: 0.

## log10

Calculate the base-ten logarithm for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: log.

Try it
100 LOG

You should get: 2.

## log2

Calculate the base-two logarithm for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

Try it
8 LOG2

You should get: 3.

## perm

Count ordered selections. Supply population size n followed by selection size r.

You can also enter: Permutations.

Try it
5 2 PERM

You should get: 20

## sec

Find the secant of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

Try it
Deg 0 SEC

You should get: 1.

## sech

Calculate the hyperbolic secant for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

Try it
0 SECH

You should get: 1.

## sin

Find the sine of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

Try it
Deg 90 SIN

You should get: 1

## sinh

Calculate the hyperbolic sine for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

Try it
0 SINH

You should get: 0.

## tan

Find the tangent of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

Try it
Deg 0 TAN

You should get: 0

## tanh

Calculate the hyperbolic tangent for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

Try it
0 TANH

You should get: 0.

## Termial

Sum the integers from 1 through n. Accepts nonnegative whole numbers, including zero. Uses n*(n+1)/2 and returns an exact integer; negative or fractional inputs report a domain error. Enter 6? or 6 Termial to get 21. With no entry being edited, press ? to apply Termial immediately to the top stack value. Typing 6? also calculates immediately without Enter; inside programs and quoted algebra, ? remains notation. In algebraic expressions, N? is postfix Termial(N). In RPL, use N ? or N Termial after a variable; question marks in ordinary RPL identifiers remain valid. Explicit names such as ⓋREADY? and the constant Ⓒ? keep their existing meanings.

You can also enter: ?.

Try it
6?

You should get: 21
Sum 1+2+3+4+5+6.

## tgamma

Calculate the gamma function for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted. Complex arguments are not implemented for this function.

You can also enter: Γ, Gamma.

Try it
5 TGAMMA

You should get: 24.

## UTPC

Find the upper-tail chi-square probability. Enter degrees of freedom followed by the statistic x. Degrees of freedom must be positive.

Try it
2 0 UTPC

You should get: 1

## UTPF

Find the upper-tail F probability. Enter numerator degrees of freedom, denominator degrees of freedom, then x. Both degree counts must be positive.

Try it
2 2 0 UTPF

You should get: 1

## UTPN

Find the upper-tail normal probability. Enter mean, variance, then x. The variance must be positive; it is variance, not standard deviation.

Try it
0 1 0 UTPN

You should get: 0.5

## UTPT

Find the upper-tail Student t probability. Enter positive degrees of freedom followed by t.

Try it
1 0 UTPT

You should get: 0.5

## Bar

Select engine bar-chart plotting.

You can also enter: BarPlot.

## BlankBitmap

Create a blank monochrome bitmap with supplied dimensions.

## BlankGraphic

Create a blank graphics object with supplied dimensions.

You can also enter: Blank.

## BlankGrob

Create a blank compatibility graphics object with supplied dimensions.

## BlankPixmap

Create a blank colour pixmap with supplied dimensions.

## Center

Set the engine plot centre.

You can also enter: PlotCenter, Centr.

## Circle

Draw a circle using the supplied centre and radius in the engine graphics view.

You can also enter: DrawCircle.

## Clip

Set the engine graphics clipping region from the supplied object.

## ClLCD

Clear the engine graphics display.

You can also enter: ClearLCD.

## Color

Convert the supplied colour specification to an engine drawing pattern.

You can also enter: ColorPattern.

## CurrentClip

Return the current engine graphics clipping region.

## Depnd

Choose the engine plot's dependent variable.

You can also enter: DependentVariable, DEPND.

## Disp

Display supplied text or an object at the requested engine display line.

You can also enter: DrawText.

## DispXY

Draw supplied text or an object at the requested graphics position.

You can also enter: DrawStyledText.

## Draw

Draw the current engine equation using its plot parameters. For LumaRPL's interactive graph tools, press Ctrl+G.

You can also enter: DrawPlot.

## Drax

Draw axes in the engine graphics view using its plot parameters.

You can also enter: DrawAxes.

## Ellipse

Draw an ellipse using the supplied bounds in the engine graphics view.

You can also enter: DrawEllipse.

## Freeze

Set the engine display-region freeze mask. This is a drawing control, not a native graph pause button.

## FromLCD

Capture calculator display pixels as a graphics object.

You can also enter: LCD→, Screen→.

## Function

Select function plotting in the engine plot parameters.

You can also enter: FunctionPlot.

## GAnd

Combine graphics objects with AND.

You can also enter: GraphicAnd.

## GOr

Combine graphics objects with OR.

You can also enter: GraphicOr.

## GraphicAppend

Join graphics objects side by side.

## GraphicExponent

Compose a graphics object with an exponent.

## GraphicIntegral

Compose graphics objects using integral notation.

## GraphicNorm

Surround a graphics object with norm bars.

## GraphicParentheses

Surround a graphics object with parentheses.

## GraphicProduct

Compose graphics objects using product notation.

## GraphicRatio

Compose graphics objects as a fraction.

## GraphicRoot

Compose a graphics object with a root sign.

## GraphicStack

Place graphics objects above one another.

You can also enter: GrobAdd.

## GraphicSubscript

Compose a graphics object with a subscript.

## GraphicSum

Compose graphics objects using summation notation.

## Gray

Build a greyscale drawing pattern from the supplied intensity.

You can also enter: GrayPattern, GreyPattern, Grey.

## GXor

Combine graphics objects with exclusive OR.

You can also enter: GraphicXor.

## Histogram

Select engine histogram plotting.

You can also enter: HistogramPlot.

## HSV

Build a drawing pattern from hue, saturation and value.

You can also enter: HSVPattern.

## Indep

Choose the engine plot's independent variable.

You can also enter: IndependentVariable, INDEP.

## Line

Draw between two supplied positions in the engine graphics view.

You can also enter: DrawLine.

## Parametric

Select parametric plotting in the engine plot parameters.

You can also enter: ParametricPlot.

## Pict

Recall the engine picture graphics object.

You can also enter: GraphicPicture.

## PixColor

Return the colour at the supplied graphics position.

You can also enter: PixelColor, PixC?.

## PixOff

Clear the pixel at the supplied graphics position using the background pattern.

You can also enter: DrawBackgroundPixel.

## PixOn

Set the pixel at the supplied graphics position using the foreground pattern.

You can also enter: DrawPixel.

## PixTest

Test the pixel at the supplied graphics position.

You can also enter: PixelGrayLevel, Pix?.

## PlotMax

Set the engine plot's maximum corner.

You can also enter: PMax.

## PlotMin

Set the engine plot's minimum corner.

You can also enter: PMin.

## PlotParameters

Read or set the engine plot parameters using the supplied plot-parameter object.

You can also enter: PPar.

## Polar

Select polar plotting in the engine plot parameters.

You can also enter: PolarPlot.

## Rect

Draw a rectangle between supplied corners.

You can also enter: DrawRectangle.

## Res

Set the engine plot resolution.

You can also enter: Resolution.

## RGB

Build a drawing pattern from red, green and blue components. Foreground and Background accept drawing patterns; themes are selected under Settings > Display.

You can also enter: RGBPattern.

## RRect

Draw a rounded rectangle using supplied bounds and corner radius.

You can also enter: DrawRoundedRectangle.

## Scale

Set both engine plot-axis scales.

You can also enter: PlotScale.

## Scatter

Select engine scatter plotting from engine statistical data.

You can also enter: ScatterPlot.

## Show

Show an enlarged rendering of the supplied calculator object. This is separate from plotting a native saved graph.

## ToBitmap

Convert to a packed monochrome bitmap.

You can also enter: →Bitmap.

## ToGrob

Render the supplied object into a graphics object.

You can also enter: →Grob.

## ToHPGrob

Convert to an HP-compatible graphics object.

You can also enter: →HPGrob.

## ToLCD

Copy a graphics object to the calculator graphics display.

You can also enter: →LCD, →Screen.

## ToPixmap

Convert to a colour pixmap.

You can also enter: →Pixmap.

## Truth

Select truth plotting in the engine plot parameters.

You can also enter: TruthPlot.

## XRange

Set the engine horizontal plot interval.

You can also enter: PlotXRange, XRng.

## XScale

Set the engine horizontal plot scale.

You can also enter: PlotScaleX, ScaleX, ScaleH.

## YRange

Set the engine vertical plot interval.

You can also enter: PlotYRange, YRng.

## YScale

Set the engine vertical plot scale.

You can also enter: PlotScaleY, ScaleY, ScaleW.

## CharToUnicode

Return the Unicode code point for a supplied character.

You can also enter: Char→Code, Codepoint, Num.

Try it
"A" CharToUnicode

You should get: 65

## DoList

Apply an operation across one or more lists. Supply the lists, their count and the operation using the engine list convention.

## DoSubs

Apply an operation to list subsequences. NSub and EndSub provide local indices while it runs.

## Explode

Break an object into its components.

You can also enter: Object→, FromObj, Obj→.

## Filter

Keep list elements that pass the supplied test.

Try it
{ 1 2 3 4 } « 2 > » FILTER

You should get: { 3 4 }

## FromList

Unpack a list into its elements and return its size using the engine list convention.

You can also enter: List→.

Try it
{ 1 2 3 } FromList

You should get: 1
2
3
3
The values 1, 2, 3 are pushed, followed by their count, 3.

## Get

Read an element from the supplied object at the supplied position.

Try it
{ 10 20 30 } 2 GET

You should get: 20

## GetI

Read the indexed element and advance the index using the engine indexed-access convention.

You can also enter: GetIteration.

## Head

Return the first element of a list.

Try it
{ 10 20 30 } HEAD

You should get: 10

## ListDifferences

Return differences between successive list elements.

You can also enter: ∆List.

Try it
{ 3 7 12 } ListDifferences

You should get: { 4 5 }

## ListProduct

Multiply the elements of a list.

You can also enter: ∏List.

Try it
{ 2 3 4 } ListProduct

You should get: 24

## ListSum

Add the elements of a list.

You can also enter: ΣList.

Try it
{ 1 2 3 } ListSum

You should get: 6

## Map

Apply the supplied operation to list elements and collect the results.

Try it
{ 1 2 3 } « SQ » MAP

You should get: { 1 4 9 }

## Put

Replace an element in the supplied object at the supplied position.

Try it
{ 10 20 30 } 2 99 PUT

You should get: { 10 99 30 }

## PutI

Write the indexed element and advance the index using the engine indexed-access convention.

You can also enter: PutIteration.

## QuickSort

Sort a list in ascending order with the engine quick-sort operation.

You can also enter: QSort.

Try it
{ 3 1 2 } QuickSort

You should get: { 1 2 3 }

## QuickUnique

Remove duplicate elements using the engine quick unique operation.

Try it
{ 1 1 2 2 } QuickUnique

You should get: { 1 2 }

## Reduce

Combine list elements using the supplied operation.

You can also enter: Stream.

Try it
{ 1 2 3 } « + » REDUCE

You should get: 6

## ReverseList

Reverse the order of list elements.

You can also enter: RevList.

Try it
{ 1 2 3 } REVLIST

You should get: { 3 2 1 }

## ReverseQuickSort

Sort a list in descending order with quick sort.

Try it
{ 3 1 2 } ReverseQuickSort

You should get: { 3 2 1 }

## ReverseSort

Sort list elements in descending order.

Try it
{ 3 1 2 } ReverseSort

You should get: { 3 2 1 }

## Size

Return an object's size, such as a list length or matrix dimensions.

Try it
{ 10 20 30 } SIZE

You should get: 3

## Sort

Sort list elements in ascending order.

Try it
{ 3 1 2 } SORT

You should get: { 1 2 3 }

## Tail

Return the list after removing its first element.

Try it
{ 10 20 30 } TAIL

You should get: { 20 30 }

## TextToUnicode

Return the Unicode code points in supplied text.

You can also enter: Text→Code.

Try it
"AB" TextToUnicode

You should get: { 65 66 }

## ToList

Collect the requested number of stack values into a list. Supply the count last.

You can also enter: →List.

Try it
1 2 3 3 ToList

You should get: { 1 2 3 }

## ToText

Convert the top object to its source-text representation.

You can also enter: →Text, ToStr, →Str.

Try it
42 ToText

You should get: "42"

## UnicodeToText

Build text from supplied Unicode code points.

You can also enter: Code→Text, Code→Char, Chr.

Try it
65 UnicodeToText

You should get: "A"

## Unique

Remove duplicate list elements.

Try it
{ 1 1 2 2 } UNIQUE

You should get: { 1 2 }

## And

Combine two values with AND. Integer inputs follow the truth/bitwise logic setting.

You can also enter: and.

Try it
True False AND

You should get: False

## BinaryToFlags

Restore engine flag state from a supplied based integer. This can change calculator behavior.

You can also enter: StoF.

## ClearFlag

Clear the numbered user flag supplied on the stack.

You can also enter: CF.

## Compile

Parse source text into calculator objects. It does not run a Lua or CHIP-8 file.

You can also enter: Text→, Str→.

Try it
"2 3 +" Compile EVAL

You should get: 5

## CompileToAlgebraic

Parse source text and require an algebraic object.

You can also enter: Text→Algebraic.

## CompileToExpression

Parse source text and require an expression.

You can also enter: Text→Expression.

## CompileToInteger

Parse source text and require an integer.

You can also enter: Text→Integer.

Try it
"42" CompileToInteger

You should get: 42

## CompileToNumber

Parse source text and require a number.

You can also enter: Text→Number.

Try it
"42" CompileToNumber

You should get: 42

## CompileToObject

Parse source text as one calculator object.

You can also enter: Text→Object.

Try it
"{ 1 2 }" CompileToObject

You should get: { 1 2 }

## CompileToPositive

Parse source text and require a positive value.

You can also enter: Text→Positive.

Try it
"42" CompileToPositive

You should get: 42

## CompileToReal

Parse source text and require a real value.

You can also enter: Text→Real.

Try it
"1.25" CompileToReal

You should get: 1.25

## Continue

Resume a paused RPL program.

You can also enter: cont.

## Debug

Start the supplied RPL program in the debugger.

You can also enter: dbug.

## EndSub

Return the final subsequence index while DoSubs is running.

## False

Put a false Boolean value on the stack.

Try it
False

You should get: False

## FlagsToBinary

Pack the engine flag state into a based integer for storage or inspection.

You can also enter: RclF.

## FlipFlag

Toggle the numbered user flag supplied on the stack.

You can also enter: FF.

## Halt

Pause RPL execution so you can inspect or debug it.

## IFT

Run the supplied branch only when the supplied condition is true.

You can also enter: IfThen.

Try it
True « 7 » IFT

You should get: 7

## IFTE

Choose between two branches from a condition. Supply the condition, true branch and false branch in that order.

You can also enter: IfThenElse.

Try it
False « 7 » « 9 » IFTE

You should get: 9

## Input

Ask for calculator input using the supplied prompt specification.

## Kill

Stop paused RPL execution.

## MultipleSteps

Execute the supplied number of debugger steps.

## Not

Invert a truth value or bits, according to the integer logic setting.

You can also enter: not.

Try it
True NOT

You should get: False

## NSub

Return the current subsequence index while DoSubs is running. Outside that operation there is no such local index.

## Or

Combine two values with OR. Integer inputs follow the truth/bitwise logic setting.

You can also enter: or.

Try it
True False OR

You should get: True

## Prompt

Display the supplied prompt during RPL execution.

## Quote

Keep the supplied expression unevaluated in the applicable symbolic context.

## Run

Run the supplied program or named object. In Programming browsers, use the labelled F2 Run action.

## same

Compare two objects for identical content and type, rather than mathematical equivalence.

Try it
3 3 SAME

You should get: True

## SetFlag

Set the numbered user flag supplied on the stack.

You can also enter: SF.

## SingleStep

Execute the next instruction of a paused RPL program.

You can also enter: sst.

## StepOut

Continue until the current nested program returns.

## StepOver

Run the next instruction without stepping into a called program.

## TestEQ

Test whether the top two values are equal.

You can also enter: =.

Try it
3 3 TestEQ

You should get: True

## TestFlagClear

Test whether the supplied user flag is clear.

You can also enter: FC?.

## TestFlagClearThenClear

Test whether a user flag was clear, then leave it clear.

You can also enter: FC?C.

## TestFlagClearThenSet

Test whether a user flag was clear, then set it.

You can also enter: FC?S.

## TestFlagSet

Test whether the supplied user flag is set.

You can also enter: FS?.

## TestFlagSetThenClear

Test whether a user flag was set, then clear it.

You can also enter: FS?C.

## TestFlagSetThenSet

Test whether a user flag was set, then leave it set.

You can also enter: FS?S.

## TestGE

Test whether the second stack value is greater than or equal to the top value.

You can also enter: ≥.

## TestGT

Test whether the second stack value is greater than the top value.

You can also enter: >.

Try it
3 2 >

You should get: True

## TestLE

Test whether the second stack value is less than or equal to the top value.

You can also enter: ≤.

## TestLT

Test whether the second stack value is less than the top value.

You can also enter: <.

Try it
2 3 <

You should get: True

## TestNE

Test whether the top two values differ.

You can also enter: ≠.

Try it
2 3 TestNE

You should get: True

## TestSame

Test whether the supplied objects are the same, using the engine comparison operation.

You can also enter: ==.

Try it
3 3 TestSame

You should get: True

## TimedEval

Evaluate an object and report its elapsed time.

You can also enter: TEval.

## ToInteger

Convert a supported object to an integer.

You can also enter: R→I, →Integer, →Int.

## ToProgram

Turn the supplied object into an RPL program.

You can also enter: →Program, →Prg.

Try it
{ 2 3 + } ToProgram EVAL

You should get: 5

## True

Put a true Boolean value on the stack.

Try it
True

You should get: True

## Wait

Wait for the supplied duration or a key, according to the engine WAIT convention. Returned key positions are engine keys, not QWERTY character codes.

## Where

Apply the supplied local assignments to an expression.

You can also enter: |.

## Xor

Combine two values with exclusive OR. Integer inputs follow the truth/bitwise logic setting.

You can also enter: xor.

Try it
True True XOR

You should get: False

## abs

Find the absolute value, or the magnitude of a complex value.

You can also enter: Modulus.

Try it
-7 ABS

You should get: 7

## add

Add the top two values. Type 2 3 + and press Enter to get 5.

You can also enter: +.

Try it
2 3 +

You should get: 5

## BinaryToReal

Convert a based integer to a real-number object.

You can also enter: B→R.

Try it
#FFh B→R

You should get: 255

## cbrt

Find the cube root of the top value.

You can also enter: ∛, CubeRoot.

Try it
27 CBRT

You should get: 3.

## ceil

Round up to the next integer.

Try it
3.2 CEIL

You should get: 4.

## cubed

Cube the top value. Example: 3 cubed gives 27.

You can also enter: x³.

Try it
3 CUBED

You should get: 27

## DFC

Convert a decimal value to a continued-fraction representation.

You can also enter: ContinuedFraction.

## DFC2F

Convert a continued-fraction representation to a fraction.

You can also enter: ContinuedFractionToFraction.

## Div2

Perform integer division and return the quotient and remainder.

You can also enter: QuoRem, IDiv2, QuotientRemainder.

## divide

Divide the second value by the top value.

Try it
1 3 /

You should get: ¹/₃

## EulerianNumber

Put Euler's number e on the stack. Symbolic/numerical constant settings affect its form.

You can also enter: ℮.

## Extract

Extract the requested part of an object using the supplied selection information.

You can also enter: Sub.

## Factors

Factor an integer, subject to the configured factorization limits.

Try it
60 FACTORS

You should get: { 2 2 3 1 5 1 }
The list alternates prime and exponent: 2^2 * 3^1 * 5^1 = 60.

## floor

Round down to the previous integer.

Try it
3.8 FLOOR

You should get: 3.

## FracPart

Keep the fractional remainder after truncation toward zero.

You can also enter: FractionalPart, FP.

Try it
3.75 FP

You should get: 0.75

## GCD

Find the greatest common divisor of two integers.

You can also enter: GreatestCommonDenominator.

Try it
48 18 GCD

You should get: 6

## hypot

Find sqrt(x^2+y^2) from two supplied values.

You can also enter: ⊿, Hypothenuse.

Try it
3 4 HYPOT

You should get: 5.

## Infinity

Put positive infinity on the stack.

You can also enter: ∞.

Try it
infinity

You should get: ∞
Run from the calculator stack. This example uses the current calculation settings.

## IntPart

Keep the integer part, truncating toward zero.

You can also enter: IntegerPart, IP.

Try it
3.75 IP

You should get: 3.

## inv

Find the reciprocal of the top value: 1 divided by that value. Zero has no finite reciprocal.

You can also enter: x⁻¹, Invert.

Try it
4 INV

You should get: ¹/₄

## IsPrime

Test whether the supplied integer is prime.

Try it
17 ISPRIME

You should get: True

## LCM

Find the least common multiple of two integers.

You can also enter: LeastCommonMultiple.

Try it
6 8 LCM

You should get: 24

## mant

Return the decimal mantissa of a number.

You can also enter: Mantissa.

Try it
1234 MANT

You should get: 1.234

## Max

Keep the larger of the top two values.

Try it
3 8 MAX

You should get: 8

## Min

Keep the smaller of the top two values.

Try it
3 8 MIN

You should get: 3

## mod

Find the mathematical modulo of two numbers, in dividend then divisor order. Negative inputs can differ from REM.

Try it
17 5 MOD

You should get: 2

## multiply

Multiply the top two values.

Try it
6 7 *

You should get: 42

## neg

Change the sign of the top value.

You can also enter: Negate.

Try it
7 NEG

You should get: -7

## NegativeInfinity

Put negative infinity on the stack.

You can also enter: −∞.

Try it
NegativeInfinity

You should get: −∞
Run from the calculator stack. This example uses the current calculation settings.

## NextPrime

Find the next prime after the supplied integer.

You can also enter: NextPr.

Try it
17 NEXTPRIME

You should get: 19

## Percent

Calculate a percentage of the value below it. The base amount goes first, then the percentage.

You can also enter: %.

Try it
200 15 %

You should get: 30

## PercentChange

Calculate percentage change from the first value to the second.

You can also enter: %Change, %Ch.

Try it
100 125 %CH

You should get: 25

## PercentTotal

Calculate the second amount as a percentage of the first total.

You can also enter: %Total, %T.

Try it
200 50 %T

You should get: 25

## Pi

Put the mathematical constant pi on the stack. Symbolic/numerical constant settings affect its form.

You can also enter: π.

## pow

Raise the second value to the power on top. Example: 2 3 ^ gives 8. On the calculator stack, a complete numeric entry such as 2^3 also gives 8 on Enter. 0^0 gives 1 by default; the advanced ZeroPowerZeroIsUndefined setting explicitly selects undefined behavior.

You can also enter: ↑, Power, ^.

Try it
2 8 ^

You should get: 256

## PreviousPrime

Find the preceding prime before the supplied integer.

You can also enter: PrevPr.

Try it
17 PreviousPrime

You should get: 13

## Random

Generate a random value between two supplied bounds. Put the lower bound first and the upper bound on top.

## RandomNumber

Generate a random value from 0 up to, but not including, 1.

You can also enter: rand.

## RandomSeed

Set the random-number generator seed. Reusing a seed is useful when reproducing a calculation.

You can also enter: rdz.

## RealToBinary

Convert a real integer to a based-number object. Base and WordSize control its representation and fixed-width operations.

You can also enter: R→B.

Try it
255 R→B

You should get: #FF₁₆

## rem

Find the division remainder, in dividend then divisor order. Example: 5 2 rem gives 1.

Try it
17 5 REM

You should get: 2

## Round

Round a value using the supplied digit count. The sign of the digit count selects the engine's rounding convention.

You can also enter: Rnd.

Try it
1.234 2 RND

You should get: 1.23

## SigDig

Count the significant decimal digits of a value.

You can also enter: SignificantDigits.

## sign

Return the sign of a real value, or its normalized direction when supported for complex values.

Try it
-5 SIGN

You should get: -1

## sq

Square the top value.

Try it
12 SQ

You should get: 144

## sqrt

Find the square root. The real/complex result setting determines whether negative real inputs can give complex results.

You can also enter: √, SquareRoot.

Try it
81 SQRT

You should get: 9.

## subtract

Subtract the top value from the value below it.

Try it
10 3 -

You should get: 7

## ToDecimal

Convert the top value to a decimal approximation using the current precision.

You can also enter: →Decimal, →Number, →Num.

Try it
1 4 / ToDecimal

You should get: 0.25

## ToFraction

Convert a value to a fraction using the fraction-conversion settings.

You can also enter: →ℚ, →Frac, →Q.

Try it
0.5 ToFraction

You should get: ¹/₂

## ToQuotient

Rewrite a value or expression as a quotient where supported.

You can also enter: →ℚπ, →QPi, →Qπ, XQ.

Try it
0.5 ToQuotient

You should get: ¹/₂

## Truncate

Discard digits using the supplied digit count instead of rounding them.

You can also enter: Trnc.

Try it
1.239 2 TRNC

You should get: 1.23

## xpon

Return the decimal exponent of a number.

You can also enter: Exponent.

Try it
1234 XPON

You should get: 3

## xroot

Find a root of the supplied value using the supplied root order.

Try it
27 3 XROOT

You should get: 3.

## ClearStack

Remove every value from the stack. Save anything important before using this command.

You can also enter: Clear.

Try it
1 2 3 CLEAR DEPTH ToList

You should get: { }
The list shows the remaining stack from deepest value to top.

## Clone

Make an independent copy of the top object.

You can also enter: NewObject, NewObj, NewOb.

## Copy

Copy a named object between calculator directories using the supplied source and destination names.

You can also enter: ▶.

## CrDir

Create a calculator variable directory with the supplied name.

You can also enter: CreateDirectory, MkDir.

## CurrentDirectory

Return the current calculator directory as an object. This is calculator variable storage, not an SD folder.

## Decrement

Decrease the supplied value or named variable by one.

You can also enter: Decr.

## Depth

Put the number of stack values on top of the stack.

Try it
10 20 DEPTH

You should get: 10
20
2

## Drop

Remove the top value from the stack.

Try it
10 20 DROP DEPTH ToList

You should get: { 10 }
The list shows the remaining stack from deepest value to top.

## Drop2

Remove the top two values.

Try it
10 20 30 DROP2 DEPTH ToList

You should get: { 10 }
The list shows the remaining stack from deepest value to top.

## DropN

Remove several values. Put the count on top of the values you want to remove; the count is removed too.

Try it
10 20 30 2 DROPN DEPTH ToList

You should get: { 10 }
The list shows the remaining stack from deepest value to top.

## Dup

Make a second copy of the top value. Example: 7 DUP leaves 7 and 7.

You can also enter: Duplicate.

Try it
7 DUP DEPTH ToList

You should get: { 7 7 }
The list shows the remaining stack from deepest value to top.

## Dup2

Copy the top pair of values, keeping their order.

You can also enter: Duplicate2.

Try it
2 3 DUP2 DEPTH ToList

You should get: { 2 3 2 3 }
The list shows the remaining stack from deepest value to top.

## DupDup

Duplicate the top value twice, leaving three copies.

You can also enter: DuplicateTwice.

Try it
7 DUPDUP DEPTH ToList

You should get: { 7 7 7 }
The list shows the remaining stack from deepest value to top.

## DupN

Copy a group of values. Put the size of the group on top first.

You can also enter: DuplicateN.

Try it
2 3 2 DUPN DEPTH ToList

You should get: { 2 3 2 3 }
The list shows the remaining stack from deepest value to top.

## Eval

Evaluate the top object. For a program, this runs its instructions.

Try it
« 2 3 + » EVAL

You should get: 5

## Home

Return to the HOME calculator directory.

You can also enter: HomeDirectory.

## Increment

Increase the supplied value or named variable by one.

You can also enter: Incr.

## LastArg

Recall the arguments saved from the previous command. Argument-saving settings determine what is available.

You can also enter: LastArguments.

## LastX

Recall the last saved first argument.

## LName

Find names used in the supplied object.

You can also enter: ListExpressionNames.

## NDupN

Repeat a group of stack values using the supplied count.

You can also enter: NDuplicateN.

## Nip

Remove the second stack value and keep the top one.

Try it
1 2 NIP DEPTH ToList

You should get: { 2 }
The list shows the remaining stack from deepest value to top.

## Order

Reorder the current directory using a list of variable names.

## Over

Copy the second value to the top without removing either original value.

Try it
2 3 OVER DEPTH ToList

You should get: { 2 3 2 }
The list shows the remaining stack from deepest value to top.

## Path

Return the path through calculator variable directories to the current directory.

You can also enter: DirectoryPath.

## PgDir

Purge the named calculator directory and its contents.

You can also enter: PurgeDirectory, RmDir.

## Pick

Copy a chosen stack level. Put the level number on top first; level 1 is the value immediately below it.

Try it
10 20 30 3 PICK DEPTH ToList

You should get: { 10 20 30 10 }
The list shows the remaining stack from deepest value to top.

## Pick3

Copy the third stack value to the top.

Try it
1 2 3 PICK3 DEPTH ToList

You should get: { 1 2 3 1 }
The list shows the remaining stack from deepest value to top.

## Purge

Remove the variable named on the stack. Use a quoted name to avoid evaluating it first.

Try it
42 'DEMO' STO 'DEMO' PURGE 7

You should get: 7
Removes the temporary DEMO variable, then leaves 7.

## Rcl

Fetch a stored object without running it: type 'ANSWER' RCL. A recalled program stays on the stack until you run EVAL.

You can also enter: Recall.

Try it
42 'DEMO' STO 'DEMO' RCL

You should get: 42
Stores a HOME variable named DEMO; choose another name if it already exists.

## RecallAdd

Recall the named value and combine it with a stack value using add arithmetic, without storing the result back.

You can also enter: Recall+, Rcl+.

## RecallDiv

Recall the named value and combine it with a stack value using divide arithmetic, without storing the result back.

You can also enter: Recall÷, Recall/, Rcl/, Rcl÷.

## RecallMul

Recall the named value and combine it with a stack value using multiply arithmetic, without storing the result back.

You can also enter: Recall×, Recall*, Rcl*, Rcl×.

## RecallSub

Recall the named value and combine it with a stack value using subtract arithmetic, without storing the result back.

You can also enter: Recall-, Rcl-.

## Roll

Bring a chosen stack level to the top, moving the intervening values down. Supply the level number first.

Try it
1 2 3 3 ROLL DEPTH ToList

You should get: { 2 3 1 }
The list shows the remaining stack from deepest value to top.

## RollD

Move the top value down to a chosen level. Supply the destination level number first.

You can also enter: RollDown.

Try it
1 2 3 3 ROLLD DEPTH ToList

You should get: { 3 1 2 }
The list shows the remaining stack from deepest value to top.

## Rot

Rotate the top three values, bringing the deepest of the three to the top.

Try it
1 2 3 ROT DEPTH ToList

You should get: { 2 3 1 }
The list shows the remaining stack from deepest value to top.

## Sto

Store a value under a name: type 42 'ANSWER' STO and press Enter. To keep HOME variables after restart, save the calculator session.

You can also enter: Store.

Try it
42 'DEMO' STO 'DEMO' RCL

You should get: 42
Stores a HOME variable named DEMO; choose another name if it already exists.

## StoreAdd

Update the named variable using add arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store+, Sto+.

Try it
10 'DEMO' STO 3 'DEMO' STO+ DEMO

You should get: 13

## StoreDiv

Update the named variable using divide arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store÷, Store/, Sto/, Sto÷.

Try it
10 'DEMO' STO 2 'DEMO' STO/ DEMO

You should get: ¹/₅
STO/ uses the supplied value divided by the stored value: 2 divided by 10 gives 1/5.

## StoreMul

Update the named variable using multiply arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store×, Store*, Sto*, Sto×.

Try it
10 'DEMO' STO 3 'DEMO' STO* DEMO

You should get: 30

## StoreSub

Update the named variable using subtract arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store-, Sto-.

Try it
10 'DEMO' STO 3 'DEMO' STO- DEMO

You should get: -7
STO- uses the supplied value minus the stored value: 3 minus 10 gives -7.

## Swap

Exchange the top two values. Example: 2 3 SWAP leaves 3 below 2.

Try it
2 3 SWAP DEPTH ToList

You should get: { 3 2 }
The list shows the remaining stack from deepest value to top.

## TVars

List variables of the requested object type in the current directory.

You can also enter: TypedVariables.

## Undo

Restore the saved calculator stack from before the last operation, when stack saving is enabled.

## UnPick

Replace a chosen stack level with the supplied value. Check the level before replacing it.

## UnRot

Rotate the top three values in the opposite direction to ROT.

Try it
1 2 3 UNROT DEPTH ToList

You should get: { 3 1 2 }
The list shows the remaining stack from deepest value to top.

## UpDir

Move to the parent calculator directory.

You can also enter: UpDirectory.

## Vars

List names in the current calculator directory.

You can also enter: Variables.

## XVars

List variables used by the supplied expression.

You can also enter: ExpressionVariables.

## AddData

Add supplied observations to the engine statistical data.

You can also enter: Σ+.

## Average

Calculate the mean using the engine statistical data or supplied supported object.

You can also enter: ΣMean, Avg, Mean.

Try it
[[1 2][2 4][3 6]] StoreData Average

You should get: [ 2 4 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## BestFit

Choose the best engine regression fit among its supported models.

## ClearData

Clear the engine statistical data.

You can also enter: ClearΣ, ClΣ.

Try it
[[1 2][2 4][3 6]] StoreData ClearData DataSize

You should get: 0
Clears calculator statistics data. Save existing data first.

## Correlation

Return the correlation coefficient for the engine selected data columns.

You can also enter: Corr.

Try it
[[1 2][2 4][3 6]] StoreData Correlation

You should get: 1.
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## Covariance

Return sample covariance for the engine selected data columns.

You can also enter: Cov.

Try it
[[1 2][2 4][3 6]] StoreData Covariance

You should get: 2
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## DataColumns

Configure the statistical columns used by engine analysis.

You can also enter: ΣColumns, ColΣ.

## DataSize

Return the engine statistical-data size.

You can also enter: ΣSize, NΣ.

Try it
[[1 2][2 4][3 6]] StoreData DataSize

You should get: 3
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## DataTotal

Return totals from the engine statistical data.

You can also enter: ΣTotal, Tot.

## DependentColumn

Choose the dependent statistical-data column.

You can also enter: YCol.

## ExponentialFit

Select the engine exponential regression model.

You can also enter: ExpFit.

## FrequencyBins

Count observations in the supplied frequency bins.

You can also enter: Bins.

## IndependentColumn

Choose the independent statistical-data column.

You can also enter: XCol.

## Intercept

Recall the intercept last computed by LinearRegression. Run the regression after changing data.

Try it
[[1 2][2 4][3 6]] StoreData LinearRegression DROP2 Intercept

You should get: 0
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## LinearFit

Select the engine linear regression model.

You can also enter: LinFit.

## LinearRegression

Fit a linear regression using engine statistical data.

You can also enter: LR.

## LogarithmicFit

Select the engine logarithmic regression model.

You can also enter: LogFit.

## MaxData

Return column maxima from engine statistical data.

You can also enter: ΣMax, MaxΣ.

Try it
[[1 2][2 4][3 6]] StoreData MaxData

You should get: [ 3 6 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## Median

Calculate the median of the supplied data.

Try it
[[1 2][2 4][3 6]] StoreData Median

You should get: [ 2 4 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## MinData

Return column minima from engine statistical data.

You can also enter: ΣMin, MinΣ.

Try it
[[1 2][2 4][3 6]] StoreData MinData

You should get: [ 1 2 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## PopulationCovariance

Return population covariance.

You can also enter: PCov.

## PopulationStandardDeviation

Return population standard deviation.

You can also enter: PSDev.

## PopulationVariance

Return population variance, using the full population denominator.

You can also enter: PVar.

Try it
[[1 2][2 4][3 6]] StoreData PopulationVariance

You should get: [ ²/₃ 2 ²/₃ ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## PowerFit

Select the engine power-law regression model.

You can also enter: PwrFit.

## PredictX

Predict X from a supplied Y using the current engine regression.

You can also enter: PredX.

## PredictY

Predict Y from a supplied X using the current engine regression.

You can also enter: PredY.

## RecallData

Recall the engine statistical-data object onto the stack.

You can also enter: RecallΣ, RclΣ.

Try it
[[1 2][2 4][3 6]] StoreData RecallData

You should get: [[ 1 2 ]
  [ 2 4 ]
  [ 3 6 ]]
Sets example statistics data, then recalls it.

## RegressionFormula

Return the current engine regression equation. Native Data regression is managed through Ctrl+D.

You can also enter: ΣLine.

## RemoveData

Remove supplied observations from the engine statistical data.

You can also enter: Σ-.

## Slope

Recall the slope last computed by LinearRegression. Importing data alone does not calculate it.

Try it
[[1 2][2 4][3 6]] StoreData LinearRegression DROP2 Slope

You should get: 2
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## StandardDeviation

Return sample standard deviation from engine statistical data.

You can also enter: SDev.

Try it
[[1 2][2 4][3 6]] StoreData StandardDeviation

You should get: [ 1. 2. ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## StatsData

Access the engine statistical-data object. Ctrl+D uses a separate native data table.

You can also enter: ΣData, ΣDat.

## StatsParameters

Access the engine statistical parameters.

You can also enter: ΣParameters, ΣPar.

## StoreData

Replace engine statistical data with the supplied object. Save native Data tables through their CSV file menu.

You can also enter: StoreΣ, StoΣ.

Try it
[[1 2][2 4][3 6]] StoreData RecallData

You should get: [[ 1 2 ]
  [ 2 4 ]
  [ 3 6 ]]
Replaces the current statistics data. RecallData reads it back.

## SumOfX

Return the sum of values in the independent data column.

You can also enter: ΣX.

Try it
[[1 2][2 4][3 6]] StoreData SumOfX

You should get: 6
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## SumOfXSquares

Return the sum of squared X values.

You can also enter: ΣX², ΣX2.

Try it
[[1 2][2 4][3 6]] StoreData SumOfXSquares

You should get: 14
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## SumOfXY

Return the sum of paired X*Y values.

You can also enter: ΣXY.

Try it
[[1 2][2 4][3 6]] StoreData SumOfXY

You should get: 28
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## SumOfY

Return the sum of values in the dependent data column.

You can also enter: ΣY.

Try it
[[1 2][2 4][3 6]] StoreData SumOfY

You should get: 12
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## SumOfYSquares

Return the sum of squared Y values.

You can also enter: ΣY², ΣY2.

Try it
[[1 2][2 4][3 6]] StoreData SumOfYSquares

You should get: 56
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## TVMAmort

Calculate amortization using engine TVM variables.

You can also enter: TVMAmortize, Amort.

## TVMAmortTable

Build an amortization table using engine TVM variables.

You can also enter: TVMAmortizationTable, AmortTable, AmortizationSchedule.

## TVMEquation

Return the time-value-of-money equation. The native Financial calculator keeps its own saved inputs.

## TVMRoot

Solve the engine time-value-of-money equation for the selected unknown.

## Variance

Return sample variance from the engine statistical data.

You can also enter: Var.

Try it
[[1 2][2 4][3 6]] StoreData Variance

You should get: [ 1 4 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## AssignKey

Assign an object to an engine virtual-key position.

You can also enter: Asn.

## Attach

Attach the specified engine library.

## BatteryVoltage

A measured battery voltage is unavailable on PicoCalc. Use Settings > Device status > Battery for the keyboard controller's percentage and charging state.

## Beep

Play a tone. Supply frequency in Hz followed by duration in seconds; for example 440 0.1 Beep. BeepOn enables sound.

## Bytes

Return information about an object’s bytes and its binary check representation.

## Catalog

Open the calculator command catalogue. Ctrl+A opens it directly; choosing an entry inserts its name.

## Const

Look up the supplied name in the constants library. Its SD resource must be available.

You can also enter: LibraryConstant.

## ConstantName

Return the name of a library constant.

You can also enter: EquationName, XLibName.

## ConstantRange

Return the range of a library constant when its definition includes uncertainty.

## Constants

Access the engine constants library.

## ConstantValue

Return the value of a library constant.

You can also enter: EquationValue, XLibValue.

## CustomMenu

Open the calculator custom soft menu defined by its custom-menu settings.

You can also enter: Cst.

## Cycle

Cycle the supplied object between its supported forms.

## DeleteKeys

Remove specified engine virtual-key assignments.

You can also enter: DelKeys.

## Detach

Detach the specified engine library.

## DMCPLowBattery

Read the platform low-battery flag supplied by the PicoCalc backend.

## doerr

Raise an error from the supplied message or error value.

You can also enter: DoError.

## DTag

Remove an object's tag.

You can also enter: DeleteTag.

Try it
:Answer:42 DTAG

You should get: 42

## Edit

Edit the top stack object in calculator command entry. To edit an SD RPL or Lua file, open it through Programming instead.

## err0

Clear the current calculator error. Esc also dismisses a visible error.

You can also enter: ClearError.

## errm

Return the most recent calculator error message.

You can also enter: ErrorMessage.

## errn

Return the most recent calculator error number.

You can also enter: ErrorNumber.

## FinanceSolverMenu

Open the engine TVM-variable solver menu. The native Financial calculator has separate saved inputs.

You can also enter: TVM.

## FlightRecorderConfigure

Configure the engine developer flight recorder with the supplied configuration. Recorder output depends on build diagnostics.

## FlightRecorderDump

Send flight-recorder data to the configured diagnostic output.

You can also enter: WSLog.

## FlightRecorderDumpSome

Send selected flight-recorder data to diagnostic output using the supplied selection.

## FreeMemory

Return engine free-memory information.

## FromTag

Remove a tag and return its contents using the engine tag convention.

You can also enter: Tag→.

Try it
:Answer:42 FromTag

You should get: 42
"Answer"
Leaves the untagged value below its text tag; the display lists deepest first.

## GarbageCollect

Reclaim unused calculator arena objects.

You can also enter: GC.

## GarbageCollectorStatistics

Return engine garbage-collection statistics.

You can also enter: GCStats.

## Header

Set the calculator engine header using the supplied header object.

You can also enter: Hdr.

## Help

Open the LumaRPL command reference. Its lists and pages use the same arrows, Enter and F5/Back controls as the rest of Help.

## KeyCode

Return the object assigned to a supplied engine virtual-key position. The position is an engine keypad code, not an ASCII character or physical QWERTY key.

## KeyEval

Evaluate an engine virtual-key action. Its key positions refer to the engine keypad, not physical QWERTY character codes.

## KeyMap

Store a complete engine virtual-key map from the supplied directory object. This is an advanced RPL key-assignment operation, separate from PicoCalc physical shortcuts.

You can also enter: Keys.

## LastMenu

Return to the previous calculator soft menu. Pressing Esc now does this directly; at the top level Esc clears the menu.

## LibEq

Load an equation from the equation library using the supplied reference.

You can also enter: LibraryEquation.

## Libs

List attached engine libraries.

You can also enter: Libraries.

## LowBattery

This legacy voltage-threshold check cannot measure PicoCalc battery health. Use Device status > Battery.

## Mem

Report available calculator object memory.

You can also enter: AvailableMemory.

## Menu

Open the supplied engine menu or custom menu definition.

## Modes

Put a program containing the nondefault calculator settings on the stack. Type EVAL to apply that program. The Advanced RPL settings →Prog key performs this action.

## Off

Request engine power-off. To save documents and return to the PicoCalc loader, use Ctrl+Q.

You can also enter: PowerOff.

## OffWithImage

Request engine power-off with its off-image behavior. Ctrl+Q is the safe save-and-return-to-loader workflow.

You can also enter: PowerOffWithImage.

## PowerVoltage

The port cannot provide a measured supply voltage through this command. Use Device status > Battery instead.

## PurgeAll

Remove calculator variables in the applicable directory scope. Save important state first.

You can also enter: PgAll.

## RecallKeys

Recall engine virtual-key assignments.

You can also enter: RclKeys.

## RecallMenu

Return the current engine menu.

You can also enter: RclMenu.

## ResetModes

Restore the engine's default calculator settings. This changes settings, so save the session first if you want to keep the current ones.

## RuntimeStatistics

Return engine execution statistics.

You can also enter: RunStats.

## SaveState

Save engine calculator state using the supplied state-file workflow. Ctrl+S is the normal PicoCalc session-save shortcut.

## ScreenCapture

The original hardware screenshot command is unavailable on PicoCalc. Use Export from a native graph for SVG or sampled CSV.

## SolvingMenu

Open the engine equation-variable solver menu. Ctrl+W provides guided single-equation and system solving.

## StackEditor

Open the interactive stack. Arrows select a stack level; Tab chooses the action row; F1-F5 and Ctrl+F5 run the displayed action.

## StandardKey

Use the standard engine action for a virtual-key position.

You can also enter: SKey.

## StoreKeys

Store supplied engine key assignments.

You can also enter: StoKeys.

## SystemMemory

The original system-memory diagnostic is unavailable here and returns zero. Open Settings > Device status > Memory / PSRAM for PicoCalc memory information.

## SystemSetup

Open PicoCalc Settings, the same screen as the System action in Advanced RPL settings.

## TemporaryMenu

Show a temporary engine menu using the supplied definition.

You can also enter: TMenu.

## ToggleCustomMenu

Switch the engine custom soft menu on or off.

## ToggleUserMode

Toggle engine user-key mode. This concerns virtual-key assignments, not a new QWERTY alphabet mode.

## ToolsMenu

Open the context-sensitive calculator soft menu for the current object or entry.

## ToTag

Attach the supplied label to an object.

You can also enter: →Tag.

Try it
42 "Answer" ToTag

You should get: Answer:42

## Type

Return the type number of the supplied object.

Try it
42 TYPE

You should get: 28
28 is this engine's type code for an integer; TypeName gives a readable name.

## TypeName

Return the type name of the supplied object.

Try it
42 TypeName

You should get: "integer"

## Unimplemented

Report that an engine operation has no implementation in this build.

## USBPowered

On PicoCalc this returns True when the keyboard battery controller reports charging, and False otherwise. It is not an independent USB-cable detector. Unknown battery data can also give False; Battery status shows whether a reading is available.

## VariablesMenu

Open the calculator variable soft menu. Tab selects a row; F1-F5 and Ctrl+F5 choose its columns.

## Version

Return the engine version information. About LumaRPL shows the installed PicoCalc release.

## XLib

Load or call an SD library object using its library reference.

You can also enter: LibraryItem.

## ApplyInverseUnit

Attach the reciprocal of the unit supplied by the current unit-menu key.

## ApplyUnit

Attach the unit chosen by the current engine unit-menu key. The menu supplies the unit; typing this helper alone does not choose one.

## Convert

Convert a quantity to the supplied target unit. Ctrl+U offers From/To unit lists.

Try it
100_cm 1_m Convert

You should get: 1 m

## ConvertToUnit

Convert to the unit supplied by the current engine unit-menu key. Ctrl+U is easier for a guided conversion.

## ConvertToUnitPrefix

Apply the prefix supplied by the active engine unit-prefix menu key.

## FromRange

Unpack a range into its defining values.

You can also enter: Range→, RangeToReal.

Try it
2 4 ToRange FromRange

You should get: 2
4

## PrecisionRound

Round to the supplied precision.

You can also enter: PrcRnd.

Try it
-3.141592654_m 0.000045_m  PrcRnd

You should get: -3.14159 3 m
Run from the calculator stack. This example uses the current calculation settings.

## RangeIntersect

Return the overlap of two ranges.

You can also enter: ∩, Intersect.

Try it
1 3 ToRange 2 6 ToRange RangeIntersect

You should get: 2…3
Run from the calculator stack. This example uses the current calculation settings.

## RangeUnion

Return the union of two ranges where representable.

You can also enter: ∪, Union.

Try it
1 3 ToRange 2 6 ToRange RangeUnion

You should get: 1…6
Run from the calculator stack. This example uses the current calculation settings.

## RelativeRound

Round using the supplied relative uncertainty.

You can also enter: RelRnd.

Try it
-3.141592654_m  0.000012  RelRnd

You should get: -3.14159 3 m
Run from the calculator stack. This example uses the current calculation settings.

## RelativeUncertainty

Return a quantity's relative uncertainty.

You can also enter: CstRU.

## StandardRound

Round using the supplied absolute uncertainty.

You can also enter: StdRnd.

Try it
-3.141592654_m  0.000045_m  StdRnd

You should get: -3.14159 3 m
Run from the calculator stack. This example uses the current calculation settings.

## StandardUncertainty

Return a quantity's absolute standard uncertainty.

You can also enter: CstSU.

## ToDeltaRange

Build a range using a central value and absolute uncertainty.

You can also enter: →∆Range.

Try it
10 2 ToDeltaRange

You should get: 10±2

## ToPercentRange

Build a range using a central value and percentage uncertainty.

You can also enter: →%Range.

Try it
10 20 ToPercentRange

You should get: 10±20%
Run from the calculator stack. This example uses the current calculation settings.

## ToRange

Build a range from supplied bounds.

You can also enter: →Range, →Interval.

Try it
2 4 ToRange

You should get: 2…4

## ToRelativeUncertainty

Build an uncertain quantity using relative uncertainty.

You can also enter: →Ur, →RelUnc.

Try it
-3.141592654_m  0.000097_m  ToRelativeUncertainty

You should get: 0.00003 1
Run from the calculator stack. This example uses the current calculation settings.

## ToStandardUncertainty

Build an uncertain quantity using absolute uncertainty.

You can also enter: →Us, →StdUnc.

Try it
-3.141592654_m  0.000012  ToStandardUncertainty

You should get: 0.00003 8 m
Run from the calculator stack. This example uses the current calculation settings.

## ToUncertain

Build an uncertain number from a central value and standard deviation. Converting an existing range to this representation is not implemented.

You can also enter: →σRange, →Uncertain, →Un.

Try it
10 1 ToUncertain

You should get: 10±σ1

## ToUnit

Combine the supplied value and unit into a quantity. Use Convert to change an existing quantity to another unit.

You can also enter: →Unit.

Try it
5 1_m ToUnit

You should get: 5 m

## UBase

Express a quantity in base units.

You can also enter: BaseUnits.

Try it
1_cm UBASE

You should get: ¹/₁₀₀ m

## UFact

Factor a quantity's units using the supplied unit factor.

You can also enter: FactorUnit.

## UnitsSIPrefixCycle

Cycle the SI prefix used by a unit-bearing value.

## UVal

Extract the value from a unit-bearing quantity.

You can also enter: UnitValue.

Try it
25_m UVAL

You should get: 25

## Algebraic expressions

Single quotes keep an algebraic expression on the stack without evaluating its symbols. Try 'X^2+1'. EVAL evaluates it using available variable values.

In a native formula field, enter X^2+1 directly; the field already knows it is a formula. RPL programs instead use « and » and contain stack commands. An expression such as X^2 and a program such as « DUP * » are different kinds of object.

## Arrays

Square brackets make vectors and matrices. A matrix contains rows, for example [[1 2][3 4]].

Try [[1 2][3 4]] DET to get -2. Matrix addition needs matching dimensions; matrix multiplication needs columns of the first to match rows of the second. Home > Math & graphs > Matrix workbench offers a grid editor and guided operations.

## Based numbers

Based integers support binary, octal, decimal, hexadecimal and other radices. Bin, Oct, Dec and Hex select common display bases. Base accepts a radix from 2 to 36.

On LumaRPL, Bin, Oct, Dec and Hex also convert an ordinary decimal integer at the top directly. For example, enter 42, then Bin to see 101010; Hex shows 2A. Decimal fractions are not converted. RealToBinary remains available explicitly; BinaryToReal returns a based object to an ordinary integer. WordSize controls fixed-width operations such as shifts and rotations. Changing a displayed base changes the representation rather than the value.

## DO UNTIL END

DO runs the body before its test, so it runs at least once. Put the body after DO and the stopping condition after UNTIL.

Try 0 « DO 1 + UNTIL DUP 3 < NOT END » EVAL. It stops at 3. Unlike WHILE, a true UNTIL condition ends the loop.

## FOR NEXT

FOR repeats a program body while giving a local variable successive values. Put the first and last values before FOR, followed by the variable name. NEXT advances by one.

Try 0 « 1 3 FOR K K + NEXT » EVAL. It adds 1, 2 and 3 to the starting zero, leaving 6. STEP can use a supplied increment instead of NEXT.

## IF THEN ELSE END

Use IF inside a program to choose a branch. The code before THEN leaves a condition; a true condition runs the THEN branch and a false one runs ELSE. ELSE is optional.

Try « IF 2 1 > THEN 7 ELSE 9 END » EVAL. The result is 7. Type the structure into a program; the words are not separate calculator buttons.

## Library programs

The SD library contains additional programs and equations. At the calculator, type Library and press Enter to browse its program groups. Supply a program's required stack inputs before running it.

A library reference uses the Ⓛ marker and needs the matching library resources on SD. It is not the same as a built-in command. The current reference lists built-in commands and settings; the supplied library definitions remain under /db48x/config and /db48x/library.

## Lists

A list groups objects inside braces, for example {1 2 3}. Elements can be numbers, expressions or other objects.

Try {1 2 3} ListSum to get 6. GET reads an element and PUT replaces one. List positions start at 1. MAP, REDUCE and FILTER apply operations to elements; open their entries for those operations.

## Local variables

A local-variable structure takes values from the stack and gives them names for its body. Those names do not replace your HOME variables.

Try 2 « -> WIDTH « WIDTH WIDTH * » » EVAL. The result is 4. Type -> for the arrow. It introduces the local names; the following program is their body. With several names, supply values in the matching order.

## Reading command pages

Open an entry with Enter or F1. Up/Down scrolls its page, Left/Right moves by a page, and F5 or Esc returns to the same list position. F4 Find in the list searches names and accepted spellings; an exact match comes first.

Read an example here, then return to the calculator to type it. Reading or pressing Enter on a help page does not run the example or change your stack. Save important work before trying examples that store variables or change settings.

## RPL Programming

An RPL program is a sequence inside « and ». It can use values already on the stack and leave its answers there.

Try « DUP * » 'SQDEMO' STO, then 12 SQDEMO: the answer is 144. Ctrl+B and Ctrl+E enter the two program delimiters.

Entering a program on its own puts it on the stack. EVAL runs it. A stored name runs its program when evaluated; quoting the name keeps it as a name. Programming > HOME RPL also offers F2 Run. Save the session to keep HOME programs after restart. SD source files have their own Save action.

## Settings and saving

Some setting commands need a value: 3 Fix means three displayed decimal places. Others select a choice without an argument: Rad selects radians. Flags such as BeepOn and BeepOff select opposite behaviors.

Choosing an angle or display mode again does not turn it off. A Beep soft-menu key toggles instead. Save the calculator session with Ctrl+S if you want the current settings after restart. Native documents use their own Save actions.

## Stack order

The bottom of a written input sequence is earlier in the stack; the last value you type is on top. Commands use their inputs from there.

For 10 3 -, 3 is on top and is subtracted from 10, leaving 7. SWAP exchanges the top two values and DUP copies the top one. Too few arguments means one or more needed values are missing. Esc dismisses the error so you can supply them.

## START NEXT

START repeats a body without naming the loop counter. Supply the first and last counter values before START.

Try 0 « 1 3 START 1 + NEXT » EVAL. The body adds 1 three times, leaving 3. Use FOR when the body needs the counter value.

## Units and conversions

Attach a unit with an underscore: 100_cm is a length. To convert it, try 100_cm 1_m Convert. The result is 1 metre. Unit names are case-sensitive and dimensions must be compatible.

Ctrl+U offers From/To lists without having to type unit syntax. Absolute temperatures and temperature differences are different quantities.

## WHILE REPEAT END

WHILE tests before running the body, so the body can run zero times. Put the condition between WHILE and REPEAT, and the body between REPEAT and END.

Try 0 « WHILE DUP 3 < REPEAT 1 + END » EVAL. The answer is 3. Keep the values needed by the next condition on the stack.

## Ⓓ

Return the calculator directory used for algebra configuration.

You can also enter: Ⓓ, CASDir.

## CASDir

Return the calculator directory used for algebra configuration.

You can also enter: Ⓓ, CASDir.

## ⓧ

Recall the current algebra variable.

You can also enter: ⓧ, VX, RclVX.

## VX

Recall the current algebra variable.

You can also enter: ⓧ, VX, RclVX.

## RclVX

Recall the current algebra variable.

You can also enter: ⓧ, VX, RclVX.

## ∂

Differentiate an expression with respect to a quoted variable. Example: 'X^2' 'X' Derivative gives 2*X.

You can also enter: ∂.

Try it
'X^3' 'X' Derivative

You should get: '3·X²'

## Eq

Use the supplied equation definition in the engine equation/library context.

You can also enter: Eq.

## EvaluateEquation

Evaluate the current engine equation.

You can also enter: EvaluateEquation.

## Expan

Expand products and powers in an algebraic expression.

You can also enter: Expan.

Try it
'(X+1)^2' EXPAND

You should get: 'X²+2·X+1'

## Polynomial→

Convert a polynomial object back to an algebraic expression.

You can also enter: Polynomial→, Poly→.

## Poly→

Convert a polynomial object back to an algebraic expression.

You can also enter: Polynomial→, Poly→.

## ∫

Numerically integrate an expression over the supplied variable and bounds. Home > Math & graphs > Calculus provides a guided definite integral.

You can also enter: ∫.

Try it
Rad 0 1 'X^2' 'X' Integrate

You should get: ¹/₃

## Isol

Rearrange an equation to isolate the supplied variable. Some equations cannot be isolated by the engine.

You can also enter: Isol.

Try it
'X+2=5' 'X' ISOL

You should get: 'X=3'

## ↓Match

Apply a symbolic rewrite while traversing the expression downward.

You can also enter: ↓Match.

## ↑Match

Apply a symbolic rewrite while traversing the expression upward.

You can also enter: ↑Match.

## MRoot

Solve several equations numerically using the supplied variables and guesses. Ctrl+W > Equation system is the guided alternative.

You can also enter: MRoot.

## MSlv

Solve using the supplied equation, variables and starting values through the engine multi-variable solver.

You can also enter: MSlv.

## NextEquation

Select the next equation in the engine equation set.

You can also enter: NextEquation.

## PartialFractions

Decompose a rational expression into partial fractions where supported.

You can also enter: PartialFractions.

## PolynomialCoefficients

Return polynomial coefficients using the engine's coefficient order.

You can also enter: PolynomialCoefficients.

Try it
CompatiblePolynomials
[ 2 -3 4 -5 ] PCoef

You should get: [ 1 2 -25 -26 120 ]
Run from the calculator stack. This example uses the current calculation settings.

## PolynomialEvaluation

Evaluate a polynomial using the supplied argument.

You can also enter: PolynomialEvaluation.

Try it
[ 1 -5 6 ] 2 PEval

You should get: 0
Run from the calculator stack. This example uses the current calculation settings.

## Risch

Find a symbolic antiderivative. The arbitrary constant is implicit.

You can also enter: Risch.

Try it
'X^2' 'X' Primitive

You should get: 'X³÷3'

## ∏

Evaluate a product over an expression, variable and bounds.

You can also enter: ∏.

## PolynomialRoots

Find roots of the supplied polynomial using the polynomial solver.

You can also enter: PolynomialRoots.

Try it
[ 1 2 -25 -26 120 ] PRoot

You should get: [ -5 -3 2 4 ]
Run from the calculator stack. This example uses the current calculation settings.

## RecallEquation

Recall the current engine equation.

You can also enter: RecallEquation.

## StoreEquation

Store the supplied equation as the engine's current equation.

You can also enter: StoreEquation.

## Storeⓧ

Set the algebra variable from the supplied name.

You can also enter: Storeⓧ, StoVX.

## StoVX

Set the algebra variable from the supplied name.

You can also enter: Storeⓧ, StoVX.

## Substitute

Substitute the supplied replacement into an expression without assigning a calculator variable.

You can also enter: Substitute.

Try it
'X+2' 'X=3' SUBST

You should get: '3+2'

## Σ

Evaluate a summation from an expression, variable and bounds.

You can also enter: Σ.

## →Polynomial

Convert an expression to the engine polynomial representation.

You can also enter: →Polynomial, →Poly.

## →Poly

Convert an expression to the engine polynomial representation.

You can also enter: →Polynomial, →Poly.

## ArithmeticShiftRight

Shift a based integer right while preserving its sign bit.

You can also enter: ArithmeticShiftRight.

## ArithmeticShiftRightByte

Shift a based integer right by one byte while preserving its sign bit.

You can also enter: ArithmeticShiftRightByte.

## ArithmeticShiftRightCount

Shift a based integer right by the supplied count while preserving its sign.

You can also enter: ArithmeticShiftRightCount.

## rcws

Put the current based-integer word size on the stack.

You can also enter: rcws.

## RotateLeft

Rotate the bits of a based integer left within WordSize.

You can also enter: RotateLeft.

Try it
#1h RL

You should get: #2₁₆

## RotateLeftByte

Rotate a based integer left by one byte.

You can also enter: RotateLeftByte.

## RotateLeftCount

Rotate a based integer left by the supplied count.

You can also enter: RotateLeftCount.

## RotateRight

Rotate the bits of a based integer right within WordSize.

You can also enter: RotateRight.

Try it
#2h RR

You should get: #1₁₆

## RotateRightByte

Rotate a based integer right by one byte.

You can also enter: RotateRightByte.

## RotateRightCount

Rotate a based integer right by the supplied count.

You can also enter: RotateRightCount.

## ShiftLeft

Shift a based integer left, filling vacated bits with zero.

You can also enter: ShiftLeft.

Try it
#1h SL

You should get: #2₁₆

## ShiftLeftByte

Shift a based integer left by one byte.

You can also enter: ShiftLeftByte.

Try it
#1h SLB

You should get: #100₁₆

## ShiftLeftCount

Shift a based integer left by the supplied count.

You can also enter: ShiftLeftCount.

## ShiftRight

Shift a based integer right, filling vacated bits with zero.

You can also enter: ShiftRight.

Try it
#8h SR

You should get: #4₁₆

## ShiftRightByte

Shift a based integer right by one byte.

You can also enter: ShiftRightByte.

Try it
#100h SRB

You should get: #1₁₆

## ShiftRightCount

Shift a based integer right by the supplied count.

You can also enter: ShiftRightCount.

## ConLib

Open the calculator constants soft menu. Type ConstantsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: ConLib, ConstantsLibrary.

Try it
ConstantsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ConstantsLibrary

Open the calculator constants soft menu. Type ConstantsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: ConLib, ConstantsLibrary.

Try it
ConstantsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## EqnLib

Open the calculator equations soft menu. Type EquationsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: EqnLib, EquationsLibrary.

Try it
EquationsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## EquationsLibrary

Open the calculator equations soft menu. Type EquationsMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: EqnLib, EquationsLibrary.

Try it
EquationsMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## DifferentiationMenu

Open the calculator integration soft menu. Type IntegrationMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: DifferentiationMenu.

Try it
IntegrationMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## ExpLogMenu

Open the calculator powers soft menu. Type PowersMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: ExpLogMenu.

Try it
PowersMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## MSolvr

Open the calculator solver soft menu. Type SolverMenu and press Enter.

Tab selects its row. F1-F5 chooses columns 1-5; Ctrl+F5 chooses column 6. Esc goes back one menu level, then clears the top-level menu.

You can also enter: MSolvr.

Try it
SolverMenu

The soft menu opens. Press Tab to select a row, then F1-F5 (Ctrl+F5 for column six). Esc returns one menu level.

## Binary

Show based integers in base 2. On LumaRPL this also makes a fixed-base integer at the top follow the selected display base, without changing its value. Fixed-base values below the top retain their representation. Ordinary decimal integers at the top convert directly too; decimal fractions remain unchanged. Negative integers use the current WordSize, as with RealToBinary. Ctrl+Y opens Bases from the calculator.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Binary.

Try it
65490 Bin

You should get: #1111 1111 1101 0010₂
The value is 65490 in decimal. Decimal integers and explicit binary, octal or hexadecimal integers convert directly. Decimal fractions are not converted.

## CapitalizedCommands

Choose capitalized rendering for calculator command names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: CapitalizedCommands.

Try it
Capitalized

Applies the display or angle choice described above.

## CapitalizedSymbols

Choose capitalized rendering for symbolic names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: CapitalizedSymbols.

Try it
CapitalizedNames

Applies the display or angle choice described above.

## Decimal

Show based integers in base 10. On LumaRPL this also makes a fixed-base integer at the top follow the selected display base, without changing its value. Fixed-base values below the top retain their representation. Ordinary decimal integers at the top convert directly too; decimal fractions remain unchanged. Negative integers use the current WordSize, as with RealToBinary. Ctrl+Y opens Bases from the calculator.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Decimal.

Try it
#177722o Dec

You should get: #6 5490₁₀
The value is 65490 in decimal. Decimal integers and explicit binary, octal or hexadecimal integers convert directly. Decimal fractions are not converted.

## Degrees

Use degrees for angles: a full turn is 360.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Degrees.

Try it
Deg

Applies the display or angle choice described above.

## EngineeringDisplay

Show numbers in engineering notation, with exponents in multiples of three and the supplied digit count.

Put the digit count on the stack first. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: EngineeringDisplay.

Try it
3 Eng

Shows three digits using this display style.

## FixedDisplay

Choose how many digits to show after the decimal point. For example, 3 Fix displays 1.25 as 1.250.

Put the digit count on the stack first. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: FixedDisplay.

Try it
3 Fix

Shows three digits using this display style.

## →QDigits

The digit limit used when converting a value to a fraction, rather than a display-decimal setting.

To change it, enter a value followed by FractionDigits. Read the current value with 'FractionDigits' RCL.

You can also enter: →QDigits.

Try it
'FractionDigits' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter FractionDigits. Save the session to keep it after restart.

## →QIterations

How many iterations fraction conversion may try.

To change it, enter a value followed by FractionIterations. Read the current value with 'FractionIterations' RCL.

You can also enter: →QIterations.

Try it
'FractionIterations' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter FractionIterations. Save the session to keep it after restart.

## →QπMaxPrime

The prime-search limit used during fraction conversion.

To change it, enter a value followed by FractionLargestPrime. Read the current value with 'FractionLargestPrime' RCL.

You can also enter: →QπMaxPrime.

Try it
'FractionLargestPrime' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter FractionLargestPrime. Save the session to keep it after restart.

## Grads

Use grads for angles: a full turn is 400.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Grads.

Try it
Grad

Applies the display or angle choice described above.

## HFP

HardwareFloatingPoint permits hardware floating-point arithmetic where supported. SoftwareFloatingPoint selects software arithmetic.

You can also enter: HFP, HardFP.

Try it
HardwareFloatingPoint

Applies the choice described above without a stack argument. Enter SoftwareFloatingPoint to choose its opposite. Save the session to keep this preference.

## HardFP

HardwareFloatingPoint permits hardware floating-point arithmetic where supported. SoftwareFloatingPoint selects software arithmetic.

You can also enter: HFP, HardFP.

Try it
HardwareFloatingPoint

Applies the choice described above without a stack argument. Enter SoftwareFloatingPoint to choose its opposite. Save the session to keep this preference.

## Hexadecimal

Show based integers in base 16. On LumaRPL this also makes a fixed-base integer at the top follow the selected display base, without changing its value. Fixed-base values below the top retain their representation. Ordinary decimal integers at the top convert directly too; decimal fractions remain unchanged. Negative integers use the current WordSize, as with RealToBinary. Ctrl+Y opens Bases from the calculator.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Hexadecimal.

Try it
#1111111111010010b Hex

You should get: #FFD2₁₆
The value is 65490 in decimal. Decimal integers and explicit binary, octal or hexadecimal integers convert directly. Decimal fractions are not converted.

## LongFormCommands

Choose long form rendering for calculator command names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: LongFormCommands.

Try it
LongForm

Applies the display or angle choice described above.

## LongFormSymbols

Choose long form rendering for symbolic names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: LongFormSymbols.

Try it
LongFormNames

Applies the display or angle choice described above.

## LowerCaseCommands

Choose lower case rendering for calculator command names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: LowerCaseCommands.

Try it
LowerCase

Applies the display or angle choice described above.

## LowerCaseSymbols

Choose lower case rendering for symbolic names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: LowerCaseSymbols.

Try it
LowerCaseNames

Applies the display or angle choice described above.

## MaxH

The maximum enlarged calculator-object rendering height in pixels.

To change it, enter a value followed by MaximumShowHeight. Read the current value with 'MaximumShowHeight' RCL.

You can also enter: MaxH.

Try it
'MaximumShowHeight' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaximumShowHeight. Save the session to keep it after restart.

## MaxW

The maximum enlarged calculator-object rendering width in pixels.

To change it, enter a value followed by MaximumShowWidth. Read the current value with 'MaximumShowWidth' RCL.

You can also enter: MaxW.

Try it
'MaximumShowWidth' RCL

Shows the current value. To change it, replace the recalled value with your chosen value and enter MaximumShowWidth. Save the session to keep it after restart.

## Octal

Show based integers in base 8. On LumaRPL this also makes a fixed-base integer at the top follow the selected display base, without changing its value. Fixed-base values below the top retain their representation. Ordinary decimal integers at the top convert directly too; decimal fractions remain unchanged. Negative integers use the current WordSize, as with RealToBinary. Ctrl+Y opens Bases from the calculator.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Octal.

Try it
#FFD2h Oct

You should get: #17 7722₈
The value is 65490 in decimal. Decimal integers and explicit binary, octal or hexadecimal integers convert directly. Decimal fractions are not converted.

## πradians

Measure angles in multiples of pi radians: 2 is a full turn, and 0.5 is a right angle.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: πradians.

Try it
PiRadians

Applies the display or angle choice described above.

## Prec

How many significant decimal digits calculations use. This affects arithmetic precision; Fix only changes displayed decimal places. For example, enter 16 Precision.

To change it, enter a value followed by Precision. Read the current value with 'Precision' RCL.

You can also enter: Prec.

Try it
16 Precision 'Precision' RCL

You should get: 16
Changes this calculator setting. Save the session to retain it after restart.

## Radians

Use radians for angles: a full turn is 2*pi.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: Radians.

Try it
Rad

Applies the display or angle choice described above.

## ScientificDisplay

Show numbers in scientific notation with the supplied digit count.

Put the digit count on the stack first. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: ScientificDisplay.

Try it
3 Sci

Shows three digits using this display style.

## SignificantDisplay

Choose how many significant digits to display.

Put the digit count on the stack first. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: SignificantDisplay.

Try it
3 Sig

Shows three digits using this display style.

## SFP

HardwareFloatingPoint permits hardware floating-point arithmetic where supported. SoftwareFloatingPoint selects software arithmetic.

You can also enter: SFP, SoftFP.

Try it
SoftwareFloatingPoint

Applies the choice described above without a stack argument. Enter HardwareFloatingPoint to choose its opposite. Save the session to keep this preference.

## SoftFP

HardwareFloatingPoint permits hardware floating-point arithmetic where supported. SoftwareFloatingPoint selects software arithmetic.

You can also enter: SFP, SoftFP.

Try it
SoftwareFloatingPoint

Applies the choice described above without a stack argument. Enter HardwareFloatingPoint to choose its opposite. Save the session to keep this preference.

## StandardDisplay

Use standard number display. The calculator chooses a suitable decimal or exponent form.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: StandardDisplay.

Try it
Std

Applies the display or angle choice described above.

## TVMBeg

TVMPayAtBeginningOfPeriod uses payments at the start of a period. TVMPayAtEndOfPeriod uses payments at its end in the engine TVM solver.

You can also enter: TVMBeg.

Try it
TVMPayAtBeginningOfPeriod

Applies the choice described above without a stack argument. Enter TVMPayAtEndOfPeriod to choose its opposite. Save the session to keep this preference.

## TVMEnd

TVMPayAtBeginningOfPeriod uses payments at the start of a period. TVMPayAtEndOfPeriod uses payments at its end in the engine TVM solver.

You can also enter: TVMEnd.

Try it
TVMPayAtEndOfPeriod

Applies the choice described above without a stack argument. Enter TVMPayAtBeginningOfPeriod to choose its opposite. Save the session to keep this preference.

## UpperCaseCommands

Choose upper case rendering for calculator command names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: UpperCaseCommands.

Try it
UpperCase

Applies the display or angle choice described above.

## UpperCaseSymbols

Choose upper case rendering for symbolic names.

Enter the command without a stack value. This choice stays active until you select another; choosing it again does not turn it off.

You can also enter: UpperCaseSymbols.

Try it
UpperCaseNames

Applies the display or angle choice described above.

## stws

The bit width used by fixed-width based-number operations. This matters for shifts, rotations and signed interpretation.

To change it, enter a value followed by WordSize. Read the current value with 'WordSize' RCL.

You can also enter: stws.

Try it
32 WordSize 'WordSize' RCL

You should get: 32
Changes this calculator setting. Save the session to retain it after restart.

## Column+

Insert the supplied column into a matrix at the supplied position.

You can also enter: Column+, Col+.

Try it
[[ 1 2 3 ] [ 4 5 6 ]]  @ Input matrix
[ 22 33 ]              @ Column to insert
2                      @ Insertion position
COL+

You should get: [[ 1 22 2 3 ]
  [ 4 33 5 6 ]]
Run from the calculator stack. This example uses the current calculation settings.

## Col+

Insert the supplied column into a matrix at the supplied position.

You can also enter: Column+, Col+.

Try it
[[ 1 2 3 ] [ 4 5 6 ]]  @ Input matrix
[ 22 33 ]              @ Column to insert
2                      @ Insertion position
COL+

You should get: [[ 1 22 2 3 ]
  [ 4 33 5 6 ]]
Run from the calculator stack. This example uses the current calculation settings.

## Row+

Insert the supplied row into a matrix at the supplied position.

You can also enter: Row+.

Try it
[[ 1 2 3 ] [ 4 5 6 ]]  @ Input matrix
[ 22 33 44 ]           @ Row to insert
2                      @ Insertion position
ROW+

You should get: [[ 1 2 3 ]
  [ 22 33 44 ]
  [ 4 5 6 ]]
Run from the calculator stack. This example uses the current calculation settings.

## CNrm

Find the matrix column norm.

You can also enter: CNrm.

## Columns→

Build a matrix from supplied column vectors.

You can also enter: Columns→, Col→.

Try it
[ 1 2 ]
[ 4 5 ]
[ 7 8 ]
3 ColumnsToMatrix

You should get: [[ 1 4 7 ]
  [ 2 5 8 ]]
Run from the calculator stack. This example uses the current calculation settings.

## Col→

Build a matrix from supplied column vectors.

You can also enter: Columns→, Col→.

Try it
[ 1 2 ]
[ 4 5 ]
[ 7 8 ]
3 ColumnsToMatrix

You should get: [[ 1 4 7 ]
  [ 2 5 8 ]]
Run from the calculator stack. This example uses the current calculation settings.

## CSwp

Exchange two specified matrix columns.

You can also enter: CSwp.

Try it
[[1 2][3 4]] 1 2 ColumnSwap

You should get: [[ 2 1 ]
  [ 4 3 ]]

## Con

Build an array filled with a supplied constant.

You can also enter: Con.

Try it
{ 2 2 } 7 CON

You should get: [[ 7 7 ]
  [ 7 7 ]]

## ⨯

Calculate the cross product of two compatible vectors.

You can also enter: ⨯.

Try it
[1 0 0] [0 1 0] CROSS

You should get: [ 0 0 1 ]

## Column-

Remove the specified column from a matrix.

You can also enter: Column-, Col-.

Try it
[[1 2][3 4]] 1 DeleteColumn

You should get: [[ 2 ][ 4 ]]
[ 1 3 ]
Leaves the reduced matrix below the removed column.

## Col-

Remove the specified column from a matrix.

You can also enter: Column-, Col-.

Try it
[[1 2][3 4]] 1 DeleteColumn

You should get: [[ 2 ][ 4 ]]
[ 1 3 ]
Leaves the reduced matrix below the removed column.

## Row-

Remove the specified row from a matrix.

You can also enter: Row-.

Try it
[[1 2][3 4]] 1 DeleteRow

You should get: [[ 3 4 ]]
[ 1 2 ]
Leaves the reduced matrix below the removed row.

## Determinant

Find a square matrix's determinant.

Try it
[[1 2][3 4]] DET

You should get: -2

## ⋅

Calculate the dot product of two vectors.

You can also enter: ⋅.

Try it
[1 2 3] [4 5 6] DOT

You should get: 32

## Arry→

Unpack an array into its values and dimensions.

You can also enter: Arry→, Array→.

Try it
[1 2 3] FromArray

You should get: 1
2
3
{ 3 }
Pushes elements followed by the dimensions.

## Array→

Unpack an array into its values and dimensions.

You can also enter: Arry→, Array→.

Try it
[1 2 3] FromArray

You should get: 1
2
3
{ 3 }
Pushes elements followed by the dimensions.

## Vector→

Unpack a vector into components.

You can also enter: Vector→, V→.

Try it
[ 3 4 ] FromVector Depth ToList

You should get: { 3 4 }
Run from the calculator stack. This example uses the current calculation settings.

## V→

Unpack a vector into components.

You can also enter: Vector→, V→.

Try it
[ 3 4 ] FromVector Depth ToList

You should get: { 3 4 }
Run from the calculator stack. This example uses the current calculation settings.

## Idn

Build a square identity matrix of the supplied size.

You can also enter: Idn.

Try it
2 IDN

You should get: [[ 1 0 ]
  [ 0 1 ]]

## LUFactorization

Factor a matrix into lower and upper triangular factors using the engine pivoting convention.

You can also enter: LUFactorization, LUDecomposition.

## LUDecomposition

Factor a matrix into lower and upper triangular factors using the engine pivoting convention.

You can also enter: LUFactorization, LUDecomposition.

## →Columns

Split a matrix into column vectors.

You can also enter: →Columns, →Col.

Try it
[ 1 2 3 ] MatrixToColumns
4 ToList

You should get: { 1 2 3 3 }
Run from the calculator stack. This example uses the current calculation settings.

## →Col

Split a matrix into column vectors.

You can also enter: →Columns, →Col.

Try it
[ 1 2 3 ] MatrixToColumns
4 ToList

You should get: { 1 2 3 3 }
Run from the calculator stack. This example uses the current calculation settings.

## →Rows

Split a matrix into row vectors.

You can also enter: →Rows, →Row.

Try it
[ 1 2 3 ] MatrixToRows
4 ToList

You should get: { 1 2 3 3 }
Run from the calculator stack. This example uses the current calculation settings.

## →Row

Split a matrix into row vectors.

You can also enter: →Rows, →Row.

Try it
[ 1 2 3 ] MatrixToRows
4 ToList

You should get: { 1 2 3 3 }
Run from the calculator stack. This example uses the current calculation settings.

## Polarℂ→ℝ

Split a polar complex value into magnitude and phase.

You can also enter: Polarℂ→ℝ, P→R.

## P→R

Split a polar complex value into magnitude and phase.

You can also enter: Polarℂ→ℝ, P→R.

## RanM

Build a matrix of random entries with the supplied dimensions.

You can also enter: RanM, RandomArray.

## RandomArray

Build a matrix of random entries with the supplied dimensions.

You can also enter: RanM, RandomArray.

## ℝ→Polarℂ

Combine magnitude and phase into a polar complex value.

You can also enter: ℝ→Polarℂ, R→P.

Try it
5 0 RealToPolar

You should get: 5∡0°

## R→P

Combine magnitude and phase into a polar complex value.

You can also enter: ℝ→Polarℂ, R→P.

Try it
5 0 RealToPolar

You should get: 5∡0°

## ℝ→ℂ

Combine two real components into a rectangular complex value.

You can also enter: ℝ→ℂ, RealToComplex, R→C.

Try it
3 4 RealToRectangular

You should get: 3+4ⅈ

## RealToComplex

Combine two real components into a rectangular complex value.

You can also enter: ℝ→ℂ, RealToComplex, R→C.

Try it
3 4 RealToRectangular

You should get: 3+4ⅈ

## R→C

Combine two real components into a rectangular complex value.

You can also enter: ℝ→ℂ, RealToComplex, R→C.

Try it
3 4 RealToRectangular

You should get: 3+4ⅈ

## ℂ→ℝ

Split a rectangular complex value into real components.

You can also enter: ℂ→ℝ, ComplexToReal, C→R.

Try it
(3;4) RectangularToReal

You should get: re:3
im:4
Pushes the real part, then the imaginary part.

## ComplexToReal

Split a rectangular complex value into real components.

You can also enter: ℂ→ℝ, ComplexToReal, C→R.

Try it
(3;4) RectangularToReal

You should get: re:3
im:4
Pushes the real part, then the imaginary part.

## C→R

Split a rectangular complex value into real components.

You can also enter: ℂ→ℝ, ComplexToReal, C→R.

Try it
(3;4) RectangularToReal

You should get: re:3
im:4
Pushes the real part, then the imaginary part.

## RowEchelonForm

Reduce a matrix to row echelon form.

You can also enter: RowEchelonForm.

## RNrm

Find the matrix row norm.

You can also enter: RNrm.

## Rows→

Build a matrix from supplied row vectors.

You can also enter: Rows→, Row→.

Try it
[ 1 2 ]
[ 4 5 ]
[ 7 8 ]
3 RowsToMatrix

You should get: [[ 1 2 ]
  [ 4 5 ]
  [ 7 8 ]]
Run from the calculator stack. This example uses the current calculation settings.

## Row→

Build a matrix from supplied row vectors.

You can also enter: Rows→, Row→.

Try it
[ 1 2 ]
[ 4 5 ]
[ 7 8 ]
3 RowsToMatrix

You should get: [[ 1 2 ]
  [ 4 5 ]
  [ 7 8 ]]
Run from the calculator stack. This example uses the current calculation settings.

## RSwp

Exchange two specified matrix rows.

You can also enter: RSwp.

Try it
[[1 2][3 4]] 1 2 RowSwap

You should get: [[ 3 4 ]
  [ 1 2 ]]

## RowReducedEchelonForm

Reduce a matrix to reduced row echelon form.

You can also enter: RowReducedEchelonForm.

Try it
[[1 2][3 4]] RREF

You should get: [[ 1 0 ]
  [ 0 1 ]]

## RowReducedEchelonFormModulo

Modulo row reduction is not implemented in this engine build and reports an error. Use REF or RREF for ordinary row reduction.

You can also enter: RowReducedEchelonFormModulo.

## RowReducedEchelonFormPivots

Reduce a matrix and also return its pivot values. The reduced matrix is on top of the stack, with the pivot list below it.

You can also enter: RowReducedEchelonFormPivots, RREFPivots.

Try it
[[2 1][3 4]] RREFP 2 ToList

You should get: {
	{ 10 5 }
	[[ 10 0 ]
	  [ 0 5 ]]
}
Run from the calculator stack. This example uses the current calculation settings.

## RREFPivots

Reduce a matrix and also return its pivot values. The reduced matrix is on top of the stack, with the pivot list below it.

You can also enter: RowReducedEchelonFormPivots, RREFPivots.

Try it
[[2 1][3 4]] RREFP 2 ToList

You should get: {
	{ 10 5 }
	[[ 10 0 ]
	  [ 0 5 ]]
}
Run from the calculator stack. This example uses the current calculation settings.

## →2D

Build a two-component vector from supplied values.

You can also enter: →2D, →V2.

Try it
3 4 To2DVector

You should get: [ 3 4 ]
Run from the calculator stack. This example uses the current calculation settings.

## →V2

Build a two-component vector from supplied values.

You can also enter: →2D, →V2.

Try it
3 4 To2DVector

You should get: [ 3 4 ]
Run from the calculator stack. This example uses the current calculation settings.

## →3D

Build a three-component vector from supplied values.

You can also enter: →3D, →V3.

Try it
3 4 5 To3DVector

You should get: [ 3 4 5 ]
Run from the calculator stack. This example uses the current calculation settings.

## →V3

Build a three-component vector from supplied values.

You can also enter: →3D, →V3.

Try it
3 4 5 To3DVector

You should get: [ 3 4 5 ]
Run from the calculator stack. This example uses the current calculation settings.

## →Arry

Build an array from the supplied values and dimensions.

You can also enter: →Arry, →Array.

Try it
1 2 3 { 3 } ToArray

You should get: [ 1 2 3 ]

## →Array

Build an array from the supplied values and dimensions.

You can also enter: →Arry, →Array.

Try it
1 2 3 { 3 } ToArray

You should get: [ 1 2 3 ]

## →Cylindrical

Convert coordinates to cylindrical form.

You can also enter: →Cylindrical.

## →Polar

Convert a complex value or coordinate object to polar form.

You can also enter: →Polar.

## →Rectangular

Convert a complex value or coordinate object to rectangular form.

You can also enter: →Rectangular, →Cartesian.

Try it
Deg 5 90 RealToPolar ToRectangular

You should get: 5ⅈ
Run from the calculator stack. This example uses the current calculation settings.

## →Cartesian

Convert a complex value or coordinate object to rectangular form.

You can also enter: →Rectangular, →Cartesian.

Try it
Deg 5 90 RealToPolar ToRectangular

You should get: 5ⅈ
Run from the calculator stack. This example uses the current calculation settings.

## →Spherical

Convert coordinates to spherical form.

You can also enter: →Spherical.

## Trn

Transpose a matrix and conjugate its entries.

You can also enter: Trn.

## Tran

Exchange matrix rows and columns.

You can also enter: Tran.

Try it
[[1 2][3 4]] TRN

You should get: [[ 1 3 ]
  [ 2 4 ]]

## Date+

Add the supplied number of days to a date.

You can also enter: Date+.

## JDN→

Convert a Julian day number to a date.

You can also enter: JDN→.

## DDays

Find the difference between supplied dates.

You can also enter: DDays, Date-, DaysBetweenDates.

Try it
20250101 20240101 DDAYS

You should get: 366 d
Run from the calculator stack. This example uses the current calculation settings.

## Date-

Find the difference between supplied dates.

You can also enter: DDays, Date-, DaysBetweenDates.

Try it
20250101 20240101 DDAYS

You should get: 366 d
Run from the calculator stack. This example uses the current calculation settings.

## DaysBetweenDates

Find the difference between supplied dates.

You can also enter: DDays, Date-, DaysBetweenDates.

Try it
20250101 20240101 DDAYS

You should get: 366 d
Run from the calculator stack. This example uses the current calculation settings.

## Degrees→Radians

Convert a plain degree value to radians.

You can also enter: Degrees→Radians, D→R.

Try it
180 DegreesToRadians

You should get: 3.14159 26535 9
A decimal approximation of pi radians.

## D→R

Convert a plain degree value to radians.

You can also enter: Degrees→Radians, D→R.

Try it
180 DegreesToRadians

You should get: 3.14159 26535 9
A decimal approximation of pi radians.

## DMS+

Add two degrees/minutes/seconds values.

You can also enter: DMS+.

## DMS-

Subtract degrees/minutes/seconds values.

You can also enter: DMS-.

## DMS→

Convert degrees/minutes/seconds to decimal degrees.

You can also enter: DMS→.

Try it
1.3 FromDMS

You should get: 1 ¹/₂

## HMS→

Convert hours/minutes/seconds form to decimal hours.

You can also enter: HMS→.

Try it
1.3 FromHMS

You should get: 1 ¹/₂

## HMS+

Add two hours/minutes/seconds values.

You can also enter: HMS+.

## HMS-

Subtract hours/minutes/seconds values.

You can also enter: HMS-.

## JDN

Convert a date to its Julian day number.

You can also enter: JDN.

Try it
20000101.120000 JDN

You should get: 2 451 545
Run from the calculator stack. This example uses the current calculation settings.

## Radians→Degrees

Convert a plain radian value to degrees.

You can also enter: Radians→Degrees, R→D.

Try it
1 RadiansToDegrees

You should get: 57.29577 95131
One radian is approximately 57.2958 degrees.

## R→D

Convert a plain radian value to degrees.

You can also enter: Radians→Degrees, R→D.

Try it
1 RadiansToDegrees

You should get: 57.29577 95131
One radian is approximately 57.2958 degrees.

## →Date

Set the calculator date from the supplied date value. Settings > Clock / timezone offers the native clock options.

You can also enter: →Date.

## →Time

Set the calculator time from the supplied time value. Use actual current time for HTTPS rate updates.

You can also enter: →Time.

## →Degrees

Express an angle in degrees.

You can also enter: →Degrees, →Deg.

## →Deg

Express an angle in degrees.

You can also enter: →Degrees, →Deg.

## →DMS

Convert an angle to degrees/minutes/seconds form.

You can also enter: →DMS.

Try it
1.5 ToDMS

You should get: 1°30′00″

## →Grads

Express an angle in grads: 400 grads make a full turn.

You can also enter: →Grads, →Grad.

## →Grad

Express an angle in grads: 400 grads make a full turn.

You can also enter: →Grads, →Grad.

## →HMS

Convert hours to hours/minutes/seconds form.

You can also enter: →HMS.

Try it
1.5 ToHMS

You should get: 1:30:00

## →PiRadians

Express an angle as a multiple of pi radians: 2 makes a full turn.

You can also enter: →PiRadians, →πr.

## →πr

Express an angle as a multiple of pi radians: 2 makes a full turn.

You can also enter: →PiRadians, →πr.

## →Radians

Express an angle in radians.

You can also enter: →Radians, →Rad.

## →Rad

Express an angle in radians.

You can also enter: →Radians, →Rad.

## cos⁻¹

Find the inverse cosine of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: cos⁻¹, arccos.

Try it
Deg 1 ACOS

You should get: 0. °

## arccos

Find the inverse cosine of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: cos⁻¹, arccos.

Try it
Deg 1 ACOS

You should get: 0. °

## cosh⁻¹

Calculate the inverse hyperbolic cosine for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: cosh⁻¹.

Try it
1 ACOSH

You should get: 0.

## cot⁻¹

Find the inverse cotangent of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: cot⁻¹, arccot.

Try it
Deg 1 ACOT

You should get: 45. °

## arccot

Find the inverse cotangent of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: cot⁻¹, arccot.

Try it
Deg 1 ACOT

You should get: 45. °

## coth⁻¹

Calculate the inverse hyperbolic cotangent for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: coth⁻¹.

## csc⁻¹

Find the inverse cosecant of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: csc⁻¹, arccsc.

Try it
Deg 1 ACSC

You should get: 90. °

## arccsc

Find the inverse cosecant of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: csc⁻¹, arccsc.

Try it
Deg 1 ACSC

You should get: 90. °

## csch⁻¹

Calculate the inverse hyperbolic cosecant for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: csch⁻¹.

## sec⁻¹

Find the inverse secant of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: sec⁻¹, arcsec.

Try it
Deg 1 ASEC

You should get: 0. °

## arcsec

Find the inverse secant of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: sec⁻¹, arcsec.

Try it
Deg 1 ASEC

You should get: 0. °

## sech⁻¹

Calculate the inverse hyperbolic secant for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: sech⁻¹.

## sin⁻¹

Find the inverse sine of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: sin⁻¹, arcsin.

Try it
Deg 1 ASIN

You should get: 90. °

## arcsin

Find the inverse sine of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: sin⁻¹, arcsin.

Try it
Deg 1 ASIN

You should get: 90. °

## sinh⁻¹

Calculate the inverse hyperbolic sine for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: sinh⁻¹.

Try it
0 ASINH

You should get: 0.

## tan⁻¹

Find the inverse tangent of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: tan⁻¹, arctan.

Try it
Deg 0 ATAN

You should get: 0. °

## arctan

Find the inverse tangent of the top value. Real angles follow the calculator angle mode; choose Degrees or Radians before calculating.

You can also enter: tan⁻¹, arctan.

Try it
Deg 0 ATAN

You should get: 0. °

## ∠

Find the angle of a two-component direction using the engine two-argument arctangent; the result follows the current angle mode.

You can also enter: ∠.

## tanh⁻¹

Calculate the inverse hyperbolic tangent for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: tanh⁻¹.

Try it
0 ATANH

You should get: 0.

## Combinations

Count unordered selections. Supply population size n followed by selection size r.

You can also enter: Combinations.

Try it
5 2 COMB

You should get: 10

## alog

Calculate the exponential 10^x for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: alog.

Try it
2 ALOG

You should get: 100.

## alog2

Calculate the exponential 2^x for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: alog2.

Try it
3 EXP2

You should get: 8.

## expm

Calculate the value of e^x-1 for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: expm.

Try it
0 EXPM

You should get: 0.

## x!

Find the factorial. Noninteger arguments follow the engine's gamma-function extension where supported. On the calculator stack, press ! with an empty editor to apply factorial immediately. Typing 6! also calculates immediately, without Enter. Inside programs and quoted algebra, ! remains notation.

You can also enter: x!, factorial, !.

Try it
5 FACT

You should get: 120

## factorial

Find the factorial. Noninteger arguments follow the engine's gamma-function extension where supported. On the calculator stack, press ! with an empty editor to apply factorial immediately. Typing 6! also calculates immediately, without Enter. Inside programs and quoted algebra, ! remains notation.

You can also enter: x!, factorial, !.

Try it
5 FACT

You should get: 120

## !

Find the factorial. Noninteger arguments follow the engine's gamma-function extension where supported. On the calculator stack, press ! with an empty editor to apply factorial immediately. Typing 6! also calculates immediately, without Enter. Inside programs and quoted algebra, ! remains notation.

You can also enter: x!, factorial, !.

Try it
5 FACT

You should get: 120

## lnΓ

Calculate the logarithm of the gamma function for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted. Complex arguments are not implemented for this function.

You can also enter: lnΓ, LnGamma.

Try it
1 LGAMMA

You should get: 0.

## LnGamma

Calculate the logarithm of the gamma function for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted. Complex arguments are not implemented for this function.

You can also enter: lnΓ, LnGamma.

Try it
1 LGAMMA

You should get: 0.

## lnp1

Calculate the natural logarithm of 1+x for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: lnp1.

Try it
0 LNP1

You should get: 0.

## log

Calculate the base-ten logarithm for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted.

You can also enter: log.

Try it
100 LOG

You should get: 2.

## Permutations

Count ordered selections. Supply population size n followed by selection size r.

You can also enter: Permutations.

Try it
5 2 PERM

You should get: 20

## ?

Sum the integers from 1 through n. Accepts nonnegative whole numbers, including zero. Uses n*(n+1)/2 and returns an exact integer; negative or fractional inputs report a domain error. Enter 6? or 6 Termial to get 21. With no entry being edited, press ? to apply Termial immediately to the top stack value. Typing 6? also calculates immediately without Enter; inside programs and quoted algebra, ? remains notation. In algebraic expressions, N? is postfix Termial(N). In RPL, use N ? or N Termial after a variable; question marks in ordinary RPL identifiers remain valid. Explicit names such as ⓋREADY? and the constant Ⓒ? keep their existing meanings.

You can also enter: ?.

Try it
6?

You should get: 21
Sum 1+2+3+4+5+6.

## Γ

Calculate the gamma function for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted. Complex arguments are not implemented for this function.

You can also enter: Γ, Gamma.

Try it
5 TGAMMA

You should get: 24.

## Gamma

Calculate the gamma function for the top stack value. Its real domain and the real/complex result setting determine which inputs are accepted. Complex arguments are not implemented for this function.

You can also enter: Γ, Gamma.

Try it
5 TGAMMA

You should get: 24.

## BarPlot

Select engine bar-chart plotting.

You can also enter: BarPlot.

## Blank

Create a blank graphics object with supplied dimensions.

You can also enter: Blank.

## PlotCenter

Set the engine plot centre.

You can also enter: PlotCenter, Centr.

## Centr

Set the engine plot centre.

You can also enter: PlotCenter, Centr.

## DrawCircle

Draw a circle using the supplied centre and radius in the engine graphics view.

You can also enter: DrawCircle.

## ClearLCD

Clear the engine graphics display.

You can also enter: ClearLCD.

## ColorPattern

Convert the supplied colour specification to an engine drawing pattern.

You can also enter: ColorPattern.

## DependentVariable

Choose the engine plot's dependent variable.

You can also enter: DependentVariable, DEPND.

## DrawText

Display supplied text or an object at the requested engine display line.

You can also enter: DrawText.

## DrawStyledText

Draw supplied text or an object at the requested graphics position.

You can also enter: DrawStyledText.

## DrawPlot

Draw the current engine equation using its plot parameters. For LumaRPL's interactive graph tools, press Ctrl+G.

You can also enter: DrawPlot.

## DrawAxes

Draw axes in the engine graphics view using its plot parameters.

You can also enter: DrawAxes.

## DrawEllipse

Draw an ellipse using the supplied bounds in the engine graphics view.

You can also enter: DrawEllipse.

## LCD→

Capture calculator display pixels as a graphics object.

You can also enter: LCD→, Screen→.

## Screen→

Capture calculator display pixels as a graphics object.

You can also enter: LCD→, Screen→.

## FunctionPlot

Select function plotting in the engine plot parameters.

You can also enter: FunctionPlot.

## GraphicAnd

Combine graphics objects with AND.

You can also enter: GraphicAnd.

## GraphicOr

Combine graphics objects with OR.

You can also enter: GraphicOr.

## GrobAdd

Place graphics objects above one another.

You can also enter: GrobAdd.

## GrayPattern

Build a greyscale drawing pattern from the supplied intensity.

You can also enter: GrayPattern, GreyPattern, Grey.

## GreyPattern

Build a greyscale drawing pattern from the supplied intensity.

You can also enter: GrayPattern, GreyPattern, Grey.

## Grey

Build a greyscale drawing pattern from the supplied intensity.

You can also enter: GrayPattern, GreyPattern, Grey.

## GraphicXor

Combine graphics objects with exclusive OR.

You can also enter: GraphicXor.

## HistogramPlot

Select engine histogram plotting.

You can also enter: HistogramPlot.

## HSVPattern

Build a drawing pattern from hue, saturation and value.

You can also enter: HSVPattern.

## IndependentVariable

Choose the engine plot's independent variable.

You can also enter: IndependentVariable, INDEP.

## DrawLine

Draw between two supplied positions in the engine graphics view.

You can also enter: DrawLine.

## ParametricPlot

Select parametric plotting in the engine plot parameters.

You can also enter: ParametricPlot.

## GraphicPicture

Recall the engine picture graphics object.

You can also enter: GraphicPicture.

## PixelColor

Return the colour at the supplied graphics position.

You can also enter: PixelColor, PixC?.

## PixC?

Return the colour at the supplied graphics position.

You can also enter: PixelColor, PixC?.

## DrawBackgroundPixel

Clear the pixel at the supplied graphics position using the background pattern.

You can also enter: DrawBackgroundPixel.

## DrawPixel

Set the pixel at the supplied graphics position using the foreground pattern.

You can also enter: DrawPixel.

## PixelGrayLevel

Test the pixel at the supplied graphics position.

You can also enter: PixelGrayLevel, Pix?.

## Pix?

Test the pixel at the supplied graphics position.

You can also enter: PixelGrayLevel, Pix?.

## PMax

Set the engine plot's maximum corner.

You can also enter: PMax.

## PMin

Set the engine plot's minimum corner.

You can also enter: PMin.

## PPar

Read or set the engine plot parameters using the supplied plot-parameter object.

You can also enter: PPar.

## PolarPlot

Select polar plotting in the engine plot parameters.

You can also enter: PolarPlot.

## DrawRectangle

Draw a rectangle between supplied corners.

You can also enter: DrawRectangle.

## Resolution

Set the engine plot resolution.

You can also enter: Resolution.

## RGBPattern

Build a drawing pattern from red, green and blue components. Foreground and Background accept drawing patterns; themes are selected under Settings > Display.

You can also enter: RGBPattern.

## DrawRoundedRectangle

Draw a rounded rectangle using supplied bounds and corner radius.

You can also enter: DrawRoundedRectangle.

## PlotScale

Set both engine plot-axis scales.

You can also enter: PlotScale.

## ScatterPlot

Select engine scatter plotting from engine statistical data.

You can also enter: ScatterPlot.

## →Bitmap

Convert to a packed monochrome bitmap.

You can also enter: →Bitmap.

## →Grob

Render the supplied object into a graphics object.

You can also enter: →Grob.

## →HPGrob

Convert to an HP-compatible graphics object.

You can also enter: →HPGrob.

## →LCD

Copy a graphics object to the calculator graphics display.

You can also enter: →LCD, →Screen.

## →Screen

Copy a graphics object to the calculator graphics display.

You can also enter: →LCD, →Screen.

## →Pixmap

Convert to a colour pixmap.

You can also enter: →Pixmap.

## TruthPlot

Select truth plotting in the engine plot parameters.

You can also enter: TruthPlot.

## PlotXRange

Set the engine horizontal plot interval.

You can also enter: PlotXRange, XRng.

## XRng

Set the engine horizontal plot interval.

You can also enter: PlotXRange, XRng.

## PlotScaleX

Set the engine horizontal plot scale.

You can also enter: PlotScaleX, ScaleX, ScaleH.

## ScaleX

Set the engine horizontal plot scale.

You can also enter: PlotScaleX, ScaleX, ScaleH.

## ScaleH

Set the engine horizontal plot scale.

You can also enter: PlotScaleX, ScaleX, ScaleH.

## PlotYRange

Set the engine vertical plot interval.

You can also enter: PlotYRange, YRng.

## YRng

Set the engine vertical plot interval.

You can also enter: PlotYRange, YRng.

## PlotScaleY

Set the engine vertical plot scale.

You can also enter: PlotScaleY, ScaleY, ScaleW.

## ScaleY

Set the engine vertical plot scale.

You can also enter: PlotScaleY, ScaleY, ScaleW.

## ScaleW

Set the engine vertical plot scale.

You can also enter: PlotScaleY, ScaleY, ScaleW.

## Char→Code

Return the Unicode code point for a supplied character.

You can also enter: Char→Code, Codepoint, Num.

Try it
"A" CharToUnicode

You should get: 65

## Codepoint

Return the Unicode code point for a supplied character.

You can also enter: Char→Code, Codepoint, Num.

Try it
"A" CharToUnicode

You should get: 65

## Num

Return the Unicode code point for a supplied character.

You can also enter: Char→Code, Codepoint, Num.

Try it
"A" CharToUnicode

You should get: 65

## Object→

Break an object into its components.

You can also enter: Object→, FromObj, Obj→.

## FromObj

Break an object into its components.

You can also enter: Object→, FromObj, Obj→.

## Obj→

Break an object into its components.

You can also enter: Object→, FromObj, Obj→.

## List→

Unpack a list into its elements and return its size using the engine list convention.

You can also enter: List→.

Try it
{ 1 2 3 } FromList

You should get: 1
2
3
3
The values 1, 2, 3 are pushed, followed by their count, 3.

## GetIteration

Read the indexed element and advance the index using the engine indexed-access convention.

You can also enter: GetIteration.

## ∆List

Return differences between successive list elements.

You can also enter: ∆List.

Try it
{ 3 7 12 } ListDifferences

You should get: { 4 5 }

## ∏List

Multiply the elements of a list.

You can also enter: ∏List.

Try it
{ 2 3 4 } ListProduct

You should get: 24

## ΣList

Add the elements of a list.

You can also enter: ΣList.

Try it
{ 1 2 3 } ListSum

You should get: 6

## PutIteration

Write the indexed element and advance the index using the engine indexed-access convention.

You can also enter: PutIteration.

## QSort

Sort a list in ascending order with the engine quick-sort operation.

You can also enter: QSort.

Try it
{ 3 1 2 } QuickSort

You should get: { 1 2 3 }

## Stream

Combine list elements using the supplied operation.

You can also enter: Stream.

Try it
{ 1 2 3 } « + » REDUCE

You should get: 6

## RevList

Reverse the order of list elements.

You can also enter: RevList.

Try it
{ 1 2 3 } REVLIST

You should get: { 3 2 1 }

## Text→Code

Return the Unicode code points in supplied text.

You can also enter: Text→Code.

Try it
"AB" TextToUnicode

You should get: { 65 66 }

## →List

Collect the requested number of stack values into a list. Supply the count last.

You can also enter: →List.

Try it
1 2 3 3 ToList

You should get: { 1 2 3 }

## →Text

Convert the top object to its source-text representation.

You can also enter: →Text, ToStr, →Str.

Try it
42 ToText

You should get: "42"

## ToStr

Convert the top object to its source-text representation.

You can also enter: →Text, ToStr, →Str.

Try it
42 ToText

You should get: "42"

## →Str

Convert the top object to its source-text representation.

You can also enter: →Text, ToStr, →Str.

Try it
42 ToText

You should get: "42"

## Code→Text

Build text from supplied Unicode code points.

You can also enter: Code→Text, Code→Char, Chr.

Try it
65 UnicodeToText

You should get: "A"

## Code→Char

Build text from supplied Unicode code points.

You can also enter: Code→Text, Code→Char, Chr.

Try it
65 UnicodeToText

You should get: "A"

## Chr

Build text from supplied Unicode code points.

You can also enter: Code→Text, Code→Char, Chr.

Try it
65 UnicodeToText

You should get: "A"

## StoF

Restore engine flag state from a supplied based integer. This can change calculator behavior.

You can also enter: StoF.

## CF

Clear the numbered user flag supplied on the stack.

You can also enter: CF.

## Text→

Parse source text into calculator objects. It does not run a Lua or CHIP-8 file.

You can also enter: Text→, Str→.

Try it
"2 3 +" Compile EVAL

You should get: 5

## Str→

Parse source text into calculator objects. It does not run a Lua or CHIP-8 file.

You can also enter: Text→, Str→.

Try it
"2 3 +" Compile EVAL

You should get: 5

## Text→Algebraic

Parse source text and require an algebraic object.

You can also enter: Text→Algebraic.

## Text→Expression

Parse source text and require an expression.

You can also enter: Text→Expression.

## Text→Integer

Parse source text and require an integer.

You can also enter: Text→Integer.

Try it
"42" CompileToInteger

You should get: 42

## Text→Number

Parse source text and require a number.

You can also enter: Text→Number.

Try it
"42" CompileToNumber

You should get: 42

## Text→Object

Parse source text as one calculator object.

You can also enter: Text→Object.

Try it
"{ 1 2 }" CompileToObject

You should get: { 1 2 }

## Text→Positive

Parse source text and require a positive value.

You can also enter: Text→Positive.

Try it
"42" CompileToPositive

You should get: 42

## Text→Real

Parse source text and require a real value.

You can also enter: Text→Real.

Try it
"1.25" CompileToReal

You should get: 1.25

## cont

Resume a paused RPL program.

You can also enter: cont.

## dbug

Start the supplied RPL program in the debugger.

You can also enter: dbug.

## RclF

Pack the engine flag state into a based integer for storage or inspection.

You can also enter: RclF.

## FF

Toggle the numbered user flag supplied on the stack.

You can also enter: FF.

## IfThen

Run the supplied branch only when the supplied condition is true.

You can also enter: IfThen.

Try it
True « 7 » IFT

You should get: 7

## IfThenElse

Choose between two branches from a condition. Supply the condition, true branch and false branch in that order.

You can also enter: IfThenElse.

Try it
False « 7 » « 9 » IFTE

You should get: 9

## SF

Set the numbered user flag supplied on the stack.

You can also enter: SF.

## sst

Execute the next instruction of a paused RPL program.

You can also enter: sst.

## =

Test whether the top two values are equal.

You can also enter: =.

Try it
3 3 TestEQ

You should get: True

## FC?

Test whether the supplied user flag is clear.

You can also enter: FC?.

## FC?C

Test whether a user flag was clear, then leave it clear.

You can also enter: FC?C.

## FC?S

Test whether a user flag was clear, then set it.

You can also enter: FC?S.

## FS?

Test whether the supplied user flag is set.

You can also enter: FS?.

## FS?C

Test whether a user flag was set, then clear it.

You can also enter: FS?C.

## FS?S

Test whether a user flag was set, then leave it set.

You can also enter: FS?S.

## ≥

Test whether the second stack value is greater than or equal to the top value.

You can also enter: ≥.

## >

Test whether the second stack value is greater than the top value.

You can also enter: >.

Try it
3 2 >

You should get: True

## ≤

Test whether the second stack value is less than or equal to the top value.

You can also enter: ≤.

## <

Test whether the second stack value is less than the top value.

You can also enter: <.

Try it
2 3 <

You should get: True

## ≠

Test whether the top two values differ.

You can also enter: ≠.

Try it
2 3 TestNE

You should get: True

## ==

Test whether the supplied objects are the same, using the engine comparison operation.

You can also enter: ==.

Try it
3 3 TestSame

You should get: True

## TEval

Evaluate an object and report its elapsed time.

You can also enter: TEval.

## R→I

Convert a supported object to an integer.

You can also enter: R→I, →Integer, →Int.

## →Integer

Convert a supported object to an integer.

You can also enter: R→I, →Integer, →Int.

## →Int

Convert a supported object to an integer.

You can also enter: R→I, →Integer, →Int.

## →Program

Turn the supplied object into an RPL program.

You can also enter: →Program, →Prg.

Try it
{ 2 3 + } ToProgram EVAL

You should get: 5

## →Prg

Turn the supplied object into an RPL program.

You can also enter: →Program, →Prg.

Try it
{ 2 3 + } ToProgram EVAL

You should get: 5

## |

Apply the supplied local assignments to an expression.

You can also enter: |.

## Modulus

Find the absolute value, or the magnitude of a complex value.

You can also enter: Modulus.

Try it
-7 ABS

You should get: 7

## +

Add the top two values. Type 2 3 + and press Enter to get 5.

You can also enter: +.

Try it
2 3 +

You should get: 5

## B→R

Convert a based integer to a real-number object.

You can also enter: B→R.

Try it
#FFh B→R

You should get: 255

## ∛

Find the cube root of the top value.

You can also enter: ∛, CubeRoot.

Try it
27 CBRT

You should get: 3.

## CubeRoot

Find the cube root of the top value.

You can also enter: ∛, CubeRoot.

Try it
27 CBRT

You should get: 3.

## x³

Cube the top value. Example: 3 cubed gives 27.

You can also enter: x³.

Try it
3 CUBED

You should get: 27

## ContinuedFraction

Convert a decimal value to a continued-fraction representation.

You can also enter: ContinuedFraction.

## ContinuedFractionToFraction

Convert a continued-fraction representation to a fraction.

You can also enter: ContinuedFractionToFraction.

## QuoRem

Perform integer division and return the quotient and remainder.

You can also enter: QuoRem, IDiv2, QuotientRemainder.

## IDiv2

Perform integer division and return the quotient and remainder.

You can also enter: QuoRem, IDiv2, QuotientRemainder.

## QuotientRemainder

Perform integer division and return the quotient and remainder.

You can also enter: QuoRem, IDiv2, QuotientRemainder.

## ÷

Divide the second value by the top value.

Try it
1 3 /

You should get: ¹/₃

## /

Divide the second value by the top value.

Try it
1 3 /

You should get: ¹/₃

## ℮

Put Euler's number e on the stack. Symbolic/numerical constant settings affect its form.

You can also enter: ℮.

## Sub

Extract the requested part of an object using the supplied selection information.

You can also enter: Sub.

## FractionalPart

Keep the fractional remainder after truncation toward zero.

You can also enter: FractionalPart, FP.

Try it
3.75 FP

You should get: 0.75

## FP

Keep the fractional remainder after truncation toward zero.

You can also enter: FractionalPart, FP.

Try it
3.75 FP

You should get: 0.75

## GreatestCommonDenominator

Find the greatest common divisor of two integers.

You can also enter: GreatestCommonDenominator.

Try it
48 18 GCD

You should get: 6

## ⊿

Find sqrt(x^2+y^2) from two supplied values.

You can also enter: ⊿, Hypothenuse.

Try it
3 4 HYPOT

You should get: 5.

## Hypothenuse

Find sqrt(x^2+y^2) from two supplied values.

You can also enter: ⊿, Hypothenuse.

Try it
3 4 HYPOT

You should get: 5.

## ∞

Put positive infinity on the stack.

You can also enter: ∞.

Try it
infinity

You should get: ∞
Run from the calculator stack. This example uses the current calculation settings.

## IntegerPart

Keep the integer part, truncating toward zero.

You can also enter: IntegerPart, IP.

Try it
3.75 IP

You should get: 3.

## IP

Keep the integer part, truncating toward zero.

You can also enter: IntegerPart, IP.

Try it
3.75 IP

You should get: 3.

## x⁻¹

Find the reciprocal of the top value: 1 divided by that value. Zero has no finite reciprocal.

You can also enter: x⁻¹, Invert.

Try it
4 INV

You should get: ¹/₄

## Invert

Find the reciprocal of the top value: 1 divided by that value. Zero has no finite reciprocal.

You can also enter: x⁻¹, Invert.

Try it
4 INV

You should get: ¹/₄

## LeastCommonMultiple

Find the least common multiple of two integers.

You can also enter: LeastCommonMultiple.

Try it
6 8 LCM

You should get: 24

## Mantissa

Return the decimal mantissa of a number.

You can also enter: Mantissa.

Try it
1234 MANT

You should get: 1.234

## ×

Multiply the top two values.

Try it
6 7 *

You should get: 42

## ·

Multiply the top two values.

Try it
6 7 *

You should get: 42

## *

Multiply the top two values.

Try it
6 7 *

You should get: 42

## Negate

Change the sign of the top value.

You can also enter: Negate.

Try it
7 NEG

You should get: -7

## −∞

Put negative infinity on the stack.

You can also enter: −∞.

Try it
NegativeInfinity

You should get: −∞
Run from the calculator stack. This example uses the current calculation settings.

## NextPr

Find the next prime after the supplied integer.

You can also enter: NextPr.

Try it
17 NEXTPRIME

You should get: 19

## %

Calculate a percentage of the value below it. The base amount goes first, then the percentage.

You can also enter: %.

Try it
200 15 %

You should get: 30

## %Change

Calculate percentage change from the first value to the second.

You can also enter: %Change, %Ch.

Try it
100 125 %CH

You should get: 25

## %Ch

Calculate percentage change from the first value to the second.

You can also enter: %Change, %Ch.

Try it
100 125 %CH

You should get: 25

## %Total

Calculate the second amount as a percentage of the first total.

You can also enter: %Total, %T.

Try it
200 50 %T

You should get: 25

## %T

Calculate the second amount as a percentage of the first total.

You can also enter: %Total, %T.

Try it
200 50 %T

You should get: 25

## π

Put the mathematical constant pi on the stack. Symbolic/numerical constant settings affect its form.

You can also enter: π.

## ↑

Raise the second value to the power on top. Example: 2 3 ^ gives 8. On the calculator stack, a complete numeric entry such as 2^3 also gives 8 on Enter. 0^0 gives 1 by default; the advanced ZeroPowerZeroIsUndefined setting explicitly selects undefined behavior.

You can also enter: ↑, Power, ^.

Try it
2 8 ^

You should get: 256

## Power

Raise the second value to the power on top. Example: 2 3 ^ gives 8. On the calculator stack, a complete numeric entry such as 2^3 also gives 8 on Enter. 0^0 gives 1 by default; the advanced ZeroPowerZeroIsUndefined setting explicitly selects undefined behavior.

You can also enter: ↑, Power, ^.

Try it
2 8 ^

You should get: 256

## ^

Raise the second value to the power on top. Example: 2 3 ^ gives 8. On the calculator stack, a complete numeric entry such as 2^3 also gives 8 on Enter. 0^0 gives 1 by default; the advanced ZeroPowerZeroIsUndefined setting explicitly selects undefined behavior.

You can also enter: ↑, Power, ^.

Try it
2 8 ^

You should get: 256

## PrevPr

Find the preceding prime before the supplied integer.

You can also enter: PrevPr.

Try it
17 PreviousPrime

You should get: 13

## rand

Generate a random value from 0 up to, but not including, 1.

You can also enter: rand.

## rdz

Set the random-number generator seed. Reusing a seed is useful when reproducing a calculation.

You can also enter: rdz.

## R→B

Convert a real integer to a based-number object. Base and WordSize control its representation and fixed-width operations.

You can also enter: R→B.

Try it
255 R→B

You should get: #FF₁₆

## Rnd

Round a value using the supplied digit count. The sign of the digit count selects the engine's rounding convention.

You can also enter: Rnd.

Try it
1.234 2 RND

You should get: 1.23

## SignificantDigits

Count the significant decimal digits of a value.

You can also enter: SignificantDigits.

## x²

Square the top value.

Try it
12 SQ

You should get: 144

## Square

Square the top value.

Try it
12 SQ

You should get: 144

## √

Find the square root. The real/complex result setting determines whether negative real inputs can give complex results.

You can also enter: √, SquareRoot.

Try it
81 SQRT

You should get: 9.

## SquareRoot

Find the square root. The real/complex result setting determines whether negative real inputs can give complex results.

You can also enter: √, SquareRoot.

Try it
81 SQRT

You should get: 9.

## -

Subtract the top value from the value below it.

Try it
10 3 -

You should get: 7

## →Decimal

Convert the top value to a decimal approximation using the current precision.

You can also enter: →Decimal, →Number, →Num.

Try it
1 4 / ToDecimal

You should get: 0.25

## →Number

Convert the top value to a decimal approximation using the current precision.

You can also enter: →Decimal, →Number, →Num.

Try it
1 4 / ToDecimal

You should get: 0.25

## →Num

Convert the top value to a decimal approximation using the current precision.

You can also enter: →Decimal, →Number, →Num.

Try it
1 4 / ToDecimal

You should get: 0.25

## →ℚ

Convert a value to a fraction using the fraction-conversion settings.

You can also enter: →ℚ, →Frac, →Q.

Try it
0.5 ToFraction

You should get: ¹/₂

## →Frac

Convert a value to a fraction using the fraction-conversion settings.

You can also enter: →ℚ, →Frac, →Q.

Try it
0.5 ToFraction

You should get: ¹/₂

## →Q

Convert a value to a fraction using the fraction-conversion settings.

You can also enter: →ℚ, →Frac, →Q.

Try it
0.5 ToFraction

You should get: ¹/₂

## →ℚπ

Rewrite a value or expression as a quotient where supported.

You can also enter: →ℚπ, →QPi, →Qπ, XQ.

Try it
0.5 ToQuotient

You should get: ¹/₂

## →QPi

Rewrite a value or expression as a quotient where supported.

You can also enter: →ℚπ, →QPi, →Qπ, XQ.

Try it
0.5 ToQuotient

You should get: ¹/₂

## →Qπ

Rewrite a value or expression as a quotient where supported.

You can also enter: →ℚπ, →QPi, →Qπ, XQ.

Try it
0.5 ToQuotient

You should get: ¹/₂

## XQ

Rewrite a value or expression as a quotient where supported.

You can also enter: →ℚπ, →QPi, →Qπ, XQ.

Try it
0.5 ToQuotient

You should get: ¹/₂

## Trnc

Discard digits using the supplied digit count instead of rounding them.

You can also enter: Trnc.

Try it
1.239 2 TRNC

You should get: 1.23

## Exponent

Return the decimal exponent of a number.

You can also enter: Exponent.

Try it
1234 XPON

You should get: 3

## Clear

Remove every value from the stack. Save anything important before using this command.

You can also enter: Clear.

Try it
1 2 3 CLEAR DEPTH ToList

You should get: { }
The list shows the remaining stack from deepest value to top.

## NewObject

Make an independent copy of the top object.

You can also enter: NewObject, NewObj, NewOb.

## NewObj

Make an independent copy of the top object.

You can also enter: NewObject, NewObj, NewOb.

## NewOb

Make an independent copy of the top object.

You can also enter: NewObject, NewObj, NewOb.

## ▶

Copy a named object between calculator directories using the supplied source and destination names.

You can also enter: ▶.

## CreateDirectory

Create a calculator variable directory with the supplied name.

You can also enter: CreateDirectory, MkDir.

## MkDir

Create a calculator variable directory with the supplied name.

You can also enter: CreateDirectory, MkDir.

## Decr

Decrease the supplied value or named variable by one.

You can also enter: Decr.

## Duplicate

Make a second copy of the top value. Example: 7 DUP leaves 7 and 7.

You can also enter: Duplicate.

Try it
7 DUP DEPTH ToList

You should get: { 7 7 }
The list shows the remaining stack from deepest value to top.

## Duplicate2

Copy the top pair of values, keeping their order.

You can also enter: Duplicate2.

Try it
2 3 DUP2 DEPTH ToList

You should get: { 2 3 2 3 }
The list shows the remaining stack from deepest value to top.

## DuplicateTwice

Duplicate the top value twice, leaving three copies.

You can also enter: DuplicateTwice.

Try it
7 DUPDUP DEPTH ToList

You should get: { 7 7 7 }
The list shows the remaining stack from deepest value to top.

## DuplicateN

Copy a group of values. Put the size of the group on top first.

You can also enter: DuplicateN.

Try it
2 3 2 DUPN DEPTH ToList

You should get: { 2 3 2 3 }
The list shows the remaining stack from deepest value to top.

## Evaluate

Evaluate the top object. For a program, this runs its instructions.

Try it
« 2 3 + » EVAL

You should get: 5

## HomeDirectory

Return to the HOME calculator directory.

You can also enter: HomeDirectory.

## Incr

Increase the supplied value or named variable by one.

You can also enter: Incr.

## LastArguments

Recall the arguments saved from the previous command. Argument-saving settings determine what is available.

You can also enter: LastArguments.

## ListExpressionNames

Find names used in the supplied object.

You can also enter: ListExpressionNames.

## NDuplicateN

Repeat a group of stack values using the supplied count.

You can also enter: NDuplicateN.

## DirectoryPath

Return the path through calculator variable directories to the current directory.

You can also enter: DirectoryPath.

## PurgeDirectory

Purge the named calculator directory and its contents.

You can also enter: PurgeDirectory, RmDir.

## RmDir

Purge the named calculator directory and its contents.

You can also enter: PurgeDirectory, RmDir.

## Recall

Fetch a stored object without running it: type 'ANSWER' RCL. A recalled program stays on the stack until you run EVAL.

You can also enter: Recall.

Try it
42 'DEMO' STO 'DEMO' RCL

You should get: 42
Stores a HOME variable named DEMO; choose another name if it already exists.

## Recall+

Recall the named value and combine it with a stack value using add arithmetic, without storing the result back.

You can also enter: Recall+, Rcl+.

## Rcl+

Recall the named value and combine it with a stack value using add arithmetic, without storing the result back.

You can also enter: Recall+, Rcl+.

## Recall÷

Recall the named value and combine it with a stack value using divide arithmetic, without storing the result back.

You can also enter: Recall÷, Recall/, Rcl/, Rcl÷.

## Recall/

Recall the named value and combine it with a stack value using divide arithmetic, without storing the result back.

You can also enter: Recall÷, Recall/, Rcl/, Rcl÷.

## Rcl/

Recall the named value and combine it with a stack value using divide arithmetic, without storing the result back.

You can also enter: Recall÷, Recall/, Rcl/, Rcl÷.

## Rcl÷

Recall the named value and combine it with a stack value using divide arithmetic, without storing the result back.

You can also enter: Recall÷, Recall/, Rcl/, Rcl÷.

## Recall×

Recall the named value and combine it with a stack value using multiply arithmetic, without storing the result back.

You can also enter: Recall×, Recall*, Rcl*, Rcl×.

## Recall*

Recall the named value and combine it with a stack value using multiply arithmetic, without storing the result back.

You can also enter: Recall×, Recall*, Rcl*, Rcl×.

## Rcl*

Recall the named value and combine it with a stack value using multiply arithmetic, without storing the result back.

You can also enter: Recall×, Recall*, Rcl*, Rcl×.

## Rcl×

Recall the named value and combine it with a stack value using multiply arithmetic, without storing the result back.

You can also enter: Recall×, Recall*, Rcl*, Rcl×.

## Recall-

Recall the named value and combine it with a stack value using subtract arithmetic, without storing the result back.

You can also enter: Recall-, Rcl-.

## Rcl-

Recall the named value and combine it with a stack value using subtract arithmetic, without storing the result back.

You can also enter: Recall-, Rcl-.

## RollDown

Move the top value down to a chosen level. Supply the destination level number first.

You can also enter: RollDown.

Try it
1 2 3 3 ROLLD DEPTH ToList

You should get: { 3 1 2 }
The list shows the remaining stack from deepest value to top.

## Store

Store a value under a name: type 42 'ANSWER' STO and press Enter. To keep HOME variables after restart, save the calculator session.

You can also enter: Store.

Try it
42 'DEMO' STO 'DEMO' RCL

You should get: 42
Stores a HOME variable named DEMO; choose another name if it already exists.

## Store+

Update the named variable using add arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store+, Sto+.

Try it
10 'DEMO' STO 3 'DEMO' STO+ DEMO

You should get: 13

## Sto+

Update the named variable using add arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store+, Sto+.

Try it
10 'DEMO' STO 3 'DEMO' STO+ DEMO

You should get: 13

## Store÷

Update the named variable using divide arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store÷, Store/, Sto/, Sto÷.

Try it
10 'DEMO' STO 2 'DEMO' STO/ DEMO

You should get: ¹/₅
STO/ uses the supplied value divided by the stored value: 2 divided by 10 gives 1/5.

## Store/

Update the named variable using divide arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store÷, Store/, Sto/, Sto÷.

Try it
10 'DEMO' STO 2 'DEMO' STO/ DEMO

You should get: ¹/₅
STO/ uses the supplied value divided by the stored value: 2 divided by 10 gives 1/5.

## Sto/

Update the named variable using divide arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store÷, Store/, Sto/, Sto÷.

Try it
10 'DEMO' STO 2 'DEMO' STO/ DEMO

You should get: ¹/₅
STO/ uses the supplied value divided by the stored value: 2 divided by 10 gives 1/5.

## Sto÷

Update the named variable using divide arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store÷, Store/, Sto/, Sto÷.

Try it
10 'DEMO' STO 2 'DEMO' STO/ DEMO

You should get: ¹/₅
STO/ uses the supplied value divided by the stored value: 2 divided by 10 gives 1/5.

## Store×

Update the named variable using multiply arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store×, Store*, Sto*, Sto×.

Try it
10 'DEMO' STO 3 'DEMO' STO* DEMO

You should get: 30

## Store*

Update the named variable using multiply arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store×, Store*, Sto*, Sto×.

Try it
10 'DEMO' STO 3 'DEMO' STO* DEMO

You should get: 30

## Sto*

Update the named variable using multiply arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store×, Store*, Sto*, Sto×.

Try it
10 'DEMO' STO 3 'DEMO' STO* DEMO

You should get: 30

## Sto×

Update the named variable using multiply arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store×, Store*, Sto*, Sto×.

Try it
10 'DEMO' STO 3 'DEMO' STO* DEMO

You should get: 30

## Store-

Update the named variable using subtract arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store-, Sto-.

Try it
10 'DEMO' STO 3 'DEMO' STO- DEMO

You should get: -7
STO- uses the supplied value minus the stored value: 3 minus 10 gives -7.

## Sto-

Update the named variable using subtract arithmetic with the supplied value. Supply the value and quoted variable name.

You can also enter: Store-, Sto-.

Try it
10 'DEMO' STO 3 'DEMO' STO- DEMO

You should get: -7
STO- uses the supplied value minus the stored value: 3 minus 10 gives -7.

## TypedVariables

List variables of the requested object type in the current directory.

You can also enter: TypedVariables.

## UpDirectory

Move to the parent calculator directory.

You can also enter: UpDirectory.

## Variables

List names in the current calculator directory.

You can also enter: Variables.

## ExpressionVariables

List variables used by the supplied expression.

You can also enter: ExpressionVariables.

## Σ+

Add supplied observations to the engine statistical data.

You can also enter: Σ+.

## ΣMean

Calculate the mean using the engine statistical data or supplied supported object.

You can also enter: ΣMean, Avg, Mean.

Try it
[[1 2][2 4][3 6]] StoreData Average

You should get: [ 2 4 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## Avg

Calculate the mean using the engine statistical data or supplied supported object.

You can also enter: ΣMean, Avg, Mean.

Try it
[[1 2][2 4][3 6]] StoreData Average

You should get: [ 2 4 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## Mean

Calculate the mean using the engine statistical data or supplied supported object.

You can also enter: ΣMean, Avg, Mean.

Try it
[[1 2][2 4][3 6]] StoreData Average

You should get: [ 2 4 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ClearΣ

Clear the engine statistical data.

You can also enter: ClearΣ, ClΣ.

Try it
[[1 2][2 4][3 6]] StoreData ClearData DataSize

You should get: 0
Clears calculator statistics data. Save existing data first.

## ClΣ

Clear the engine statistical data.

You can also enter: ClearΣ, ClΣ.

Try it
[[1 2][2 4][3 6]] StoreData ClearData DataSize

You should get: 0
Clears calculator statistics data. Save existing data first.

## Corr

Return the correlation coefficient for the engine selected data columns.

You can also enter: Corr.

Try it
[[1 2][2 4][3 6]] StoreData Correlation

You should get: 1.
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## Cov

Return sample covariance for the engine selected data columns.

You can also enter: Cov.

Try it
[[1 2][2 4][3 6]] StoreData Covariance

You should get: 2
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣColumns

Configure the statistical columns used by engine analysis.

You can also enter: ΣColumns, ColΣ.

## ColΣ

Configure the statistical columns used by engine analysis.

You can also enter: ΣColumns, ColΣ.

## ΣSize

Return the engine statistical-data size.

You can also enter: ΣSize, NΣ.

Try it
[[1 2][2 4][3 6]] StoreData DataSize

You should get: 3
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## NΣ

Return the engine statistical-data size.

You can also enter: ΣSize, NΣ.

Try it
[[1 2][2 4][3 6]] StoreData DataSize

You should get: 3
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣTotal

Return totals from the engine statistical data.

You can also enter: ΣTotal, Tot.

## Tot

Return totals from the engine statistical data.

You can also enter: ΣTotal, Tot.

## YCol

Choose the dependent statistical-data column.

You can also enter: YCol.

## ExpFit

Select the engine exponential regression model.

You can also enter: ExpFit.

## Bins

Count observations in the supplied frequency bins.

You can also enter: Bins.

## XCol

Choose the independent statistical-data column.

You can also enter: XCol.

## LinFit

Select the engine linear regression model.

You can also enter: LinFit.

## LR

Fit a linear regression using engine statistical data.

You can also enter: LR.

## LogFit

Select the engine logarithmic regression model.

You can also enter: LogFit.

## ΣMax

Return column maxima from engine statistical data.

You can also enter: ΣMax, MaxΣ.

Try it
[[1 2][2 4][3 6]] StoreData MaxData

You should get: [ 3 6 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## MaxΣ

Return column maxima from engine statistical data.

You can also enter: ΣMax, MaxΣ.

Try it
[[1 2][2 4][3 6]] StoreData MaxData

You should get: [ 3 6 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣMin

Return column minima from engine statistical data.

You can also enter: ΣMin, MinΣ.

Try it
[[1 2][2 4][3 6]] StoreData MinData

You should get: [ 1 2 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## MinΣ

Return column minima from engine statistical data.

You can also enter: ΣMin, MinΣ.

Try it
[[1 2][2 4][3 6]] StoreData MinData

You should get: [ 1 2 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## PCov

Return population covariance.

You can also enter: PCov.

## PSDev

Return population standard deviation.

You can also enter: PSDev.

## PVar

Return population variance, using the full population denominator.

You can also enter: PVar.

Try it
[[1 2][2 4][3 6]] StoreData PopulationVariance

You should get: [ ²/₃ 2 ²/₃ ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## PwrFit

Select the engine power-law regression model.

You can also enter: PwrFit.

## PredX

Predict X from a supplied Y using the current engine regression.

You can also enter: PredX.

## PredY

Predict Y from a supplied X using the current engine regression.

You can also enter: PredY.

## RecallΣ

Recall the engine statistical-data object onto the stack.

You can also enter: RecallΣ, RclΣ.

Try it
[[1 2][2 4][3 6]] StoreData RecallData

You should get: [[ 1 2 ]
  [ 2 4 ]
  [ 3 6 ]]
Sets example statistics data, then recalls it.

## RclΣ

Recall the engine statistical-data object onto the stack.

You can also enter: RecallΣ, RclΣ.

Try it
[[1 2][2 4][3 6]] StoreData RecallData

You should get: [[ 1 2 ]
  [ 2 4 ]
  [ 3 6 ]]
Sets example statistics data, then recalls it.

## ΣLine

Return the current engine regression equation. Native Data regression is managed through Ctrl+D.

You can also enter: ΣLine.

## Σ-

Remove supplied observations from the engine statistical data.

You can also enter: Σ-.

## SDev

Return sample standard deviation from engine statistical data.

You can also enter: SDev.

Try it
[[1 2][2 4][3 6]] StoreData StandardDeviation

You should get: [ 1. 2. ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣData

Access the engine statistical-data object. Ctrl+D uses a separate native data table.

You can also enter: ΣData, ΣDat.

## ΣDat

Access the engine statistical-data object. Ctrl+D uses a separate native data table.

You can also enter: ΣData, ΣDat.

## ΣParameters

Access the engine statistical parameters.

You can also enter: ΣParameters, ΣPar.

## ΣPar

Access the engine statistical parameters.

You can also enter: ΣParameters, ΣPar.

## StoreΣ

Replace engine statistical data with the supplied object. Save native Data tables through their CSV file menu.

You can also enter: StoreΣ, StoΣ.

Try it
[[1 2][2 4][3 6]] StoreData RecallData

You should get: [[ 1 2 ]
  [ 2 4 ]
  [ 3 6 ]]
Replaces the current statistics data. RecallData reads it back.

## StoΣ

Replace engine statistical data with the supplied object. Save native Data tables through their CSV file menu.

You can also enter: StoreΣ, StoΣ.

Try it
[[1 2][2 4][3 6]] StoreData RecallData

You should get: [[ 1 2 ]
  [ 2 4 ]
  [ 3 6 ]]
Replaces the current statistics data. RecallData reads it back.

## ΣX

Return the sum of values in the independent data column.

You can also enter: ΣX.

Try it
[[1 2][2 4][3 6]] StoreData SumOfX

You should get: 6
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣX²

Return the sum of squared X values.

You can also enter: ΣX², ΣX2.

Try it
[[1 2][2 4][3 6]] StoreData SumOfXSquares

You should get: 14
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣX2

Return the sum of squared X values.

You can also enter: ΣX², ΣX2.

Try it
[[1 2][2 4][3 6]] StoreData SumOfXSquares

You should get: 14
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣXY

Return the sum of paired X*Y values.

You can also enter: ΣXY.

Try it
[[1 2][2 4][3 6]] StoreData SumOfXY

You should get: 28
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣY

Return the sum of values in the dependent data column.

You can also enter: ΣY.

Try it
[[1 2][2 4][3 6]] StoreData SumOfY

You should get: 12
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣY²

Return the sum of squared Y values.

You can also enter: ΣY², ΣY2.

Try it
[[1 2][2 4][3 6]] StoreData SumOfYSquares

You should get: 56
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## ΣY2

Return the sum of squared Y values.

You can also enter: ΣY², ΣY2.

Try it
[[1 2][2 4][3 6]] StoreData SumOfYSquares

You should get: 56
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## TVMAmortize

Calculate amortization using engine TVM variables.

You can also enter: TVMAmortize, Amort.

## Amort

Calculate amortization using engine TVM variables.

You can also enter: TVMAmortize, Amort.

## TVMAmortizationTable

Build an amortization table using engine TVM variables.

You can also enter: TVMAmortizationTable, AmortTable, AmortizationSchedule.

## AmortTable

Build an amortization table using engine TVM variables.

You can also enter: TVMAmortizationTable, AmortTable, AmortizationSchedule.

## AmortizationSchedule

Build an amortization table using engine TVM variables.

You can also enter: TVMAmortizationTable, AmortTable, AmortizationSchedule.

## Var

Return sample variance from the engine statistical data.

You can also enter: Var.

Try it
[[1 2][2 4][3 6]] StoreData Variance

You should get: [ 1 4 ]
Replaces calculator statistics data with three rows: (1,2), (2,4), (3,6). Save existing statistics first.

## Asn

Assign an object to an engine virtual-key position.

You can also enter: Asn.

## LibraryConstant

Look up the supplied name in the constants library. Its SD resource must be available.

You can also enter: LibraryConstant.

## EquationName

Return the name of a library constant.

You can also enter: EquationName, XLibName.

## XLibName

Return the name of a library constant.

You can also enter: EquationName, XLibName.

## EquationValue

Return the value of a library constant.

You can also enter: EquationValue, XLibValue.

## XLibValue

Return the value of a library constant.

You can also enter: EquationValue, XLibValue.

## Cst

Open the calculator custom soft menu defined by its custom-menu settings.

You can also enter: Cst.

## DelKeys

Remove specified engine virtual-key assignments.

You can also enter: DelKeys.

## DoError

Raise an error from the supplied message or error value.

You can also enter: DoError.

## DeleteTag

Remove an object's tag.

You can also enter: DeleteTag.

Try it
:Answer:42 DTAG

You should get: 42

## ClearError

Clear the current calculator error. Esc also dismisses a visible error.

You can also enter: ClearError.

## ErrorMessage

Return the most recent calculator error message.

You can also enter: ErrorMessage.

## ErrorNumber

Return the most recent calculator error number.

You can also enter: ErrorNumber.

## TVM

Open the engine TVM-variable solver menu. The native Financial calculator has separate saved inputs.

You can also enter: TVM.

## WSLog

Send flight-recorder data to the configured diagnostic output.

You can also enter: WSLog.

## Tag→

Remove a tag and return its contents using the engine tag convention.

You can also enter: Tag→.

Try it
:Answer:42 FromTag

You should get: 42
"Answer"
Leaves the untagged value below its text tag; the display lists deepest first.

## GC

Reclaim unused calculator arena objects.

You can also enter: GC.

## GCStats

Return engine garbage-collection statistics.

You can also enter: GCStats.

## Hdr

Set the calculator engine header using the supplied header object.

You can also enter: Hdr.

## Keys

Store a complete engine virtual-key map from the supplied directory object. This is an advanced RPL key-assignment operation, separate from PicoCalc physical shortcuts.

You can also enter: Keys.

## LibraryEquation

Load an equation from the equation library using the supplied reference.

You can also enter: LibraryEquation.

## Libraries

List attached engine libraries.

You can also enter: Libraries.

## AvailableMemory

Report available calculator object memory.

You can also enter: AvailableMemory.

## PowerOff

Request engine power-off. To save documents and return to the PicoCalc loader, use Ctrl+Q.

You can also enter: PowerOff.

## PowerOffWithImage

Request engine power-off with its off-image behavior. Ctrl+Q is the safe save-and-return-to-loader workflow.

You can also enter: PowerOffWithImage.

## PgAll

Remove calculator variables in the applicable directory scope. Save important state first.

You can also enter: PgAll.

## RclKeys

Recall engine virtual-key assignments.

You can also enter: RclKeys.

## RclMenu

Return the current engine menu.

You can also enter: RclMenu.

## RunStats

Return engine execution statistics.

You can also enter: RunStats.

## SKey

Use the standard engine action for a virtual-key position.

You can also enter: SKey.

## StoKeys

Store supplied engine key assignments.

You can also enter: StoKeys.

## TMenu

Show a temporary engine menu using the supplied definition.

You can also enter: TMenu.

## →Tag

Attach the supplied label to an object.

You can also enter: →Tag.

Try it
42 "Answer" ToTag

You should get: Answer:42

## LibraryItem

Load or call an SD library object using its library reference.

You can also enter: LibraryItem.

## Range→

Unpack a range into its defining values.

You can also enter: Range→, RangeToReal.

Try it
2 4 ToRange FromRange

You should get: 2
4

## RangeToReal

Unpack a range into its defining values.

You can also enter: Range→, RangeToReal.

Try it
2 4 ToRange FromRange

You should get: 2
4

## PrcRnd

Round to the supplied precision.

You can also enter: PrcRnd.

Try it
-3.141592654_m 0.000045_m  PrcRnd

You should get: -3.14159 3 m
Run from the calculator stack. This example uses the current calculation settings.

## ∩

Return the overlap of two ranges.

You can also enter: ∩, Intersect.

Try it
1 3 ToRange 2 6 ToRange RangeIntersect

You should get: 2…3
Run from the calculator stack. This example uses the current calculation settings.

## Intersect

Return the overlap of two ranges.

You can also enter: ∩, Intersect.

Try it
1 3 ToRange 2 6 ToRange RangeIntersect

You should get: 2…3
Run from the calculator stack. This example uses the current calculation settings.

## ∪

Return the union of two ranges where representable.

You can also enter: ∪, Union.

Try it
1 3 ToRange 2 6 ToRange RangeUnion

You should get: 1…6
Run from the calculator stack. This example uses the current calculation settings.

## Union

Return the union of two ranges where representable.

You can also enter: ∪, Union.

Try it
1 3 ToRange 2 6 ToRange RangeUnion

You should get: 1…6
Run from the calculator stack. This example uses the current calculation settings.

## RelRnd

Round using the supplied relative uncertainty.

You can also enter: RelRnd.

Try it
-3.141592654_m  0.000012  RelRnd

You should get: -3.14159 3 m
Run from the calculator stack. This example uses the current calculation settings.

## CstRU

Return a quantity's relative uncertainty.

You can also enter: CstRU.

## StdRnd

Round using the supplied absolute uncertainty.

You can also enter: StdRnd.

Try it
-3.141592654_m  0.000045_m  StdRnd

You should get: -3.14159 3 m
Run from the calculator stack. This example uses the current calculation settings.

## CstSU

Return a quantity's absolute standard uncertainty.

You can also enter: CstSU.

## →∆Range

Build a range using a central value and absolute uncertainty.

You can also enter: →∆Range.

Try it
10 2 ToDeltaRange

You should get: 10±2

## →%Range

Build a range using a central value and percentage uncertainty.

You can also enter: →%Range.

Try it
10 20 ToPercentRange

You should get: 10±20%
Run from the calculator stack. This example uses the current calculation settings.

## →Range

Build a range from supplied bounds.

You can also enter: →Range, →Interval.

Try it
2 4 ToRange

You should get: 2…4

## →Interval

Build a range from supplied bounds.

You can also enter: →Range, →Interval.

Try it
2 4 ToRange

You should get: 2…4

## →Ur

Build an uncertain quantity using relative uncertainty.

You can also enter: →Ur, →RelUnc.

Try it
-3.141592654_m  0.000097_m  ToRelativeUncertainty

You should get: 0.00003 1
Run from the calculator stack. This example uses the current calculation settings.

## →RelUnc

Build an uncertain quantity using relative uncertainty.

You can also enter: →Ur, →RelUnc.

Try it
-3.141592654_m  0.000097_m  ToRelativeUncertainty

You should get: 0.00003 1
Run from the calculator stack. This example uses the current calculation settings.

## →Us

Build an uncertain quantity using absolute uncertainty.

You can also enter: →Us, →StdUnc.

Try it
-3.141592654_m  0.000012  ToStandardUncertainty

You should get: 0.00003 8 m
Run from the calculator stack. This example uses the current calculation settings.

## →StdUnc

Build an uncertain quantity using absolute uncertainty.

You can also enter: →Us, →StdUnc.

Try it
-3.141592654_m  0.000012  ToStandardUncertainty

You should get: 0.00003 8 m
Run from the calculator stack. This example uses the current calculation settings.

## →σRange

Build an uncertain number from a central value and standard deviation. Converting an existing range to this representation is not implemented.

You can also enter: →σRange, →Uncertain, →Un.

Try it
10 1 ToUncertain

You should get: 10±σ1

## →Uncertain

Build an uncertain number from a central value and standard deviation. Converting an existing range to this representation is not implemented.

You can also enter: →σRange, →Uncertain, →Un.

Try it
10 1 ToUncertain

You should get: 10±σ1

## →Un

Build an uncertain number from a central value and standard deviation. Converting an existing range to this representation is not implemented.

You can also enter: →σRange, →Uncertain, →Un.

Try it
10 1 ToUncertain

You should get: 10±σ1

## →Unit

Combine the supplied value and unit into a quantity. Use Convert to change an existing quantity to another unit.

You can also enter: →Unit.

Try it
5 1_m ToUnit

You should get: 5 m

## BaseUnits

Express a quantity in base units.

You can also enter: BaseUnits.

Try it
1_cm UBASE

You should get: ¹/₁₀₀ m

## FactorUnit

Factor a quantity's units using the supplied unit factor.

You can also enter: FactorUnit.

## UnitValue

Extract the value from a unit-bearing quantity.

You can also enter: UnitValue.

Try it
25_m UVAL

You should get: 25

