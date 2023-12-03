#!/bin/bash

ec=0
i=0
while [ $ec -eq 0 ]
do
    i=$((i+1))
    echo "Run: $i"
    $@
    ec=$?
done
