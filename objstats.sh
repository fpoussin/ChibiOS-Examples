#!/bin/bash

arm-none-eabi-size $1/build/obj/*.o | sort -n | awk '{print "Flash: "($1+$2)"\tRAM: "($2+$3)"\t\t" $6}'
