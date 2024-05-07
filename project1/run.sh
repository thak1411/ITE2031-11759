#!/bin/bash

# Usage: sh run.sh <file_name>
#  Example: sh run.sh test1

cd assembler && make && ./assembler $1.as $1.mc && cp $1.mc ../simulator/$1.mc && cd .. &&
    cd simulator && make && ./simulator $1.mc > $1.out.result