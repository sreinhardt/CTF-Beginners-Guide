#### CTF Beginners Guide \\ Reversing with Radare2
###### Talos Intelligence
###### Spenser Reinhardt

[Trail of Bits: EasyServer](https://github.com/trailofbits/ctf/tree/master/exploits/binary1_workshop/easy)

###### Local Files

| Path | Usage |
|-----|-----|
| CTF_Beginners_Guide.pptx | This presentation |
| binaries/easy32 | Challenge Binary (32-bit Linux ELF statically linked) |
| binaries/easy_key | Challenge flag (place in same dir as easy32) |
| source/EasyServer.c | Challenge C source |
| source/makefile | Compile new easy32 with `make all` |
| analysis/easy32.md | Plaintext analysis of easy32 binary |
| analysis/easy32.i64 | IDA Pro 7.2 (64-bit) analysis database |
| analysis/easy32.bndb | Binary Ninja analysis database |
| analysis/easy32-r2db/rc | Radare2 analysis project |
| analysis/easy32-parent-manual.r2 | Radare2 debug script for parent process, stops on breakpoints for inspection |
| analysis/easy32-child-manual.r2 | Radare2 debug script for child process, stops on breakpoints for inspection |
| analysis/easy32-parent-auto.r2 | Radare2 debug script for parent process, automates debugging (some dbc commands may fail) |
| analysis/easy32-child-auto.r2 | Radare2 debug script for child process, automates debugging (some dbc commands may fail) |
| exploitation/just_answer.py | Script sending single correct answer |
| exploitation/40bytes.py | Script sending 40 bytes, incorrectly overflowing |
| exploitation/exploit.py | Script correctly overflowing all game questions |
| extra/radare2rc | Radare2 configuration profile |
