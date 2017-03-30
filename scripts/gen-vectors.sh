#! /bin/bash
set -euo pipefail
IFS=$'\n\t'

# Convert assembly vectors table to C.

SRC_PATH="Device/SiliconLabs"

for f in "$SRC_PATH/"EF*
do
  SOURCE="$SRC_PATH/"$(basename ${f})"/Source"
  "$(dirname $0)/generate-vectors-from-arm-startup.sh" "$SOURCE/"ARM "$SOURCE/"
done
