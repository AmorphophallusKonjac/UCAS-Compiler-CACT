#!/bin/bash

prj_dir="/home/szx/Documents/cact" 

compiler="$prj_dir/build/compiler"

cact_dir="$prj_dir/test/samples_lex_and_syntax"

for file in "$cact_dir"/*.cact; do
    if [[ -f "$file" ]]; then
        filename=$(basename "$file")
        ans=$(echo "$filename" | grep -E "[0-9][0-9]_(true|false)" -o | grep -E "true|false" -o)
        out=$($compiler $file 2>/dev/null | tail -n 1 | grep -E "true|false")
        if [[ "$out" != "$ans" ]]; then
            echo syntax test failed at $filename
            echo ans is "$ans"
            $compiler $file
            exit 1
        fi
    fi
done

echo syntax test pass!

exit 0