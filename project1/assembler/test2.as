        lw     0       7       one      load one to reg7
        lw     0       4       subAdd   4 = sub
        jalr   4       6                call sub
        lw     0       3       b        load b
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
one     .fill   1
subAdd  .fill   sub
