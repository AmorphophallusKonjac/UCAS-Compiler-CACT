#!/bin/bash
sample_dir=$(pwd)
for file in "$sample_dir"/*.out; do
    if [[ -f "$file" ]]; then
        mv $file "$sample_dir"/out
    fi
done

for file in "$sample_dir"/*.cact; do
    if [[ -f "$file" ]]; then
        mv $file "$sample_dir"/cact
    fi
done

for file in "$sample_dir"/*.in; do
    if [[ -f "$file" ]]; then
        mv $file "$sample_dir"/in
    fi
done