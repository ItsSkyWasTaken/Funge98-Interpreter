# Fingerprints

Fingerprints are similar to modules in other programming languages. They can be loaded ("imported") to allow the IP to 
access extra instructions represented by one or more of the uppercase letters from A to Z. Instructions that read or 
write to files or execute system commands still require their respective permission flags enabled when the program 
starts. Fingerprints are loaded with `(` and unloaded with `)`. Each IP has its own set of imported fingerprints, which 
is cloned whenever `t` is executed.

Supported fingerprints are baked directly into this interpreter in native C++ code, allowing faster execution times and 
better portability than Minifunge implementations. This interpreter will gradually gain more support for the major 
fingerprints provided by RCS, RC/Funge, and Cats-Eye. Custom fingerprints can be emulated on this interpreter by using 
the MACR (macros) or SUBR (subroutine) fingerprints when they gain support.

Below is a current list of supported fingerprints. The functions that each fingerprint offers can be found in each of 
their respective pages.

|    Fingerprint ID     | String Name | Origin | Description                    |
|:---------------------:|:-----------:|:------:|:-------------------------------|
| [0x424F4F4C](bool.md) |    BOOL     |  RCS   | Bitwise operations.            |
| [0x5354524E](strn.md) |    STRN     |  RCS   | Functions for string handling. |