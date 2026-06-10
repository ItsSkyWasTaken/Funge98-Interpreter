# Overview

**Funge98** is the successor to Befunge93. It is a family of esoteric languages, which includes the languages Unefunge98
(1D), Befunge98 (2D), and Trefunge98 (3D).

These languages (and other derived languages, known as Fungeoids), are mainly characterized by being primarily 
stack-based and by the presence of one or more "instruction pointers" (Funge's version of a program counter) that moves
around a "Funge world", or a grid of characters that represent instructions.

## Data Storage

Funge98 has two native ways of storing data: the Funge world itself and the stack of stacks. Every cell in both the
Funge world and each stack is 32 bits, which may be split or combined to store smaller or larger chunks of data as 
desired by the programmer (for example, programmers may choose to store two 16-bit integers in one cell or use two cells
to store a 64-bit integer). Each cell stores raw bits, which may be interpreted as an integer, a character, or a 
floating-point number depending on the program's needs.

### The Funge World

The Funge world is a virtually infinite array of cells that is used to store instructions and data. Depending on the
specific language, the Funge world may be 1D (Unefunge), 2D (Befunge), or 3D (Trefunge). When a program is first loaded,
the first character in the source file is located at the origin (that is, (0, 0, 0) in Trefunge), and characters 
continue to load east (positive *x*-direction). Line feed (`\n`) and/or carriage return (`\r`) characters cause the
loading pointer to return to *x* = 0 and increment *y* by 1 (south), and form feed characters (`\f`) cause the 
loading pointer to return to (0, 0, *z*) and increment *z* by 1 (down/nadir). Because negative coordinates will never be 
affected during the initial load, it's common to store persistent variables in those spaces.

In Unefunge and Befunge files, form feeds are simply treated as regular characters. Additionally, in Unefunge, newline 
sequences are not transferred into the program.

Funge worlds are effectively infinite; they are only bound by the 32-bit integer limits. As such, the theoretical limit 
of a Unefunge world is enough to fill the RAM space of a typical laptop, and the theoretical limits of Befunge and 
Trefunge worlds exceed the maximum theoretical memory on a 64-bit processor. Assuming a hypothetical device with 
infinite storage, storage limits per Funge world are as follows:
* Unefunge: 16GB, from 4,294,967,296 cells (8GB initial load, from 2,147,483,648 cells)
* Befunge: 64EB, from 18,446,744,073,709,551,616 cells (16EB initial load, from 4,611,686,018,427,387,904 cells)
* Trefunge: 256RB, from 79,228,162,514,264,337,593,543,950,336 cells (32RB initial load, from 
  9,903,520,314,283,042,199,192,993,792 cells)

Due to the massive amount of data that Funge worlds can theoretically store, worlds are typically loaded dynamically so 
computers only have to store active regions, while automatically assuming that any unloaded regions contain spaces 
(` `). This interpreter loads Funge worlds in 16-kilobyte chunks (4,096 cells), which correspond to 64×64 chunks in 
Befunge, or 16×16×16 chunks in Trefunge.

After the initial Funge file is loaded into the Funge world, a single "instruction pointer" (which may be abbreviated as
"IP" throughout these articles) spawns at the origin and points east (the positive *x*-direction). The IP's location may
be referred to as either its *location* or its *position*, and the direction that the IP is facing is referred to as its
*delta*. Each "tick", the IP executes the instruction at its position, then it travels by its delta until it comes 
across another valid instruction (that is, spaces are passed over in zero ticks). In Concurrent Funge, when there are 
multiple IP's, each IP receives a tick in an order determined by a queue (predictable, see the `t` instruction).

### The Stack

Funge98 is a stack-based language. In Befunge93, each stack would simply store numbers. However, in Funge98, stacks are 
instead implemented as stacks of stacks, which facilitate scoping. If a Funge98 program does not use the instructions 
`{`, `}`, or `u`, then the stack is effectively treated the same as the stack in Befunge93. Each IP has its own stack 
(important for Concurrent Funge), and each stack starts with a single substack. Each substack stores cells that are 32
bits each.

Most instructions operate on the stack, either by popping arguments or pushing return values (or both, or sometimes 
neither). As stated earlier, the stack stores raw sequences of 32 bits, and each instruction may interpret the retrieved
or returned bits as integers, characters, or floating-point numbers as needed.

If an instruction attempts to pop from an empty stack, the stack quietly returns a `0` without throwing an error.

## Instructions
    *Main Article: [Instructions](instructions.md)*

Funge98 instructions are each represented as a character in the Funge world. An IP starts at the origin (0, 0, 0) and 
moves around the Funge world, executing each character between the Unicode values 33–126 as it moves over them. Anything
outside of that range is ignored.

A complete list of instructions can be found with the link above. If an instruction in that range cannot be executed,
the IP flips 180°, allowing programs to handle errors.

If the instruction pointer ever reaches the edge of the world, it simply wraps back around to the other side using 
Lahey-space rules. This rule states that, assuming the delta stays the same, the IP will eventually reach where it left
off. This wrapping is achieved by this interpreter by moving the IP backwards until it reaches the other edge, ignoring
all instructions in its path. Once the other edge is reached, the IP resumes normally.

### Fingerprints
    *Main Article: [Fingerprints](fingerprints/fingerprints.md)*

By default, the instructions A–Z are unimplemented and result in a pointer reflection. Fingerprints add definitions to
some or all of these instructions. A list of supported fingerprints can be found at the link above, and a description of
the instructions for each fingerprint can be found in their respective pages.