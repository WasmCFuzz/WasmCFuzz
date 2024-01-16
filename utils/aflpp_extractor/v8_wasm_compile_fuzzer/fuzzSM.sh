#!/bin/bash


SOURCE_DIR=$1
FUZZOUT_DIR=$2
PARALLE_NUMBER="$3"
CURRENT_DIR=$(cd `dirname $0`; pwd)

if [[ -z "$1" || -z "$2" || -z "$3" ]]; then
    echo "Error: Missing required arguments."
    echo "Usage: ./fuzzWasm.sh SOURCE_DIR FUZZOUT_DIR PARALLE_NUMBER"
    echo "e.g:   ./fuzzWasm.sh /home/seeds/seed /data/fuzzout/WASM/JSC_0621 5"
    exit 1
fi

eval "cd /sys/devices/system/cpu;echo performance | tee cpu*/cpufreq/scaling_governor"
eval "cd ${CURRENT_DIR}"

FUZZER_DIR="/data/AFLplusplus-4.07c"
TARGET_PATH="/home/gecko-dev/js/src/afl/dist/bin/js"
PARENT_DIR=$(dirname "${SOURCE_DIR}")
ARGS="--wasm-moz-intgemm --wasm-memory-control --wasm-multi-memory --wasm-function-references --wasm-verbose --wasm-test-serialization --wasm-gc"
# Create the destination DIRs for the X copies
for ((i=1; i<=PARALLE_NUMBER; i++)); do
    DEST_DIR="${PARENT_DIR}/copy_${i}"
    if [ -d "$DEST_DIR" ]; then
  	echo "Folder already exists!"
  	eval "rm -r ${DEST_DIR}"
    fi
    mkdir "${DEST_DIR}"
done
# Copy 100 files to each destination DIR
for ((i=1; i<=PARALLE_NUMBER; i++)); do
    DEST_DIR="${PARENT_DIR}/copy_${i}"
    FILES=$(ls "${SOURCE_DIR}" | shuf -n 512)
    for file in ${FILES}; do
        cp "${SOURCE_DIR}/${file}" "${DEST_DIR}"
    done
    if [ $i -eq 1 ];then
    	eval "nohup ${FUZZER_DIR}/afl-fuzz -S sm_1 -L 1 -g 1 -G 512 -t 800+ -i ${PARENT_DIR}/copy_1 -o ${FUZZOUT_DIR} ${TARGET_PATH} ${ARGS} @@ > /dev/null 2>&1 &" 
    elif [ $i -eq 2 ];then
        eval "nohup ${FUZZER_DIR}/afl-fuzz -S sm_2 -L 1 -g 1 -G 512  -t 800+ -i ${PARENT_DIR}/copy_2 -o ${FUZZOUT_DIR} ${TARGET_PATH} ${ARGS} @@ > /dev/null 2>&1 &"
    else
	eval "nohup ${FUZZER_DIR}/afl-fuzz -S sm_${i} -L 1 -g 1 -G 512 -t 800+  -i ${PARENT_DIR}/copy_${i} -o ${FUZZOUT_DIR} ${TARGET_PATH} ${ARGS} @@ > /dev/null 2>&1 &"
    fi
done

while true; do
    sleep 10
    eval "${FUZZER_DIR}/afl-whatsup ${FUZZOUT_DIR}"
    sleep 60
done

