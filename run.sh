#!/usr/bin/env bash
#
# Usage:
#   ./run.sh [PY_WORKERS] [CPP_WORKERS] [INPUT_FILE] [OUTPUT_FILE]
#
# Example:
#   ./run.sh 2 4 data/test.json data/output.txt

PY_WORKERS=${1:-4}
CPP_WORKERS=${2:-4}
INPUT_FILE=${3:-data/default_in.json}
OUTPUT_FILE=${4:-data/default_out.txt}

# Build the C++ program
cd cpp_main
mkdir -p build
cd build
cmake ..
make

cd ..
# Run Python helper in the background
python3 ../python_helper/main.py "$PY_WORKERS" &

sleep 1

cd build

# Run main_program with optional arguments
./main_program -w "$CPP_WORKERS" -i "$INPUT_FILE" -o "$OUTPUT_FILE"