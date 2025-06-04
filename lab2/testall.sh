#!/bin/bash

cd ~/My_compiler/lab2/Code/
make 

files=($(ls -v ../Test/*.cmm))
for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        echo ""
        echo "running: $file"
        ./parser "$file"
    fi
done

make clean

echo "Done"

