#!/bin/bash

cd ~/My_compiler/lab3/Code/
make 

files=($(ls -v ../Test/*.cmm))
for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        # 提取文件名（不包含路径和扩展名），用于构建输出文件名
        base_name=$(basename "$file" .cmm)
        output_file="../ir/${base_name}.ir"
        
        echo "running:./parser $file $output"
        ./parser "$file" "$output_file"
    fi
done
cp ./parser ../
make clean

echo "Done"

