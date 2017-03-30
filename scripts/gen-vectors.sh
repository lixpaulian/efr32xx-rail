#! /bin/bash
set -euo pipefail
IFS=$'\n\t'

# Convert assembly vector tables to C.

SRC_PATH="platform/Device/SiliconLabs"

for f in "$SRC_PATH/"EF*
do
  SOURCE="$SRC_PATH/"$(basename ${f})"/Source"
  "$(dirname $0)/generate-vectors-from-arm-startup.sh" "$SOURCE/"ARM "$SOURCE/"
done
