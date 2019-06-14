# have to set initial breakpoints before setting commands to them
# its nice to organize one function's debugging all together when you can.
# using breakpoint commands (dbc) we can execute specific instructions
# when a breakpoint is hit and only manually inspect when absolutely necessary

# main
db main+0xF5
db main+0x11B
db main+0x15E


"dbc main+0xF5 !echo accepting new client"
# "dbc main+0xF5 dc"

"dbc main+0x11B !echo 'forking for child, use dpc to attach to child'"
# "dbc main+0x11b dc"

"dbc main+0x15E !echo 'closing client connection (parent)'"
# "dbc main+0x15E dc"


# allow the program to execute
dc
