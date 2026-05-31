# Instructions

The following is a complete list of all native Funge98 instructions. Fingerprint-specific instructions can be found on
their respective pages. For a list of supported fingerprints, see [Fingerprints](fingerprints/fingerprints.md).

## Datatypes

Recall that data is stored as raw 32-bit patterns, both in the Funge world and on the stack. Possible interpretations of
such patterns are called **datatypes**. Each datatype may be represented by one or more cells. 

Native Funge98 instructions operate on the five main datatypes listed below. Fingerprint-specific instructions may 
utilize extra datatypes as well.

- **int**: A signed 32-bit integer, as represented in two's complement binary by the 32 bits in the provided cell. For 
  example, the integer 10 is represented by the bit pattern `0x0000000A`, and the integer −36 is represented by 
  `0xFFFFFFDC`. This is considered the "default" datatype.
- **char**: A Unicode character, as represented by the 32 bits in the provided cell. For example, the character 'A' is
  represented by the sequence `0x00000041` (codepoint 65).
- **bool**: A boolean; that is, a value that's either `true` or `false`. In Funge98, when passing in a boolean value to
  an instruction, a value of `0` (`0x00000000`) is `false`, and anything else is `true`. When receiving a returned 
  boolean value from an instruction, `true` is always returned as `1` and `false` is `0`.
- **vector**: A vector denoting coordinates in the Funge world. Vectors are stored as sequences of 1, 2, or 3 cells in
  Unefunge, Befunge, and Trefunge worlds, respectively. For example, the origin is stored as the sequence [`0x00000000`,
  `0x00000000`] in a Befunge program, but [`0x00000000`, `0x00000000`, `0x00000000`] in a Trefunge program. When 
  retrieving a vector from the stack or pushing a vector to the stack, the bottom value (i.e., the last value popped or
  the first value pushed) is the *x*-coordinate, followed by the *y* and *z* coordinates if they apply. Instructions 
  that call for a vector automatically detect if the program is Unefunge, Befunge, or Trefunge and use the appropriate
  number of cells.
- **string**: A backwards, null-terminated string; that is, a sequence of Unicode characters, the last of which is a
  null character (`0x00000000`). For example, the string `"Hello!"` is represented by the integer sequence 
  `0 111 108 108 101 72`, where `72` (corresponding to the character 'H') is at the top of the stack. Some
  specifications may call this datatype `0gnirts`, which is derived by the word "string" backwards, with a 0 at the
  beginning to represent the null character.

## Instruction Summary

In the table below, the parameter list is the list of values popped from the stack, and the return list is the list of
values pushed to the stack. Both lists are ordered where the first value is the first value pushed to the stack. This
means that, in the parameter lists, the last value listed is the first value popped (and as such should be the last 
value pushed when passing in parameters).

