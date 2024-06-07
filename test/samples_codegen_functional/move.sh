#!/bin/bash
set -x
sample_dir=$(pwd)
for file in "$sample_dir"/*.out; do
    if [[ -f "$file" ]]; then
        mv $file "$sample_dir"/out
    fi
done