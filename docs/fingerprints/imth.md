# Fingerprint 0x494D5448 (IMTH)

The IMTH fingerprint provides 17 extra functions for integer math.

## Instruction Summary

|             Character             |    Instruction Name     | Parameter List | Return List | Description                                           |
|:---------------------------------:|:-----------------------:|:---------------|:------------|:------------------------------------------------------|
|         [`A`](#average-a)         |         Average         | int..., int    | int         | Takes the average of a list of integers.              |
|     [`B`](#absolute-value-b)      |     Absolute Value      | int            | int         | Takes the absolute value of the top integer.          |
|     [`C`](#multiply-by-100-c)     |     Multiply by 100     | int            | int         | Multiplies the top value by 100.                      |
|  [`D`](#decrease-towards-zero-d)  |  Decrease Towards Zero  | int            | int         | Decreases the absolute value of the top integer by 1. |
|    [`E`](#multiply-by-10000-e)    |   Multiply by 10,000    | int            | int         | Multiplies the top value by 10,000.                   |
|        [`F`](#factorial-f)        |        Factorial        | int            | int         | Takes the factorial of the top value.                 |
|          [`G`](#sign-g)           |          Sign           | int            | int         | Gets the sign of the top value.                       |
|    [`H`](#multiply-by-1000-h)     |    Multiply by 1,000    | int            | int         | Multiplies the top value by 1,000.                    |
| [`I`](#increase-away-from-zero-i) | Increase Away From Zero | int            | int         | Increases the absolute value of the top integer by 1. |
|       [`L`](#shift-left-l)        |       Shift Left        | int, int       | int         | Performs a leftward bitwise shift.                    |
|         [`N`](#minimum-n)         |         Minimum         | int..., int    | int         | Takes the minimum of a list of integers.              |
|       [`R`](#shift-right-r)       |       Shift Right       | int, int       | int         | Performs a rightward bitwise shift.                   |
|        [`S`](#summation-s)        |        Summation        | int..., int    | int         | Takes the sum of a list of integers.                  |
|     [`T`](#multiply-by-10-t)      |     Multiply by 10      | int            | int         | Multiplies the top value by 10.                       |
|     [`U`](#print-unsigned-u)      |     Print Unsigned      | int            | void        | Prints the top value as an unsigned integer.          |
|         [`X`](#maximum-x)         |         Maximum         | int..., int    | int         | Takes the maximum of a list of integers.              |
|         [`Z`](#negate-z)          |         Negate          | int            | int         | Multiplies the top value by -1.                       |

## Instruction Detail

### Average (`A`)

Takes the average of `count` integers. Any decimal portion of the result is truncated.

If `count` is zero, this instruction returns `0`. If `count` is greater than the stack size, the remaining space is 
filled with zeros.

**Pops:**
- `values: int...` — the list of integers to take the average of
- `count: int` — the length of `values`

**Pushes:**
- `avg: int` — the average of the integers in `values`

**Fails:** If `count` is negative; `count` is pushed back onto the stack.

### Absolute Value (`B`)

Takes the absolute value of the top value.

If the top value is `-2147483648` (the minimum integer value), the result will still be `-2147483648` due to integer 
overflow.

**Pops:**
- `n: int` — the operand

**Pushes:**
- `r: int` — the absolute value of `n`

### Multiply by 100 (`C`)

Multiplies the top value by 100. An integer overflow/underflow causes a wrap-around.

**Pops:**
- `n: int` — the value to multiply

**Pushes:**
- `r: int` — `n` multiplied by 100

### Decrease Towards Zero (`D`)

Decreases the absolute value of `n` if it's nonzero. If `n` is zero, this instruction effectively does nothing (pushes
`0`).

**Pops:**
- `n: int` — the value to decrease

**Pushes:**
- `r: int` — a value one step closer to zero

### Multiply by 10,000 (`E`)

Multiplies the top value by 10,000. An integer overflow/underflow causes a wrap-around.

**Pops:**
- `n: int` — the value to multiply

**Pushes:**
- `r: int` — `n` multiplied by 10,000

### Factorial (`F`)

Takes the factorial of the top value. An integer overflow causes a wrap-around. Due to this, any `n` above 33 will 
result in an `r` of zero.

**Pops:**
- `n: int` — the operand

**Pushes:**
- `r: int` — the factorial of `n`

**Fails:** If `n` is negative; `n` is pushed back onto the stack.

### Sign (`G`)

Takes the sign of the top value: returns `1` if `n` is positive, `-1` if `n` is negative, or `0` if `n` is zero.

**Pops:**
- `n: int` — the operand

**Pushes:**
- `r: int` — the sign of `n`

### Multiply by 1,000 (`H`)

Multiplies the top value by 1,000. An integer overflow/underflow causes a wrap-around.

**Pops:**
- `n: int` — the value to multiply

**Pushes:**
- `r: int` — `n` multiplied by 1,000

### Increase Away From Zero (`I`)

Increases the absolute value of `n` if it's nonzero. 

If `n` is zero, this instruction effectively does nothing (pushes `0`). If `n` is either the min value or max value, 
such that an increase would cause an underflow or overflow, the value wraps around, so that the result is the max value 
or min value, respectively.

**Pops:**
- `n: int` — the value to increase

**Pushes:**
- `r: int` — a value one step farther from zero

### Shift Left (`L`)

Performs a bitwise shift on `n`, `c` units to the left.

If `c` is negative, `n` is shifted to the right instead. If `c >= 32` (or `c <= -32`), `r` will be zero.

**Pops:**
- `n: int` — the value to shift
- `c: int` — the shift amount

**Pushes:**
- `r: int` — the result of the shift operation

### Minimum (`N`)

Takes the minimum of `count` integers. If `count` is greater than the stack size, the remaining slots are filled with 
zeros.

**Pops:**
- `values: int...` — a list of integers to take a minimum from
- `count: int` — the length of `values`

**Pushes:**
- `min: int` — the lowest of the integers in `values`

**Fails:** If `count` is not positive; `count` is pushed back onto the stack.

### Shift Right (`R`)

Performs a bitwise shift on `n`, `c` units to the right.

If `c` is negative, `n` is shifted to the left instead. If `c >= 32` (or `c <= -32`), `r` will be zero.

**Pops:**
- `n: int` — the value to shift
- `c: int` — the shift amount

**Pushes:**
- `r: int` — the result of the shift operation

### Summation (`S`)

Takes the sum of `count` integers.

If `count` is zero, this instruction returns `0`. If `count` is greater than the stack size, the remaining space is
filled with zeros (and as such, those values do not affect the result).

**Pops:**
- `values: int...` — the list of integers to take the sum of
- `count: int` — the length of `values`

**Pushes:**
- `avg: int` — the sum of the integers in `values`

**Fails:** If `count` is negative; `count` is pushed back onto the stack.

### Multiply by 10 (`T`)

Multiplies the top value by 10. An integer overflow/underflow causes a wrap-around.

**Pops:**
- `n: int` — the value to multiply

**Pushes:**
- `r: int` — `n` multiplied by 10

### Print Unsigned (`U`)

Prints the top cell of the stack as an unsigned integer.

**Pops:**
- `n: int` — the value to print

### Maximum (`X`)

Takes the maximum of `count` integers. If `count` is greater than the stack size, the remaining slots are filled with
zeros.

**Pops:**
- `values: int...` — a list of integers to take a maximum from
- `count: int` — the length of `values`

**Pushes:**
- `min: int` — the highest of the integers in `values`

**Fails:** If `count` is not positive; `count` is pushed back onto the stack.

### Negate (`Z`)

Negates the top value of the stack (multiplies it by `−1`).

If the top value is `-2147483648` (the minimum integer value), the result will still be `-2147483648` due to integer
overflow.

**Pops:**
- `n: int` — the value to negate

**Pushes:**
- `r: int` — the negated value