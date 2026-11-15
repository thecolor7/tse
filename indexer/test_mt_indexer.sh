#!/bin/bash
THREAD_COUNT=$1
TARGET_OUTPUT="mt_index.out"
REFERENCE_OUTPUT="CorrectOutput.txt"
PAGEDIR="../crawler/pages"

# Ensure the user provided a valid thread count between 0 and 16
if [ -z "$1" ]; then
    echo "Error: Missing thread count argument."
    echo "Usage: ./test_mt_indexer.sh <thread_count> (0-16)"
    exit 1
fi

if ! [[ "$1" =~ ^[0-9]+$ ]] || [ "$1" -gt 16 ]; then
    echo "Error: Thread count must be an integer between 0 and 16."
    echo "Usage: ./test_mt_indexer.sh <thread_count> (0-16)"
    exit 1
fi

# Printing to notify tester that the test is initiating
echo "Running test file for multithreaded indexer implementation with a threadcount of $THREAD_COUNT"
echo "Running valgrind with target output $TARGET_OUTPUT"

# Running Valgrind
# Exit and return that valgrind failed if leaks are detected
valgrind --error-exitcode=1 --leak-check=full ./mt_indexer $PAGEDIR $TARGET_OUTPUT $THREAD_COUNT > /dev/null 2> valgrind_log.txt

if [ $? -eq 0 ]; then
    echo "SUCCESS: Valgrind reported no possible memory leaks"
else
    echo "FAILURE: Valgrind reported memory leaks"
    cat valgrind_log.txt 
    rm -f valgrind_log.txt
    exit 1
fi

# Comparing output files
echo "Now to compare the output files..."

# Normalzing and sorting for accurate comparison
awk '{ for(i=2; i<=NF; i+=2) print $1, $i, $(i+1) }' $REFERENCE_OUTPUT | sort > sorted_correct.txt
awk '{ for(i=2; i<=NF; i+=2) print $1, $i, $(i+1) }' $TARGET_OUTPUT | sort > sorted_mt.txt

if cmp -s sorted_correct.txt sorted_mt.txt; then
    echo "SUCCESS: The output of the multithreaded indexer does match the reference output of the module 5 indexer"
else
    echo "FAILURE: The output of the multithreaded indexer does not match the reference output from the module 5 indexer"
fi

# Deleting temporary files
rm -f valgrind_log.txt sorted_correct.txt sorted_mt.txt
