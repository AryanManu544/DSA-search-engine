
#!/usr/bin/env bash

set -e

echo "Compiling Mini Search Engine..."

g++ -std=c++14 -Wall -Wextra -O2 -o search_engine \
    src/cpp/main.cpp \
    src/cpp/Preprocessor.cpp \
    src/cpp/InvertedIndex.cpp \
    src/cpp/TFIDFCalculator.cpp \
    src/cpp/PerformanceTracker.cpp \
    src/cpp/SearchEngine.cpp

echo "Compilation successful! Run with: ./search_engine <dataset_directory>"
