#!/usr/bin/env bash
set -e

INPUT="$1"
OUTPUT="$2"

xxd -i -n "${INPUT##*/}" "$INPUT" "$OUTPUT"

sed -i '1s/^/#pragma once\n\nnamespace Font {\n\n/' "$OUTPUT"
sed -i 's/^unsigned char \(.*\)_ttf\[\]/inline const unsigned char \1\[\]/' "$OUTPUT"
sed -i 's/^unsigned int \(.*\)_ttf_len/inline const unsigned int \1_Size/' "$OUTPUT"

echo -e "\n}" >> "$OUTPUT"
