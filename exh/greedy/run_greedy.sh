#!/bin/bash

# Compiles the main program.
g++ -Wall -O3 -std=c++17 greedy.cc -o greedy

# Checks whether compilation was successful.
if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi

# Path to the folder containing the query files.
query_folder="./new_benchs"

# List of queries.
queries=("$query_folder/easy-1.txt" "$query_folder/easy-2.txt" "$query_folder/easy-4.txt" "$query_folder/easy-6.txt" "$query_folder/hard-1.txt" "$query_folder/hard-3.txt" "$query_folder/hard-5.txt" "$query_folder/hard-7.txt" "$query_folder/med-2.txt" "$query_folder/med-4.txt" "$query_folder/med-6.txt"
         "$query_folder/easy-3.txt" "$query_folder/easy-5.txt" "$query_folder/easy-7.txt" "$query_folder/hard-2.txt" "$query_folder/hard-4.txt" "$query_folder/hard-6.txt" "$query_folder/med-1.txt" "$query_folder/med-3.txt" "$query_folder/med-5.txt" "$query_folder/med-7.txt")

# Path to program.
program="./greedy"

# Path to the database.
database="data_base.txt"

# Output directory.
output_directory="output_files"

# Creates the output directory if it doesn't exist.
mkdir -p "$output_directory"

# Time limit for each execution (in seconds).
execution_duration=5  # 3 minutes.

# Loops through each query file.
for query in "${queries[@]}"; do
    # Formulates the output file named based on the query file name.
    output_file="${output_directory}/$(basename "$query")"

    # Identifies the query being executed.
    echo "Running $query..."

    # Runs your program with the current query file and saves the output to the corresponding output file.
    timeout "$execution_duration" $program $database "$query" "$output_file"
done
