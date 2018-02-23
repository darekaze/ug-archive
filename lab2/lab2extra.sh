#!/bin/bash

file="config.sys"
declare -A arr           # declare associative array arr

row=0
while read -r -a line; do
  for ((col=0; col<${#line[@]}; col++)); do
    arr[$row,$col]="${line[$col]}"
  done
  ((row++))
done < "$file"

echo "${!arr[@]}"
