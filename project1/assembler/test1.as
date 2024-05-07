        lw     0       7       one      load one to reg7
loop    lw     0       1       i        load i
        lw     0       2       n        load n
        beq    1       2       end      if n == i then goto end
        lw     0       3       sum      load sum
        add    1       3       3        sum = sum + i
        sw     0       3       sum      store sum
        add    1       7       1        i = i + 1
        sw     0       1       i        store i
        beq    0       0       loop     goto loop
end     halt
one     .fill   1
i       .fill   1
n       .fill   10
sum     .fill   0
