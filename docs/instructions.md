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
|     [`"`](#string-mode-)     |     String Mode      | void                        | void           | Toggles "string mode" for the IP.                                                                                  |
|     [`#`](#trampoline-)      |      Trampoline      | void                        | void           | Skips the instruction at the next cell, if there is one.                                                           |
|         [`$`](#pop-)         |         Pop          | int                         | void           | Pops the top value of the stack.                                                                                   |
|       [`%`](#modulus-)       |       Modulus        | int, int                    | int            | Calculates `a % b`.                                                                                                |
|    [`&`](#read-integer-)     |     Read Integer     | void                        | int            | Takes an integer value from the input stream.                                                                      |
|   [`'`](#fetch-character-)   |   Fetch Character    | void                        | char           | Fetches the character at the IP's next position.                                                                   |
|  [`(`](#load-fingerprint-)   |   Load Fingerprint   | int..., int                 | int, `1`       | Loads the specified fingerprint.                                                                                   |
| [`)`](#unload-fingerprint-)  |  Unload Fingerprint  | int..., int                 | void           | Unloads the specified fingerprint.                                                                                 |
|      [`*`](#multiply-)       |       Multiply       | int, int                    | int            | Calculates `a * b`.                                                                                                |
|         [`+`](#add-)         |         Add          | int, int                    | int            | Calculates `a + b`.                                                                                                |
|   [`,`](#print-character-)   |   Print Character    | char                        | void           | Prints the top value of the stack as a Unicode character.                                                          |
|      [`-`](#subtract-)       |       Subtract       | int, int                    | int            | Calculates `a - b`.                                                                                                |
|    [`.`](#print-integer-)    |    Print Integer     | int                         | void           | Prints the top value of the stack as a signed integer.                                                             |
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
|   [`=`](#execute-command-)   |       Execute        | string                      | int            | Executes a supplied system command.                                                                                |
|        [`>`](#east-)         |         East         | void                        | void           | Sets the IP's delta to (1, 0, 0).                                                                                  |
|       [`?`](#go-away-)       |       Go Away        | void                        | void           | Sets the IP's delta to a random cardinal direction.                                                                |
|     [`@`](#stop-thread-)     |         Stop         | void                        | void           | Kills the current IP.                                                                                              |
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
|        [`g`](#get-g)         |         Get          | vector                      | int            | Retrieves the data at the specified coordinates in the Funge world.                                                |
|        [`h`](#high-h)        |         High         | void                        | void           | Sets the IP's delta to (0, 0, -1).                                                                                 |
|     [`i`](#input-file-i)     |        Input         | vector, int, string         | vector, vector | Loads the contents of a file into the Funge world at the specified coordinates.                                    |
|        [`j`](#jump-j)        |         Jump         | int                         | void           | Jumps the IP forward a specified number of steps by its delta.                                                     |
|       [`k`](#repeat-k)       |        Repeat        | int                         | void           | Executes the next instruction a specified number of times, in a single tick.                                       |
|        [`l`](#low-l)         |         Low          | void                        | void           | Sets the IP's delta to (0, 0, 1).                                                                                  |
| [`m`](#z-axis-conditional-m) | *z*-axis Conditional | bool                        | void           | Points the IP up (zenith) if the top value of the stack is `true`, or down (nadir) if the top value is `false `.   |
|       [`n`](#clear-n)        |     Clear Stack      | int...                      | void           | Clears the stack.                                                                                                  |
|    [`o`](#output-file-o)     |        Output        | vector, vector, int, string | void           | Copies the contents of a specified region of the Funge world to a file.                                            |
|        [`p`](#put-p)         |         Put          | int, vector                 | void           | Sets the data at the specified coordinates of the Funge world to a supplied data value.                            |
|        [`q`](#quit-q)        |         Quit         | int                         | void           | Terminates the program with a specified exit code.                                                                 |
|      [`r`](#reflect-r)       |       Reflect        | void                        | void           | Rotates the IP's delta by 180° (multiplies all components by −1).                                                  |
|  [`s`](#store-character-s)   |   Store Character    | char                        | void           | Stores a supplied character at the IP's next position.                                                             |
|       [`t`](#split-t)        |        Split         | void                        | void           | Splits the IP into two, which will proceed to run concurrently.                                                    |
| [`u`](#stack-under-stack-u)  |  Stack Under Stack   | int                         | int...         | Retrieves a specified number of entries from the SOSS.                                                             |
|       [`v`](#south-v)        |        South         | void                        | void           | Sets the IP's delta to (0, 1, 0).                                                                                  |
|      [`w`](#compare-w)       |       Compare        | int, int                    | void           | Turns the IP left if the first value is greater, or right if the second value is greater.                          |
|     [`x`](#set-delta-x)      |      Set Delta       | vector                      | void           | Sets the IP's delta to the supplied vector.                                                                        |
|  [`y`](#get-system-info-y)   |     System Info      | int                         | int...         | Retrieves SysInfo (see details).                                                                                   |
|    [`z`](#no-operation-z)    |     No Operation     | void                        | void           | Stalls the current IP for one tick.                                                                                |
|     [`{`](#begin-scope-)     |     Begin Scope      | int..., int                 | int...         | Pushes a new stack to the stack of stacks, transferring a number of elements from the SOSS to the TOSS.            |
| [`\|`](#y-axis-conditional-) | *y*-axis Conditional | bool                        | void           | Points the IP north if the top value of the stack is `true`, or south if the top value is `false `.                |
|      [`}`](#end-scope-)      |      End Scope       | int..., int                 | void           | Points the IP north if the top value of the stack is `true`, or south if the top value is `false `.                |
|   [`~`](#read-character-)    |    Read Character    | void                        | char           | Takes an character from the input stream.                                                                          |

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

Duplicates the top value of the stack. More specifically, this instruction pops the top value of the stack and pushes it
back twice. This means that an empty stack will produce *two* `0`s, not just one.

**Pops:**
- `v: int` — The value to duplicate.

**Pushes:**
- `v1: int` — The popped value.
- `v2: int` — The popped value, again.

### Swap (`\`)

Swaps the top two values of the stack. If the stack only has one value, this instruction effectively simply pushes a 
`0`. If the stack is empty, this instruction pushes two `0`s.

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
(e.g., `5` becomes `-5`). If the stack is empty, this instruction pushes a `0`.

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
truncated. A divisor (`b`) of `0` is not an error; the instruction simply returns `0`.

**Pops:**
- `a: int` — The dividend.
- `b: int` — The divisor.

**Pushes:**
- `r: int` — The result of `a / b`, or `0` if `b` is `0`.

### Modulus (`%`)

Pops the top two values from the stack and pushes the remainder that results from a division. If the stack has only one 
value, it will be replaced with a `0`. If the stack is empty, this instruction pushes a `0`.

A divisor (`b`) of `0` is not an error; the instruction simply returns `0`.

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
as a chess knight in the north-northeast direction. A zero-vector can also be passed in, which causes the pointer to
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

### No Operation (`z`)

Does nothing; stalls the IP for one tick.

Unlike spaces, which are completely skipped over, taking zero ticks to execute, `z` actually takes one tick, which is
useful in Concurrent Funge to get timings down.

### Trampoline (`#`)

Causes the IP to skip over the next Funge cell. If the cell is empty, this instruction acts as a no-op.

### Jump (`j`)

Causes the IP to skip over a specified number of Funge cells in its current delta. If `n` is `0`, this instruction acts
as a no-op. If `n` is `1`, this instruction behaves as `#`. Negative values send the IP backwards by the specified
amount; the delta remains unchanged.

**Pops:**
- `n: int` — The number of Funge cells to jump over.

### Repeat (`k`)

Pops a value `n` from the stack. A ghost IP is then created with the current IP's position and delta to seek out the 
next instruction in the IP's path. Once that instruction is found, it is executed `n` times *from the real IP's 
position* (i.e., at the `k`, *not* at the next instruction). Note that spaces (` `), along with semicolons (`;`) and 
anything in between them, are never executed by `k`.

For example, the following code would print `5`, *not* `6`, as the `v` instruction (to go south) is executed from the
`k`, not the `v`:
```
>1kv
  56
  >>.@
```

Additionally, because the IP really stays at `k` and only sends a ghost IP to seek an instruction, once the `k` is 
finished running, assuming the IP's position or delta did not change as a result, it will continue into the next 
instruction, running it an extra time. For example, the following will print *five* `0`'s, not four (recall that popping
from an empty stack is not an error and instead gracefully returns `0`):
```
>4k.@
```

In Concurrent Funge, it's important to note that the first four zeros are printed all in one tick, but the extra fifth
zero is printed on the IP's *next* tick.

However, if `n` is zero, the IP instead teleports to the next instruction, skipping it entirely. For example, this 
prints nothing whatsoever, not even the "extra" `0` that would appear if a positive number had been passed as `n` into 
`k`:
```
>0k.@
```

**Pops:**
- `n: int` — The number of times to repeat the next instruction.

**Fails:** If `n` is negative, or if the instruction that's called from this instruction fails.

### Jump Over (`;`)

Causes the IP to instantly skip to the next `;` in its path, consuming zero ticks. If there is no other `;` in either
direction, this instruction is completely ignored. The next instruction after the `;` is executed in the same tick
(important for Concurrent Funge).

### String Mode (`"`)

Toggles string mode for the IP, causing it to push subsequent characters to the stack instead of executing them, up 
until the next quotation mark. Consecutive whitespaces are collapsed in one tick total.

### Fetch Character (`'`)

Pushes the next character to the stack, skipping its execution.

**Pushes:**
- `c: char` — The next character in the IP's path.

### Store Character (`s`)

Places the top value on the stack onto the IP's next space. This space is then skipped by the IP.

**Pops:**
- `c: char` — The character to store.

### Read Integer (`&`)

Prompts the user for an integer input.

This instruction guarantees an integer; if the user gives malformed input, it is discarded and the user is reprompted.
After an integer is received, the rest of the stream is discarded.

**Pushes:**
- 'n: int' — The integer received from the user input stream.

### Read Character (`~`)

Prompts the user for a character.

Unlike the `&` instruction, this instruction does not discard the rest of the input stream after the character is 
received. As such, strings can be received by executing this instruction in a loop and comparing each character to the
integer `10` (the ASCII value of a newline, `\n`). Note that the received string will be backwards if stored solely on
the stack, so it's recommended to use `p` to store each character elsewhere within the Funge space as input is being 
received.

**Pushes:**
- `c: char` — The character received from the user input stream.

### Print Integer (`.`)

Prints an integer. The integer will be followed by a space by default; however, if the interpreter detects that an ANSI
escape sequence is being printed, the space will be skipped. This allows the Funge script to use the `.` instruction
seamlessly instead of requiring a conversion to a character sequence.

**Pops:**
- `n: int` — The integer to print.

### Print Character (`,`)

Prints a character.

Strings can be printed by looping this instruction. For example, this is one way to print "Hello world!" followed by a 
newline:
```
0a"!dlrow olleH">:#,_$ ;continue;
```

If you are coming from Befunge93, bear in mind that 32-bit Funge98 uses UTF-32 characters rather than ASCII characters. 
For basic characters, this does not change anything, but values will change for ASCII values above 127.

**Pops:**
- `c: char` — The character to print.

### Put (`p`)

Writes a value into the Funge space. This is often used to store persistent data within the negative spaces of the Funge
world, but it can also overwrite pre-existing instructions in the positive space.

**Pops:**
- `n: int` — The value to store.
- `v: vector` — The location to store `n`.

### Get (`g`)

Gets a value from the Funge space.

**Pops:**
- `v: vector` — The location from which the value will be grabbed.

**Pushes:**
- `n: int` — The value at location `v`.

### Input File (`i`)

Writes the contents of a file into the Funge space.

The file will be internally converted to UTF-32 to fit the 32-bit Funge space. Spaces in the original file do not 
overwrite anything within the Funge space; they are treated as transparent characters. In Unefunge, line breaks and form
feed characters are ignored. In Befunge, form feed characters are ignored, and line breaks increment the *y*-axis while
resetting the *x*-axis. In Trefunge, form feed characters increment the *z*-axis while resetting the *x*- and *y*-axes.

If the low bit of `flags` is set, the file is read in binary mode instead. In all dimensions of Funge, the newline and 
form feed characters are copied literally into the Funge space rather than incrementing the *y*- and *z*-axes.

After the file is read, this instruction pushes two vectors: the `location` that was supplied, along with another vector
relative to `location` (instead of the origin) that says how much space the file's contents took up in the Funge space.
These are suitable arguments to use with a corresponding `o` instruction.

***Note:*** If you read about this instruction from the [Esolangs Wiki](https://esolangs.org/wiki/Funge-98), or looked
at the table from the [Funge-98 Final Specification](https://github.com/catseye/Funge-98/blob/master/doc/funge98.markdown#instruction-quick-reference),
`location` is listed before `size` in the push list. That's an error. From the article portion of the specification:

> `i` pops a null-terminated 0"gnirts" string for the filename, followed by a flags cell, then a vector Va telling it 
> where to operate. If the file can be opened for reading, it is inserted into Funge-Space at Va, and immediately 
> closed. **Two vectors are then pushed onto the stack, Va and Vb, suitable arguments to a corresponding `o` 
> instruction.** If the file open failed, the instruction acts like `r`.

*Va* and *Vb* correspond to `location` and `size`, respectively.

***Note:*** If you are using the Mycology test suite, it will incorrectly label a few tests related to this instruction 
as **[BAD]**. Mycology expects `size` to be exclusive, which is not consistent with Catseye's specification, which says 
that `size` is inclusive (the same as `y`).

**Pops:**
- `location: vector` — The upper northwest corner (negative axes) of the region to start placing the file's contents, 
  relative to the IP's storage offset, inclusive.
- `flags: int` — Only the first (low) bit is relevant: dictates whether to read the file in binary mode.
- `filename: string` — The name of the file to read from.

**Pushes:**
- `size: vector` — The lower southeast corner (positive axes) of the region of the file's content box, relative to
  `location`, inclusive.
- `location: vector` — The upper northwest corner (negative axes) of the region of the file's content box, relative to 
  the IP's storage offset, inclusive.

**Fails:** If the `-r` flag was not enabled when running the Funge98 file, or if the file of the supplied file name does
not exist or otherwise could not be opened. In the former case, the instruction reflects immediately without touching 
the stack. In the latter cases, the instruction pushes the `filename` back onto the stack and then reflects. In any 
case, the stack effectively remains untouched.

### Output File (`o`)

Writes the contents of a specified region of the Funge space to a specified file, attempting to create the file if it 
does not already exist.

The interpreter will automatically convert its internal UTF-32 data to UTF-8 before outputting to the file. By default,
all data, including any trailing spaces, are copied over to the file.

***Note:*** If you read about this instruction from the [Esolangs Wiki](https://esolangs.org/wiki/Funge-98), or looked
at the table from the [Funge-98 Final Specification](https://github.com/catseye/Funge-98/blob/master/doc/funge98.markdown#instruction-quick-reference),
`location` is listed before `size` in the pop list. That's an error. From the article portion of the specification:

> `o` first pops a null-terminated 0"gnirts" string to use for a filename. Then it pops a flags cell. **It then pops a 
> vector Va indicating a *least point* (point with the smallest numerical coordinates of a region; also known as the 
> upper-left corner, when used in the context of Befunge) in space, and another vector Vb describing the size of a 
> rectangle (or a rectangular prism in Trefunge, etc).** If the file named by the filename can be opened for writing, 
> the contents of the rectangle of Funge-Space from Va to Va+Vb are written into it, and it is immediately closed. If 
> not, the instruction acts like `r`.

*Va* and *Vb* correspond to `location` and `size`, respectively.

***Note:*** If you are using the Mycology test suite, it will incorrectly label a few tests related to this instruction 
as **[BAD]**. Mycology expects `size` to be exclusive, which is not consistent with Catseye's specification, which says
that `size` is inclusive (the same as `y`).

**Pops:**
- `size: vector` — The lower southeast corner (positive axes) of the region of the file's content, relative to
  `location`, inclusive.
- `location: vector` — The upper northwest corner (negative axes) of the region of the file's content, relative to the
  IP's storage offset, inclusive.
- `flags: int` — Only the first (low) bit is relevant: dictates whether to write the file in linear mode.
- `filename: string` — The name of the file to write to.

**Fails:** If the `-w` flag was not enabled when running the Funge98 file, or if the file otherwise could not be written
to. The stack remains untouched, or is otherwise restored before calling the fail.

### Execute Command (`=`)

Executes a system command as if it were executed on the command line. Calling this instruction is equivalent to calling
`system()` in C or running the command on a terminal.

After execution, this command pushes the exit code onto the stack, which provides information about the command's 
success or failure.

**Pops:**
- `command: string` — The command to execute.

**Pushes:**
- `result: int` — The exit code of the command.

**Fails**: If the `-e` flag was not enabled when running the Funge98 file. The stack remains untouched. Note that 
attempting to run a command that does not exist on the system is **not** considered a fail by the interpreter; to catch 
this case, the Funge98 program should instead compare `result` to `127`, which is the standard exit code for "Command 
not found". Attempting to run a command with insufficient permissions unrelated to the `-e` flag (e.g., admin privileges
were required but not provided) is also **not** a fail; the Funge98 program should instead compare `result` to `126`, 
which is the standard exit code for "Command found but could not be executed".

### Split (`t`)

Splits this IP. The child IP will have the same position, storage offset, stacks, and fingerprints, as the parent IP, 
but its delta will be inverted. This IP will receive its first tick immediately before the parent IP's next tick.

### Begin Scope (`{`)

Begins a new scope. When a new scope is created for an IP, the top stack of the IP's stack of stacks (the TOSS, the "top
of stack stack") is split according to the top value of the TOSS (`n`). The new stack becomes the new TOSS, and the 
former TOSS becomes the SOSS ("second on stack stack"). If `n` is positive, that many elements are transferred to the 
new TOSS. If `n` exceeds the size of the SOSS, the remaining bottom elements are padded with `0`s. If `n` is negative, 
that many zeros are pushed onto the SOSS.

The IP's storage offset is then pushed onto the SOSS, and the location of the IP's next instruction becomes the new 
storage offset.

**Pops:**
- `e: int...` — A list of elements of size `n` to transfer to the new TOSS.
- `n: int` — The number of elements `e` to transfer to the new TOSS.

**Pushes:**
- `e: int...` — The list of elements grabbed from the SOSS, as dictated by `n`. The rest stay on the SOSS.

### End Scope (`}`)

Ends a scope. When a scope ends for an IP, a value `n` is popped from the TOSS, and a vector is popped from the SOSS,
which becomes the new storage offset for the IP. Afterward, `n` elements are transferred from the TOSS to the SOSS. If
`n` is greater than the size of the TOSS, the rest of the cells are filled with zeros. If `n` is negative, that many 
cells are popped from the SOSS instead. The TOSS is then popped, and the SOSS becomes the new TOSS.

**Pops:**
- `e: int...` — A list of elements of size `n` to transfer to the SOSS.
- `n: int` — The number of elements `e` to transfer to the SOSS.

**Pushes:**
- `e: int...` — The list of elements grabbed from the former TOSS, as dictated by `n`.

**Fails:** If there is no SOSS (i.e., no `{` was called first).

### Stack Under Stack (`u`)

Transfers a specified number (`n`) of cells from the SOSS to the TOSS. These cells are transferred in a pop-push loop,
meaning cells are transferred in reverse order from how they were in the SOSS. If `n` is negative, cells are transferred
from the TOSS to the SOSS instead. If `n` is larger than the number of cells from the source stack, the remaining cells
are padded with `0`s.

**Pops:**
- `n: int` — The number of elements to transfer.

**Pushes:**
- `e: int...` — A list of `n` elements transferred from the SOSS, in reverse order.

**Fails:** If there is no SOSS.

### Get System Info (`y`)

Pops a value `n`. This instruction then loads an array of system information onto the stack, in the following order (the
bottom of this list is closest to the top of the stack):
- `flags: int` — A 5-bit integer, with the following bits: `[unbufferedIO | execute | write | read | concurrent]`
  - `unbufferedIO` — Always low on this interpreter. This interpreter uses buffered IO streams.
  - `execute` — High if `-e` was enabled.
  - `write` — High if `-w` or `-o` was enabled.
  - `read` — High if `-r` or `-i` was enabled.
  - `concurrent` — Always high on this interpreteer. This interpreter supports Concurrent Funge.
- `bytesPerCell: int` — The number `4`, as this interpreter runs 32-bit Funge.
- `handprint: int` — This interpreter's handprint is `1230198612` (hex `0x49535754`).
- `version: int` — The version number, without dots or letters. This interpreter's current version is **v0.2.2**, which
  is pushed as `202`.
- `operatingParadigm: int` — If `-e` is enabled, this number will be `1`, meaning calling `=` is equivalent to calling
  `system()` in C. If `-e` is disabled, this number will be `0`.
- `pathSeparator: char` — The OS's preferred path separator (`\` on Windows, `/` on most other operating systems).
- `dimensions: int` — The number of dimensions of the world, which also dictates the number of scalars per vector.
- `id: int` — The ID of the current IP. ID's simply start at `0` and increment each time a new one is created via `t`, 
  ensuring no duplicate ID's happen (unless over 4 billion IPs are created during a Funge program, then duplicate ID's 
  will inevitably occur due to integer overflow).
- `location: vector` — The IP's current location.
- `delta: vector` — The IP's current delta.
- `offset: vector` — The IP's current storage offset.
- `leastPoint: vector` — The least point that contains a non-space cell, relative to the origin, inclusive.
- `greatestPoint: vector` — The greatest point that contains a non-space cell, relative to `leastPoint`, inclusive.
- `day: int` — The current day, as `year * 65536 + month * 256 + dayOfMonth`, where `month` is a number in the range 
  1–12. For example, **25 December 2012** is pushed as **2012(65536) + 5(256) + 25** = `131859737`.
- `time: int` — The current time, as `hour * 65536 + minute * 256 + second`. For example, the time **13:15:28** (1:15:28 
  PM) is pushed as **13(65536) + 15(256) + 28** = `855836`.
- `stacks: int` — The current number of stacks that this IP has on its stack of stacks.
- `stackSizes: int[]` — An array of size `stacks`, containing the size of each stack on the stack of stacks. The size of
  the top stack is pushed first. Sizes are recorded *before* `y` starts pushing data.
- `args: string[]` — A list of null-terminated strings containing program arguments, the list itself terminated by 
  a double null. This allows a null string as an argument, but no two consecutive arguments may be null strings. The 
  argument pushed is the name of the Funge source file.
- `envars: string[]` — A list of null-terminated strings representing the current environment variables, the list 
  terminated by another null character. Strings are in the format `KEY=VALUE`.

If `n` is negative, this instruction immediately returns after pushing this information, leaving around a hundred new
cells on the stack. However, if `n` is positive, then the integer at the specified index after all these cells are 
pushed onto the stack is saved, and the rest are cleared. For example, `7y` would result in only the number of 
dimensions being stored on the stack. In Befunge, an `n` of **10** would result in getting the *y*-value of the IP's 
location, and **11** would result in the *x*-value, etc.

If `n` is greater than the number of cells pushed onto the stack, all those cells are cleared, and the difference 
between `n` and the number of cells pushed is used as an index to pick a cell from the stack. For example, if the stack
initially contained `5 7 1 4 3` (where `3` is the top of the stack), and `y` pushes 100 cells, passing `102` as `n` 
would result in the stack holding `5 7 1 4 3 4`, where the new `4` is taken from the second element from the top 
originally. If `n` is greater than the total number of cells (i.e., 106 in this case), then a `0` is pushed.

**Pops:**
- `n: int` — The index of the cell to keep

**Pushes:**
- `info: int...` — The cell at the index of the information array, or the entire information array if `n <= 0`.

### Load Fingerprint (`(`)

Pops a number `n`, then pops the next `n` cells, creating a number by multiplying a temporary value by 256 and pushing
the next number to it for each cell. The IP then attempts to load a fingerprint of the specified number. Values of `n`
above 4 simply pop extra cells, as this interpreter is limited to 32 bits.

After the fingerprint is loaded, the IP pushes the temporary value back onto the stack, along with the number `1`. These
values are fit to be used for the corresponding `)` instruction, but they are often discarded.

**Pops:**
- `fingerprint: int...` — A sequence of `n` numbers that combine to form a fingerprint ID.
- `n: int` — The number of cells to use to construct the fingerprint ID.

**Pushes:**
- `fingerprint: int` — The fingerprint ID, as one integer.
- `c: int` — The number `1`.

**Fails:** If the specified fingerprint is not supported by this interpreter. The `n` values are still popped. A
negative `n` is treated as `0` and pops nothing other than the `n` itself.

### Unload Fingerprint (`)`)

Pops a number `n`, then pops the next `n` cells, creating a number by multiplying a temporary value by 256 and pushing
the next number to it for each cell. The IP then attempts to unload the fingerprint represented by this number. Values 
of `n` above 4 simply pop extra cells, as this interpreter is limited to 32 bits.

**Pops:**
- `fingerprint: int...` — A sequence of `n` numbers that combine to form a fingerprint ID.
- `n: int` — The number of cells to use to construct the fingerprint ID.

**Fails:** If the specified fingerprint is not supported by this interpreter. The `n` values are still popped. A 
negative `n` is treated as `0` and pops nothing other than the `n` itself. If the fingerprint is supported but was never 
loaded, calling this instruction is not an error; this instruction simply does nothing after popping from the stack.

### Stop Thread (`@`)

Kills the current IP. If this IP is the last active IP, the program immediately terminates with an exit code of `0`.

### Quit (`q`)

Immediately terminates the program with a specified exit code.

**Pops:**
- `code: int` — The exit code.