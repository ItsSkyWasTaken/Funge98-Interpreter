# Fingerprint 0x424F4F4C (BOOL)

The BOOL fingerprint is a fingerprint provided by RCS that provides four bitwise operations to facilitate bitmasking.

Bitwise operations are operations that use the raw binary patterns of integers. Details about each operation are 
described in their own sections below. Other operations can be achieved by sequencing multiple of these operations; for
example, NAND can be achieved by the sequence `AN`.

## Instruction Summary

|   Character   | Instruction Name | Parameter List | Return List | Description                                   |
|:-------------:|:----------------:|:---------------|:------------|:----------------------------------------------|
| [`A`](#and-a) |       And        | int, int       | int         | Performs a bitwise AND on the top two values. |
| [`N`](#not-n) |       Not        | int            | int         | Performs a bitwise NOT on the top value.      |
| [`O`](#or-o)  |        Or        | int, int       | int         | Performs a bitwise OR on the top two values.  |
| [`X`](#xor-x) |       Xor        | int, int       | int         | Performs a bitwise XOR on the top two values. |

## Instruction Detail

### And (`A`)

Performs a bitwise AND on the top two values. This operation merges the bit patterns of the two numbers such that any 
`1`s that overlap are recorded as `1`s, and the rest become `0`s. For example, `12 & 10 == 8`:
```
  1100 
& 1010
______
  1000  
```

**Pops:**
- `a: int` — the first operand
- `b: int` — the second operand

**Pushes:**
- `r: int` — the result of `a & b`

### Not (`N`)

Performs a bitwise NOT on the top value. This operation simply inverts the bit pattern, yielding −*n* − 1:
```
~ 00000000000011001111001011110001 ( 848625)
__________________________________
  11111111111100110000110100001110 (−848626)
```

**Pops:**
- `n: int` — the operand

**Pushes:**
- `r: int` — the result of `~n`

### Or (`O`)

Performs a bitwise OR on the top two values. This operation merges the bit patterns of the two numbers such that any
`0`s that overlap are recorded as `0`s, and the rest become `1`s. For example, `12 | 10 == 14`:
```
  1100 
| 1010
______
  1110  
```

**Pops:**
- `a: int` — the first operand
- `b: int` — the second operand

**Pushes:**
- `r: int` — the result of `a | b`

### Xor (`X`)

Performs a bitwise XOR on the top two values. This operation merges the bit patterns of the two numbers such that any
numbers that are the same are recorded as `0`s, and any that are different are `1`s. For example, `12 ^ 10 == 6`:
```
  1100 
^ 1010
______
  0110  
```

**Pops:**
- `a: int` — the first operand
- `b: int` — the second operand

**Pushes:**
- `r: int` — the result of `a ^ b`