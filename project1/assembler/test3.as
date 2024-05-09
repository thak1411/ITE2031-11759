        lw     0       7       oneof    load one offset to reg7
        lw     7       7       oneof    load one to reg7
        lw     0       4       subAdd   4 = sub
        jalr   4       6                call sub
        lw     0       3       b        load b
        lw     0       7       oneofa   load oneof address to reg7
        lw     7       7       4        load one to reg7 with constant offset
        add    3       7       3        b + 1
        sw     0       3       b        b = b + 1
        jalr   4       6                call sub
        halt
sub     lw     0       1       a        load a
        lw     0       2       b        load b
        nor    0       2       3        reg3 = ~b
        add    7       3       3        reg3 = -b
        add    1       3       3        a - b
        sw     0       3       a        a = a - b
        jalr   6       5                return
a       .fill   5
b       .fill   -3
oneofa  .fill   oneof
oneof   .fill   4
        noop
        noop
        noop
one     .fill   1
subAdd  .fill   sub
