#!/usr/bin/env bash

set -o pipefail

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 FUNCTION_NAME PROGRAM [ARGS...]" 1>&2
    exit 1
fi

function_name=$1
shift 1

echo "${function_name}"

events=(
    cache-references:u
    cache-misses:u
    L1-dcache-loads:u
    L1-dcache-load-misses:u
    LLC-loads:u
    LLC-load-misses:u
)
for event in "${events[@]}"; do
    printf "${event}: "
    perf record -e "${event}" -F 10000 -- $@ > /dev/null 2>&1
    perf report -F overhead,period,sym --stdio | grep -- "${function_name}" | tr -s ' ' | cut -d ' ' -f 3
    if [ "$?" -ne 0 ]; then
        echo "No sample"
    fi
done
