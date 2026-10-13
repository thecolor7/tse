#!/bin/bash

#loops through all tests - more can be added
for check in testQput testQapply testQremove #add more tests here
do
#this check sends valgrind to stdout and checks for 0 errors to come through
if valgrind --leak-check=full "$check" 2>&1 | grep -q "ERROR SUMMARY: 0"; then
    echo ""$check" passed"
else
    echo ""$check" failed"
fi
done
