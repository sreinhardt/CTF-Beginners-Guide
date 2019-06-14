# have to set initial breakpoints before setting commands to them
# its nice to organize one function's debugging all together when you can.
# using breakpoint commands (dbc) we can execute specific instructions
# when a breakpoint is hit and only manually inspect when absolutely necessary

# stop on fork()
e dbg.forks = true

# allow parent to run, breaking on exec of fork()
dc

# attach to child process
dpc


# DONT SET THESE BEFORE CHILD FORKS
# main
db main+0x140
db main+0x14E

"dbc main+0x140 !echo 'closing listener socket (child)'"
# "dbc main+0x140 dc"

"dbc main+0x14E !echo 'starting readInput() for client'"
# "dbc main+0x14E dc"


# readInput
db sym.readInput+0xB8
db sym.readInput+0x54
db sym.readInput+0xBE
db sym.readInput+0x6C
db sym.readInput+0x7C
db sym.readInput+0x8C

"dbc sym.readInput+0xB8 !echo entering main game loop"
# "dbc sym.readInput+0xB8 dc"

"dbc sym.readInput+0x54 dr eax; !echo player choice"
# "dbc sym.readInput+0x54 dr eax; dc"

"dbc sym.readInput+0xBE !echo calling checksec"
# "dbc sym.readInput+0xBE dc"

"dbc sym.readInput+0x6C !echo calling pokemans"
# "dbc sym.readInput+0x6C dc"

"dbc sym.readInput+0x7C !echo calling heckers"
# "dbc sym.readInput+0x7C dc"

"dbc sym.readInput+0x8C !echo calling batmenss"
# "dbc sym.readInput+0x8C dc"

# checksec
db sym.checksec+0xF
db sym.checksec+0x14
db sym.checksec+0x1B
db sym.checksec+0x1D
db sym.checksec+0x5E

"dbc sym.checksec+0xF dr edx; !echo global h val"
# "dbc sym.checksec+0xF dr edx; dc"

"dbc sym.checksec+0x14 dr eax; !echo global b val"
# "dbc sym.checksec+0x14 dr eax; dc"

"dbc sym.checksec+0x1B dr eax; !echo global p val"
# "dbc sym.checksec+0x1B dr eax; dc"

"dbc sym.checksec+0x1D dr eax; !echo are all enabled"
# "dbc sym.checksec+0x1D dr eax; dc"

"dbc sym.checksec+0x5E dr eax; ps+ @ eax; !echo sending flag"
# "dbc sym.checksec+0x5E dr eax; ps+ @ eax; dc"


# heckers
db sym.hekers+0x34
db sym.hekers+0x74

"dbc sym.hekers+0x34 dr eax; px 40 @ eax; !echo heckers buf"
# "dbc sym.hekers+0x34 dr eax; px 40 @ eax; dc"

"dbc sym.hekers+0x74 dr eax; !echo zeroweekend val"
# "dbc sym.hekers+0x74 dr eax; dc"

# batmenss
db sym.batmenss+0x34
db sym.batmenss+0x74

"dbc sym.batmenss+0x34 dr eax; px 40 @ eax; !echo batmenss buf"
# "dbc sym.batmenss+0x34 dr eax; px 40 @ eax; dc"

"dbc sym.batmenss+0x74 dr eax; !echo batsignet val"
# "dbc sym.batmenss+0x74 dr eax; dc"

# pokemans
db sym.pokemans+0x34
db sym.pokemans+0x74

"dbc sym.pokemans+0x34 dr eax; px 40 @ eax; !echo pokemans buf"
# "dbc sym.pokemans+0x34 dr eax; px 40 @ eax; dc"

"dbc sym.pokemans+0x74 dr eax; !echo pikachy val"
# "dbc sym.pokemans+0x74 dr eax; dc"

# allow the program to execute
dc
