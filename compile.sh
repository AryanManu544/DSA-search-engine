

echo "Compiling Mini Search Engine..."

g++ -std=c++14 -Wall -Wextra -O2 -o search_engine \
    main.cpp \
    Preprocessor.cpp \
    InvertedIndex.cpp \
    TFIDFCalculator.cpp \
    PerformanceTracker.cpp \
    SearchEngine.cpp

if [ $? -eq 0 ]; then
    echo "Compilation successful! Run with: ./search_engine <dataset_directory>"
else
    echo "Compilation failed!"
    exit 1
fi

