#!/bin/bash

arm-none-eabi-size $1 |tail -n1 | awk '{print "Flash: "($1+$2)/1024"KB  RAM: "($2+$3)/1024"KB"}'
