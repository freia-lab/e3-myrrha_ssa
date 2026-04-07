#!/usr/bin/env bash

# Usage: ./replace.sh /path/to/directory REPLACEMENT

DIR="$1"
PRFX="$2"

if [[ -z "$DIR" || -z "$PRFX" ]]; then
  echo "Usage: $0 /path/to/directory REPLACEMENT"
  exit 1
fi

# Escape characters that sed cares about
ESCAPED=$(printf '%s\n' "$PRFX" | sed 's/[\/&$]/\\&/g')

find "$DIR" -type f -name "*.bob" -print0 | while IFS= read -r -d '' file; do
  sed -i "s/CAB-/${ESCAPED}/g" "$file"
done

echo "Replacement complete."
