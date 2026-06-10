# Funge98

This program is yet another Funge98 interpreter, this time written in C++.

I started learning C++ just under a year ago but haven't really gotten to do much with it since then. Then later, I came
across the concept of esoteric programming languages, and I felt compelled to write an interpreter for one, so here we 
are!

More information on Funge98 can be found in the
[Funge-98 Final Specification](https://github.com/catseye/Funge-98/blob/master/doc/funge98.markdown), which details the
specifications that should ideally apply across all interpreters. Additionally, details on Funge98 may be found 
[here](docs/funge98-overview.md), including interpreter-specific specifications that may have arisen due to ambiguity 
from the source specifications (from either the base language or fingerprints).

## Interpreter Statistics

This interpreter is still a WIP. These are some issues in the current version (v0.3.0) that will be resolved in the 
future:

- Replacing a character at the edge of the active region with a space, such that there is no non-space character on the
  edge anymore, does not shrink the active region as reported by `y`.
- Anything outside the 32–127 range is ignored instead of causing a reflection

These are the details that are received as a result of the GetSysInfo (`y`) command:

- Concurrent Funge98 (`t`) enabled: **Y**
- Input (`i`) enabled: **N** by default; must specify `--read`, `-r`, or `-i` for **Y**
- Output (`o`) enabled: **N** by default; must specify `--write`, `-w`, or `-o` for **Y**
- Execute (`=`) enabled: **N** by default; must specify `--execute` or `-e` for **Y**
- I/O: **buffered**
- Cell size: **32-bit**
- Handprint: **1230198612** (`0x49535754`)
- Version: v0.3.0 (`300`)
- Operating paradigm: Equivalent to C-language `system()`

## Running a Funge98 File

**Note:** The compiled executable is currently only provided for Windows. Since this program uses only standard C++ 
libraries and no third-party libraries, the source code should compile and run as intended on any platform, but for now,
macOS and Linux users will need to download the source code (provided in a ZIP) and compile it themselves with a C++26 
compiler.

Funge98 files can be run from the command line by specifying the file name as the first argument:  
```
funge98 example.b98
```

By default, this interpreter starts in a "sandbox mode", and the input (`i`), output (`o`), and execute (`=`) commands
are disabled for security. Additionally, as fingerprint support gets added, any fingerprint command that relates to 
executing system commands or reading, creating, modifying, or deleting files will also be disabled by default. To enable
them for a program, you must use flags when running the Funge98 file:
- To enable `i` and related fingerprint instructions (anything that can read files or folders), use `--read`, `-r`, or 
  `-i`.
- To enable `o` and related fingerprint instructions (anything that can create, modify, or delete files or folders), use
  `--write`, `-w`, or `-o`.
- To enable `=` and related fingerprint instructions (anything that can interact with system-level data or execute 
  commands), use `--execute` or `-e`.

These flags may be specified in any order, but they must appear after the Funge98 file. For example, both of these work:  
```
funge98 example.b98 -r -e 
funge98 example.b98 -e -r
```

Additionally, this interpreter was set up to automatically detect the dimensions of the supplied file and load it as a
Unefunge, Befunge, or Trefunge program. Any file that has only one line is loaded as a Unefunge program, any file that 
has a form feed character, or a line with `\f` (and nothing else, including whitespace), will be loaded as a Trefunge 
program, and any file that does not fit either will be loaded as a Befunge program. The file extension is not 
considered. To override the default, use `--dim=VALUE`. This flag may also appear in any order alongside the permission 
flags, but it must also appear after the Funge98 file:  
```
funge98 example.b98 -r --dim=3
```

If the Funge98 file supports extra arguments (which can be obtained either through `y` as the 19th entry or with 
fingerprint instructions), these can be passed after the `--args` flag. This flag **must** be the last flag to appear,
and anything else after it will be considered a program argument. When reading arguments, the Funge98 file is treated as
the first argument. 

For example, this command runs a Funge98 file called `example.b98`, granting it reading permissions and specifying that
it's a Befunge file. The arguments listed will be [`example.b98`, `Example`, `Another Example`, `-w`]. Note that writing
permissions were not granted since `-w` was passed *after* the `--args` flag, so it was treated like a normal 
argument.
```
funge98 example.b98 -r --dim=2 --args Example "Another Example" -w
```