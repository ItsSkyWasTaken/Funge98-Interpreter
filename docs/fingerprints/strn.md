# Fingerprint 0x5354524E (STRN)

The STRN fingerprint is a fingerprint provided by RCS that facilitates processing strings. Functions include joining two
strings together, comparing two strings lexicographically, `p` and `g` equivalents for strings, direct I/O stream 
interaction with strings, substrings, and conversion to and from numbers.

If any instruction attempts to pop a string from an empty stack, or otherwise a stack with `0` as its top element, an 
empty string (`""`) is returned (the `0` is removed if it exists). If it cannot find a null character that terminates 
the string, the entire stack is treated as one long string (as the negative space of the stack is padded with an 
infinite number of `0`s).

## Instruction Summary

|         Character          | Instruction Name | Parameter List   | Return List | Description                                        |
|:--------------------------:|:----------------:|:-----------------|:------------|:---------------------------------------------------|
|      [`A`](#append-a)      |      Append      | string, string   | string      | Joins two strings together.                        |
|     [`C`](#compare-c)      |     Compare      | string, string   | int         | Compares two strings.                              |
|     [`D`](#display-d)      |     Display      | string           | void        | Prints a string to the output stream.              |
|       [`F`](#find-f)       |       Find       | string, string   | string      | Finds a substring within a string.                 |
|       [`G`](#get-g)        |       Get        | vector           | string      | Gets a string from the Funge world.                |
|      [`I`](#input-i)       |      Input       | void             | string      | Takes a string from the input stream.              |
|       [`L`](#left-l)       |       Left       | string, int      | string      | Retrieves a substring from the left of a string.   |
|      [`M`](#middle-m)      |      Middle      | string, int, int | string      | Retrieves a substring from the middle of a string. |
|      [`N`](#length-n)      |      Length      | string           | string, int | Gets the length of a string.                       |
|       [`P`](#put-p)        |       Put        | string, vector   | void        | Puts a string into the Funge world.                |
|      [`R`](#right-r)       |      Right       | string, int      | string      | Retrieves a substring from the right of a string.  |
| [`S`](#number-to-string-s) | Number to String | int              | string      | Converts an integer to its string representation.  |
| [`V`](#string-to-number-v) | String to Number | string           | int         | Converts a string to its integer value.            |

## Instruction Detail

### Append (`A`)

Appends the bottom string to the upper string. This effectively removes the first zero on the stack. If the first zero
is the bottommost element of the stack, nothing happens. If there is no zero, one will appear at the bottom of the stack
after this call.

**Pops:**
- `a: string` — the bottom string
- `b: string` — the top string

**Pushes:**
- `r: string` — a string combining `b + a`

### Compare (`C`)

Compares two strings lexicographically. The result is `0` if the two strings are equal, negative if `a` comes first
lexicographically, or positive if `b` comes first.

This is equivalent to a call to `a.compare(b)` in C++, or `a.compareTo(b)` in Java.

**Pops:**
- `b: string` — the bottom string
- `a: string` — the top string

**Pushes:**
- `r: string` — the comparison result

### Display (`D`)

Prints a string to the output stream.

**Pops:**
- `str: string` — the string to print to the output stream

### Find (`F`)

Returns the longest suffix starting with the specified substring within a specified string. If the substring does not
exist in the main string, an empty string is pushed instead.

For example, `find("Hello", "l")` would result in `"llo"`: it takes the first instance of `l` and all the characters 
after it.

The first index of the substring can be figured out by subtracting the length of the returned suffix from that of the 
original larger string.

**Pops:**
- `sub: string` — the substring to look for
- `str: string` — the string to search in

**Pushes:**
- `result: string` — the longest suffix in `str` that starts with `sub`

### Get (`G`)

Gets a null-terminated string at a specified vector from the Funge space. This instruction starts at the specified 
vector and always moves east (positive *x* direction), until it hits a null character (`\0`).

This instruction is influenced by the IP's storage offset.

**Pops:**
- `location: vector` — the location to take the string from

**Pushes:**
- `str: string` — the string located at `location`

**Fails:** If there is no null character found between `location` and the eastern edge of the loaded world.

### Input (`I`)

Takes a string input from the standard input stream.

**Pushes:**
- `str: string` — the string received by the user input

### Left (`L`)

Takes the first `n` characters from the left of a string. If `n` is larger than the size of the string, the whole string
is returned.

**Pops:**
- `str: string` — the string to take from
- `n: int` — the number of characters to take

**Pushes:**
- `sub: string` — the substring taken from the left side of the `str`, `n` characters long (or the size of `str` if less
  than `n`)

**Fails:** If `n` is negative.

### Middle (`M`)

Takes a substring from the middle of a string by accepting a `position` and `length`. If `length` would go past the 
string's right bound, this instruction simply returns the entire string starting from `position`. If `length` is `0`, an
empty string is returned. If `position` is exactly equal to the length of `str`, an empty string is returned.

**Pops:**
- `str: string` — the string to take from
- `position: int` — the starting index of the substring
- `length: int` — the number of characters to take for the substring

**Pushes:**
- `sub: string` — the substring taken from the specified `position` and `length`

**Fails:** If either integer argument is negative, or if `position` is greater than the length of `str`.

### Length (`N`)

Gets the length of a string. The string remains on the stack.

**Pops:**
- `str: string` — the string to check

**Pushes:**
- `str: string` — the supplied string
- `length: int` — the length of `str`

### Put (`P`)

Places a null-terminated string into the Funge world, at a specified location. This instruction always moves east 
(positive *x* direction), placing a null character (`\0`) after the string is placed.

This instruction is influenced by the IP's storage offset.

**Pops:**
- `str: string` — the string to place
- `location: vector` — the location to place the string

**Fails:** If the string would cross over the 32-bit integer limit of the world given the supplied `location`.

### Right (`R`)

Grabs the `n` rightmost characters of a specified string and returns them as a substring. If `n` is greater than the
length of `str`, the entirety of `str` is returned.

**Pops:**
- `str: string` — the string to take from
- `n: int` — the number of characters to take

**Pushes:**
- `sub: string` — the `n` rightmost characters of `str`

**Fails:** If `n` is negative.

### Number to String (`S`)

Returns a string representation of the specified integer.

**Pops:**
- `n: int` — the number to convert

**Pushes:**
- `str: string` — the string representation of `n`

### String to Number (`V`)

Returns an integer from a string, similar to `stoi()` in C++. If the string does not represent an integer, this 
instruction simply returns `0`.

**Pops:**
- `str: string` — the string to convert

**Pushes:**
- `n: int` — the integer representation of `str`