The instructions in the following table are ordered by ASCII value. The instructions in 
[§Instruction Detail](#instruction-detail) are grouped with related instructions, which is a different order from the 
table. You may find it helpful to use the links in the summary table to navigate.

|          Character           |   Instruction Name   | Parameter List              | Return List    | Description                                                                                                        |
|:----------------------------:|:--------------------:|:----------------------------|:---------------|:-------------------------------------------------------------------------------------------------------------------|
|     [`!`](#logical-not-)     |     Logical NOT      | bool                        | bool           | Performs a logical NOT on the top value of the stack.                                                              |
|             `"`              |     String Mode      | void                        | void           | Toggles "string mode" for the IP.                                                                                  |
|     [`#`](#trampoline-)      |      Trampoline      | void                        | void           | Skips the instruction at the next cell, if there is one.                                                           |
|         [`$`](#pop-)         |         Pop          | int                         | void           | Pops the top value of the stack.                                                                                   |
|       [`%`](#modulus-)       |       Modulus        | int, int                    | int            | Calculates `a % b`.                                                                                                |
|             `&`              |     Read Integer     | void                        | int            | Takes an integer value from the input stream.                                                                      |
|             `'`              |   Fetch Character    | void                        | char           | Fetches the character at the IP's next position.                                                                   |
|             `(`              |   Load Fingerprint   | int..., int                 | int, `1`       | Loads the specified fingerprint.                                                                                   |
|             `)`              |  Unload Fingerprint  | int..., int                 | void           | Unloads the specified fingerprint.                                                                                 |
|      [`*`](#multiply-)       |       Multiply       | int, int                    | int            | Calculates `a * b`.                                                                                                |
|         [`+`](#add-)         |         Add          | int, int                    | int            | Calculates `a + b`.                                                                                                |
|             `,`              |   Print Character    | char                        | void           | Prints the top value of the stack as a Unicode character.                                                          |
|      [`-`](#subtract-)       |       Subtract       | int, int                    | int            | Calculates `a - b`.                                                                                                |
|             `.`              |    Print Integer     | int                         | void           | Prints the top value of the stack as a signed integer.                                                             |
|       [`/`](#divide-)        |        Divide        | int, int                    | int            | Calculates `a / b` using integer division, as in C.                                                                |
|        [`0`](#zero-0)        |         Zero         | void                        | `0`            | Pushes `0` onto the stack.                                                                                         |
|        [`1`](#one-1)         |         One          | void                        | `1`            | Pushes `1` onto the stack.                                                                                         |
|        [`2`](#two-2)         |         Two          | void                        | `2`            | Pushes `2` onto the stack.                                                                                         |
|       [`3`](#three-3)        |        Three         | void                        | `3`            | Pushes `3` onto the stack.                                                                                         |
|        [`4`](#four-4)        |         Four         | void                        | `4`            | Pushes `4` onto the stack.                                                                                         |
|        [`5`](#five-5)        |         Five         | void                        | `5`            | Pushes `5` onto the stack.                                                                                         |
|        [`6`](#six-6)         |         Six          | void                        | `6`            | Pushes `6` onto the stack.                                                                                         |
|       [`7`](#seven-7)        |        Seven         | void                        | `7`            | Pushes `7` onto the stack.                                                                                         |
|       [`8`](#eight-8)        |        Eight         | void                        | `8`            | Pushes `8` onto the stack.                                                                                         |
|        [`9`](#nine-9)        |         Nine         | void                        | `9`            | Pushes `9` onto the stack.                                                                                         |
|      [`:`](#duplicate-)      |      Duplicate       | int                         | int, int       | Duplicates the top value of the stack.                                                                             |
|      [`;`](#jump-over-)      |      Jump Over       | void                        | void           | Causes the IP to instantly skip to the next `;` in its path.                                                       |
|        [`<`](#west-)         |         West         | void                        | void           | Sets the IP's delta to (−1, 0, 0).                                                                                 |
|             `=`              |       Execute        | string                      | int            | Executes a supplied system command.                                                                                |
|        [`>`](#east-)         |         East         | void                        | void           | Sets the IP's delta to (1, 0, 0).                                                                                  |
|       [`?`](#go-away-)       |       Go Away        | void                        | void           | Sets the IP's delta to a random cardinal direction.                                                                |
|             `@`              |         Stop         | void                        | void           | Kills the current IP.                                                                                              |
|      [`[`](#turn-left-)      |      Turn Left       | void                        | void           | Rotates the IP 90° counterclockwise about the *z*-axis.                                                            |
|        [`\`](#swap-)         |         Swap         | int, int                    | int, int       | Swaps the top two values of the stack.                                                                             |
|     [`]`](#turn-right-)      |      Turn Right      | void                        | void           | Rotates the IP 90° clockwise about the *z*-axis.                                                                   |
|        [`^`](#north-)        |        North         | void                        | void           | Sets the IP's delta to (0, -1, 0).                                                                                 |
| [`_`](#x-axis-conditional-_) | *x*-axis Conditional | bool                        | void           | Points the IP west if the top value of the stack is `true`, or east if the top value is `false`.                   |
| [`` ` ``](#greater-than---)  |     Greater Than     | int, int                    | bool           | Compares the top two values of the stack, returning `true` if the first value is strictly greater than the second. |
|        [`a`](#ten-a)         |         Ten          | void                        | `10`           | Pushes `10` onto the stack.                                                                                        |
|       [`b`](#eleven-b)       |        Eleven        | void                        | `11`           | Pushes `11` onto the stack.                                                                                        |
|       [`c`](#twelve-c)       |        Twelve        | void                        | `12`           | Pushes `12` onto the stack.                                                                                        |
|      [`d`](#thirteen-d)      |       Thirteen       | void                        | `13`           | Pushes `13` onto the stack.                                                                                        |
|      [`e`](#fourteen-e)      |       Fourteen       | void                        | `14`           | Pushes `14` onto the stack.                                                                                        |
|      [`f`](#fifteen-f)       |       Fifteen        | void                        | `15`           | Pushes `15` onto the stack.                                                                                        |
|             `g`              |         Get          | vector                      | int            | Retrieves the data at the specified coordinates in the Funge world.                                                |
|        [`h`](#high-h)        |         High         | void                        | void           | Sets the IP's delta to (0, 0, -1).                                                                                 |
|             `i`              |        Input         | string, int, vector         | vector, vector | Loads the contents of a file into the Funge world at the specified coordinates.                                    |
|        [`j`](#jump-j)        |         Jump         | int                         | void           | Jumps the IP forward a specified number of steps by its delta.                                                     |
|       [`k`](#repeat-k)       |        Repeat        | int                         | void           | Executes the next instruction a specified number of times, in a single tick.                                       |
|        [`l`](#low-l)         |         Low          | void                        | void           | Sets the IP's delta to (0, 0, 1).                                                                                  |
| [`m`](#z-axis-conditional-m) | *z*-axis Conditional | bool                        | void           | Points the IP up (zenith) if the top value of the stack is `true`, or down (nadir) if the top value is `false `.   |
|       [`n`](#clear-n)        |     Clear Stack      | int...                      | void           | Clears the stack.                                                                                                  |
|             `o`              |        Output        | vector, vector, int, string | void           | Copies the contents of a specified region of the Funge world to a file.                                            |
|             `p`              |         Put          | int, vector                 | void           | Sets the data at the specified coordinates of the Funge world to a supplied data value.                            |
|             `q`              |         Quit         | int                         | void           | Terminates the program with a specified exit code.                                                                 |
|      [`r`](#reflect-r)       |       Reflect        | void                        | void           | Rotates the IP's delta by 180° (multiplies all components by −1).                                                  |
|             `s`              |   Store Character    | char                        | void           | Stores a supplied character at the IP's next position.                                                             |
|             `t`              |        Split         | void                        | void           | Splits the IP into two, which will proceed to run concurrently.                                                    |
|             `u`              |  Stack Under Stack   | int                         | int...         | Retrieves a specified number of entries from the SOSS.                                                             |
|       [`v`](#south-v)        |        South         | void                        | void           | Sets the IP's delta to (0, 1, 0).                                                                                  |
|      [`w`](#compare-w)       |       Compare        | int, int                    | void           | Turns the IP left if the first value is greater, or right if the second value is greater.                          |
|     [`x`](#set-delta-x)      |      Set Delta       | vector                      | void           | Sets the IP's delta to the supplied vector.                                                                        |
|             `y`              |     System Info      | int                         | int...         | Retrieves SysInfo (see details).                                                                                   |
|             `z`              |     No Operation     | void                        | void           | Stalls the current IP for one tick.                                                                                |
|             `{`              |     Begin Scope      | int..., int                 | int...         | Pushes a new stack to the stack of stacks, transferring a number of elements from the SOSS to the TOSS.            |
| [`\|`](#y-axis-conditional-) | *y*-axis Conditional | bool                        | void           | Points the IP north if the top value of the stack is `true`, or south if the top value is `false `.                |
|             `}`              |      End Scope       | int..., int                 | void           | Points the IP north if the top value of the stack is `true`, or south if the top value is `false `.                |
|             `~`              |    Read Character    | void                        | char           | Takes an character from the input stream.                                                                          |

The uppercase letters `A` to `Z` (Unicode points 65 to 90) "fail" and act identically to `r` by default, but may be 
overloaded to do something else by using `(` to load fingerprints. These functions are listed on the fingerprints'
respective pages.

Remember, if an instruction attempts to pop from the stack while the stack is empty, the stack will quietly return `0` 
without an error.

## Instruction Detail

In this section, the instructions are grouped next to other related elements.

Each instruction will have a general description, followed by a pop list, a push list, and a fail condition, if 
applicable. The pop list lists each datum to be popped from the TOSS, with the first element listed being the last 
element popped (and therefore the first element pushed when supplying arguments). The push list lists each datum to be
pushed to the TOSS as a result of calling the respective instruction, with the first element listed being the first
element pushed to the stack. The fail condition specifies when an instruction "fails". If an instruction fails, it acts
like `r` and causes the IP to flip, allowing programs to catch the error and respond with an alternative.

### Zero (`0`)

Pushes the value `0` onto the stack.

**Pushes:**
- `c: int` — The constant value `0`.

### One (`1`)

Pushes the value `1` onto the stack.

**Pushes:**
- `c: int` — The constant value `1`.

### Two (`2`)

Pushes the value `2` onto the stack.

**Pushes:**
- `c: int` — The constant value `2`.

### Three (`3`)

Pushes the value `3` onto the stack.

**Pushes:**
- `c: int` — The constant value `3`.

### Four (`4`)

Pushes the value `4` onto the stack.

**Pushes:**
- `c: int` — The constant value `4`.

### Five (`5`)

Pushes the value `5` onto the stack.

**Pushes:**
- `c: int` — The constant value `5`.

### Six (`6`)

Pushes the value `6` onto the stack.

**Pushes:**
- `c: int` — The constant value `6`.

### Seven (`7`)

Pushes the value `7` onto the stack.

**Pushes:**
- `c: int` — The constant value `7`.

### Eight (`8`)

Pushes the value `8` onto the stack.

**Pushes:**
- `c: int` — The constant value `8`.

### Nine (`9`)

Pushes the value `9` onto the stack.

**Pushes:**
- `c: int` — The constant value `9`.

### Ten (`a`)

Pushes the value `10` onto the stack.

**Pushes:**
- `c: int` — The constant value `10`.

### Eleven (`b`)

Pushes the value `11` onto the stack.

**Pushes:**
- `c: int` — The constant value `11`.

### Twelve (`c`)

Pushes the value `12` onto the stack.

**Pushes:**
- `c: int` — The constant value `12`.

### Thirteen (`d`)

Pushes the value `13` onto the stack.

**Pushes:**
- `c: int` — The constant value `13`.

### Fourteen (`e`)

Pushes the value `14` onto the stack.

**Pushes:**
- `c: int` — The constant value `14`.

### Fifteen (`f`)

Pushes the value `15` onto the stack.

**Pushes:**
- `c: int` — The constant value `15`.

### Duplicate (`:`)

Duplicates the top value of the stack. If the stack is empty, this instruction simply pushes a `0`.

**Pops:**
- `v: int` — The value to duplicate.

**Pushes:**
- `v1: int` — The popped value.
- `v2: int` — The popped value, again.

### Swap (`\`)

Swaps the top two values of the stack. If the stack only has one value, this instruction simply pushes a `0`. If the
stack is empty, this instruction pushes two `0`s.

**Pops:**
- `a: int` — The second value popped.
- `b: int` — The first value popped.

**Pushes:**
- `ra: int` — The value of `b`.
- `rb: int` — The value of `a`.

### Pop (`$`)

Pops the top value of the stack. If the stack is empty, this instruction simply acts as a no-op.

**Pops:**
- `v: int` — The top value of the stack.

### Clear (`n`)

Pops all values from the stack. If the stack is empty, this instruction simply acts as a no-op.

**Pops:**
- `e: int...` — All values in the stack.

### Add (`+`)

Pops the top two values from the stack and pushes the sum. If the stack has only one value, this instruction is 
effectively a no-op. If the stack is empty, this instruction pushes a `0`.

If the addition results in an integer overflow, the returned value quietly wraps around without an error.

**Pops:**
- `a: int` — The first addend.
- `b: int` — The second addend.

**Pushes:**
- `r: int` — The result of `a + b`.

### Subtract (`+`)

Pops the top two values from the stack and pushes the difference. If the stack has only one value, it will be negated
(e.g, `5` becomes `-5`). If the stack is empty, this instruction pushes a `0`.

If the subtraction results in an integer overflow, the returned value quietly wraps around without an error.

**Pops:**
- `a: int` — The minuend.
- `b: int` — The subtrahend.

**Pushes:**
- `r: int` — The result of `a - b`.

### Multiply (`*`)

Pops the top two values from the stack and pushes the product. If the stack has only one value, it will be replaced with
a `0`. If the stack is empty, this instruction pushes a `0`.

If the multiplication results in an integer overflow, the returned value quietly wraps around without an error.

**Pops:**
- `a: int` — The first multiplicand.
- `b: int` — The second multiplicand.

**Pushes:**
- `r: int` — The result of `a * b`.

### Divide (`/`)

Pops the top two values from the stack and pushes the quotient. If the stack has only one value, it will be replaced 
with a `0`. If the stack is empty, this instruction pushes a `0`.

This instruction performs integer division, nearly identical to languages like C and Java. Any remainder is simply 
truncated. However, if the divisor (`b`) is `0`, this instruction quietly returns `0` instead of failing.

**Pops:**
- `a: int` — The dividend.
- `b: int` — The divisor.

**Pushes:**
- `r: int` — The result of `a / b`, or `0` if `b` is `0`.

### Modulus (`%`)

Pops the top two values from the stack and pushes the quotient. If the stack has only one value, it will be replaced
with a `0`. If the stack is empty, this instruction pushes a `0`.

If the divisor (`b`) is `0`, this instruction quietly returns `0` instead of failing.

**Pops:**
- `a: int` — The dividend.
- `b: int` — The divisor.

**Pushes:**
- `r: int` — The result of `a % b`, or `0` if `b` is `0`.

### West (`<`)

Sets the IP to point one unit west (in the negative *x*-direction).

### East (`>`)

Sets the IP to point one unit east (in the positive *x*-direction).

### North (`^`)

Sets the IP to point one unit north (in the negative *y*-direction).

**Fails:** If the program is a Unefunge program.

### South (`v`)

Sets the IP to point one unit south (in the positive *y*-direction).

**Fails:** If the program is a Unefunge program.

### High (`h`)

Sets the IP to point one unit up, or zenith (in the negative *z*-direction).

**Fails:** If the program is a Unefunge or Befunge program.

### Low (`l`)

Sets the IP to point one unit down, or nadir (in the positive *z*-direction).

**Fails:** If the program is a Unefunge or Befunge program.

### Go Away (`?`)

Sets the IP to point one unit in a random cardinal direction. In Unefunge, this instruction can randomly act as `<` or
`>`. In Befunge, this instruction can act as `<`, `>`, `^`, or `v`. In Trefunge, this instruction can act as `<`, `>`, 
`^`, `v`, `h`, or `l`.

### Reflect (`r`)

Multiplies the IP's delta by −1.

### Set Delta (`x`)

Sets the IP's delta to `delta`. The IP's new delta instantly takes over on the tick it's assigned, and the next cell
visited will be according to the new delta.

Zero-values and non-zero components other than `1` or `-1` are allowed. For example, a vector (2, 0, 0) can be passed in
to cause the IP to move east but skip every other cell, and a vector (1, −2, 0) can be passed in to cause the IP to move
as a chess knight in the north-northeast direction. A zero-vector can also be passed in, which causes the vector to 
completely stop in place. If the stack is empty, this causes an infinite loop.

An IP with a zero delta is said to be "stopped", and an IP with a delta that is not one of the cardinal directions is
said to be "flying".

**Pops:**
- `delta: vector` — The new delta to assign to the IP.

### *x*-axis Conditional (`_`)

Pops the top value of the stack. If the value is `true` (i.e., not `0`), this instruction acts as `<`. Otherwise, it
acts as `>`. If the stack is empty, acts as `>`.

**Pops:**
- `b: bool` — The boolean to check.

### *y*-axis Conditional (`|`)

Pops the top value of the stack. If the value is `true` (i.e., not `0`), this instruction acts as `^`. Otherwise, it
acts as `v`. If the stack is empty, acts as `v`.

**Pops:**
- `b: bool` — The boolean to check.

### *z*-axis Conditional (`m`)

Pops the top value of the stack. If the value is `true` (i.e., not `0`), this instruction acts as `h`. Otherwise, it
acts as `l`. If the stack is empty, acts as `l`.

**Pops:**
- `b: bool` — The boolean to check.

### Turn Left (`[`)

Rotates the IP 90° counterclockwise. 

In Trefunge programs, the rotation always occurs about the *z*-axis. That is, a vector with an initial delta of 
(*x*, *y*, *z*) will get a new delta of (*y*, −*x*, *z*).

**Fails:** If the program is a Unefunge program.

### Turn Right (`]`)

Rotates the IP 90° clockwise.

In Trefunge programs, the rotation always occurs about the *z*-axis. That is, a vector with an initial delta of
(*x*, *y*, *z*) will get a new delta of (−*y*, *x*, *z*).

**Fails:** If the program is a Unefunge program.

### Compare (`w`)

Pops two values `a` and `b`. If `a < b`, acts as `[`. If `a > b`, acts as `]`. If the two operands are equal, acts as 
`z`.

If the stack is empty, this instruction acts as a no-op (`z`). If the stack has only one value, it is passed as `b` 
and compared using `a = 0`: a value of `0` results in `z`, a positive value results in `[`, and a negative value results
in `]`.

**Pops:**
- `a: int` — The first operand.
- `b: int` — The second operand.

**Fails:** If the program is a Unefunge program. If this is the case, the stack is left untouched (no values are 
popped).

### Greater Than (`` ` ``)

Pops two values and checks if `a > b`. If the stack only has one element, this instruction returns `1` if that element 
is negative, or `0` otherwise. If the stack is empty, this instruction pushes a `0`.

**Pops:**
- `a: int` — The first operand.
- `b: int` — The second operand.

**Pushes:**
- `r: bool` — `true` if `a > b`

### Logical NOT (`!`)

Inverts the top of the stack. If the stack is empty, this instruction pushes a `1`.

**Pops:**
- `a: bool` — The value to invert.

**Pushes:**
- `r: bool` — `true` if `a` is false (i.e., `0`, or if the stack is empty), `false` otherwise.

### Trampoline (`#`)

Causes the IP to skip over the next Funge cell. If the cell is empty, this instruction acts as a no-op.

### Jump (`j`)

Causes the IP to skip over a specified number of Funge cells in its current delta. If `n` is `0`, this instruction acts
as a no-op. If `n` is `1`, this instruction behaves as `#`. Negative values send the IP backwards by the specified 
amount; the delta remains unchanged.

**Pops:**
- `n: int` — The number of Funge cells to jump over.

### Repeat (`k`)

Pops a value off the stack, and repeats the next instruction in the IP's path that many times, all in one tick total. If
the next cell is a space (` `), it is ignored, and the IP continues traveling until it finds a valid instruction. If `n`
is `0`, the next instruction is skipped.

**Pops:**
- `n: int` — The number of times to repeat the next instruction.

### Jump Over (`;`)

Causes the IP to instantly skip to the next `;` in its path, consuming zero ticks. If there is no other `;` in either 
direction, this instruction is completely ignored. The next instruction after the `;` is executed in the same tick
(important for Concurrent Funge).