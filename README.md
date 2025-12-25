# Mini Search Engine - DSA Project

## Table of Contents

- [Project Overview](#project-overview)
- [Approach & Design](#approach--design)
- [Data Structures Used](#data-structures-used)
- [Time & Space Complexity](#time--space-complexity)
- [Features](#features)
- [How to Run](#how-to-run)
- [Usage Examples](#usage-examples)
- [Performance Report](#performance-report)
- [Architecture](#architecture)

##  Project Overview

This Mini Search Engine is a simplified version of Google that:
- Processes and indexes large text datasets
- Performs efficient full-text search
- Ranks documents using TF-IDF scoring
- Provides autocomplete and query suggestions
- Tracks performance metrics (time and memory)

Key Constraints:
- ✅ Implemented entirely from scratch
- ✅ Uses only C++ standard library
- ✅ No ML/NLP libraries
- ✅ No external search libraries or databases

## Approach & Design

### System Architecture

The search engine is built using a modular design with the following components:

1. **Preprocessor Module** (`Preprocessor.h/cpp`)
   - Handles all text preprocessing operations
   - Tokenization, lowercasing, stopword removal, punctuation removal

2. **Inverted Index Module** (`InvertedIndex.h/cpp`)
   - Core data structure for fast document retrieval
   - Maps terms to posting lists containing document IDs and term frequencies

3. **TF-IDF Calculator** (`TFIDFCalculator.h/cpp`)
   - Implements Term Frequency-Inverse Document Frequency algorithm
   - Calculates relevance scores for ranking documents

4. **Search Engine** (`SearchEngine.h/cpp`)
   - Main orchestrator component
   - Coordinates all modules to provide search functionality

5. **Performance Tracker** (`PerformanceTracker.h/cpp`)
   - Monitors execution time and memory usage
   - Provides detailed performance reports

### Design Decisions

1. **Inverted Index Structure**
   - Uses `std::unordered_map` for O(1) average-case term lookup
   - Posting lists stored as vectors for efficient iteration
   - Separate tracking of document frequencies for IDF calculation

2. **TF-IDF Implementation**
   - Normalized TF: `TF(t,d) = term_frequency / document_length`
   - Logarithmic IDF: `IDF(t) = log(total_documents / document_frequency)`
   - Score: `TF-IDF(t,d) = TF(t,d) × IDF(t)`
   - Query scoring: Sum of TF-IDF scores for all query terms

3. **Memory Optimization**
   - Documents stored separately from index
   - Posting lists use compact structures
   - IDF values cached to avoid redundant calculations

4. **Performance Optimization**
   - Single-pass index construction
   - Efficient candidate document filtering
   - Minimal memory allocations during search

##  Data Structures Used

### Core Data Structures

1. **`std::unordered_map<string, PostingList>`**
   - **Purpose**: Inverted index main structure
   - **Why**: O(1) average-case lookup for terms
   - **Location**: `InvertedIndex` class

2. **`std::vector<DocumentInfo>`**
   - **Purpose**: Posting lists for terms
   - **Why**: Efficient iteration and memory locality
   - **Location**: `PostingList` structure

3. **`std::unordered_map<int, int>`**
   - **Purpose**: Document length tracking
   - **Why**: Fast access for normalization calculations
   - **Location**: `InvertedIndex` class

4. **`std::set<string>`**
   - **Purpose**: Stopword storage
   - **Why**: O(log n) lookup for stopword checking
   - **Location**: `Preprocessor` class

5. **`std::map<string, double>`**
   - **Purpose**: IDF value caching
   - **Why**: Avoid redundant IDF calculations
   - **Location**: `TFIDFCalculator` class

6. **`std::vector<string>`**
   - **Purpose**: Document storage and token lists
   - **Why**: Simple, efficient sequential access
   - **Location**: Multiple classes

7. **`std::set<int>`**
   - **Purpose**: Candidate document collection during search
   - **Why**: Automatic deduplication
   - **Location**: `TFIDFCalculator::calculateTFIDF`

## ⏱ Time & Space Complexity

### Index Construction

**Time Complexity: O(N × M)**
- N = number of documents
- M = average number of tokens per document
- Single pass through all documents and tokens

**Space Complexity: O(T + P)**
- T = number of unique terms
- P = total number of term-document pairs (postings)

### Search Operation

**Time Complexity: O(Q × D + D log D)**
- Q = number of query terms
- D = number of candidate documents
- Breakdown:
  - O(Q × D) for TF-IDF calculation
  - O(D log D) for sorting results
- In practice, D << N due to inverted index filtering

**Space Complexity: O(D)**
- D = number of candidate documents (for result storage)

### Preprocessing

**Time Complexity: O(L)**
- L = length of text string
- Linear scan for tokenization and cleaning

**Space Complexity: O(L)**
- Temporary storage for processed tokens

### Autocomplete

**Time Complexity: O(T)**
- T = total number of terms
- Linear scan through all terms (could be optimized with Trie)

**Space Complexity: O(K)**
- K = number of suggestions returned

## ✨ Features

### Core Features (Mandatory)

**Text Preprocessing**
- Tokenization
- Lowercasing
- Stopword removal (60+ common English stopwords)
- Punctuation and special character removal

**Inverted Index Construction**
- Efficient index building from documents
- Term → Document mapping with frequencies
- Document frequency tracking

**Query Processing & Ranking**
- Multi-term query support
- TF-IDF based ranking
- Manual TF-IDF implementation

**Performance Tracking**
- Search time measurement
- Memory usage tracking
- Detailed performance reports

### Optional Enhancements (Bonus)

**Autocomplete**
- Prefix-based term suggestions
- Sorted by document frequency

**Query Suggestions**
- History-based query recommendations
- Keyword similarity matching

## How to Run

### Prerequisites

- C++ compiler with C++14 support (GCC 5+, Clang 3.8+)
- Unix-like system (Linux/macOS) for directory reading
- wget or curl (for downloading dataset)
- 20 Newsgroups Dataset (can be downloaded automatically)

### Compilation

```bash
# Option 1: Use the provided compile script
./compile.sh

# Option 2: Compile manually
g++ -std=c++14 -Wall -Wextra -O2 -o search_engine \
    main.cpp Preprocessor.cpp InvertedIndex.cpp \
    TFIDFCalculator.cpp PerformanceTracker.cpp SearchEngine.cpp
```

### Downloading the 20 Newsgroups Dataset

```bash
# Download and extract the dataset automatically
./download_dataset.sh

# This will:
# - Download the 20news-18828.tar.gz file (~1.4 MB compressed, ~20 MB uncompressed)
# - Extract it to ./20news-18828/
# - Verify the dataset structure
```

**Manual Download:**
If the script doesn't work, download manually from:
- http://qwone.com/~jason/20Newsgroups/20news-18828.tar.gz
- Extract: `tar -xzf 20news-18828.tar.gz`

### Running

```bash
# Run with dataset directory (recursively reads all subdirectories)
./search_engine <path_to_20newsgroups_directory>

# Examples
./search_engine ./20news-18828
./search_engine ./20news-bydate-train

# If dataset is in default location, you can run without arguments:
./search_engine  # (will auto-detect ./20news-18828)
```

### Interactive Commands

Once running, use these commands:

- `search <query>` - Search for documents
  - Example: `search machine learning algorithms`
  
- `autocomplete <prefix>` - Get autocomplete suggestions
  - Example: `autocomplete mach`
  
- `suggest <query>` - Get query suggestions from history
  - Example: `suggest learn`
  
- `stats` - Show index statistics
  
- `performance` - Show performance report
  
- `help` - Show help message
  
- `quit` or `exit` - Exit the program

### Clean Build

```bash
# Remove old executable and recompile
rm -f search_engine
./compile.sh
```

## 📝 Usage Examples

### Example 1: Basic Search

```
> search computer science
Found 5 result(s):

[1] Document ID: 42 | Score: 0.8234
Preview: The field of computer science encompasses theoretical disciplines...

[2] Document ID: 128 | Score: 0.7891
Preview: Computer science is the study of algorithmic processes...
```

### Example 2: Autocomplete

```
> autocomplete alg
Autocomplete suggestions for 'alg':
  1. algorithm
  2. algorithmic
  3. algorithms
  4. algebra
```

### Example 3: Query Suggestions

```
> search machine learning
> search deep learning
> suggest learn
Query suggestions for 'learn':
  1. machine learning
  2. deep learning
```

### Example 4: Statistics

```
> stats
========== Index Statistics ==========
Total Documents: 18828
Total Terms: 125483
Total Postings: 2839472
======================================
```

## 📈 Performance Report

### Performance Metrics

The system tracks:

1. **Document Loading Time**
   - Time to read and parse all documents
   - Memory usage for document storage

2. **Index Building Time**
   - Time to construct inverted index
   - Memory usage of index structure

3. **Search Query Time**
   - Time per search query
   - Average search time over multiple queries

4. **Memory Usage**
   - Peak memory consumption
   - Memory per operation
   - Current memory footprint

### Sample Performance Output

```
========== Performance Report ==========

--- Timing Statistics ---
Operation: document_loading
  Total Time: 1234.567 ms
  Calls: 1
  Average Time: 1234.567 ms

Operation: index_building
  Total Time: 3456.789 ms
  Calls: 1
  Average Time: 3456.789 ms

Operation: search_query
  Total Time: 12.345 ms
  Calls: 10
  Average Time: 1.235 ms

--- Memory Statistics ---
Operation: document_loading
  Memory Usage: 45.67 MB

Operation: index_building
  Memory Usage: 78.90 MB

Peak Memory Usage: 125.43 MB
Current Memory Usage: 125.43 MB
========================================
```

### Performance Observations

1. **Index Construction**: One-time cost, scales linearly with dataset size
2. **Search Performance**: Sub-millisecond for typical queries on moderate datasets
3. **Memory Efficiency**: Index size typically 1.5-2x raw document size
4. **Scalability**: Handles datasets with 100K+ documents efficiently

## 🏛️ Architecture

### Class Diagram Overview

```
SearchEngine
    ├── Preprocessor
    │   └── stopwords: set<string>
    ├── InvertedIndex
    │   ├── index: unordered_map<string, PostingList>
    │   └── documentLengths: unordered_map<int, int>
    ├── TFIDFCalculator
    │   └── idfCache: map<string, double>
    └── PerformanceTracker
        ├── timers: map<string, TimerInfo>
        └── memoryRecords: map<string, size_t>
```

### Data Flow

1. **Indexing Phase**
   ```
   Documents → Preprocessor → Tokens → InvertedIndex → Index Built
   ```

2. **Search Phase**
   ```
   Query → Preprocessor → Query Terms → TFIDFCalculator → 
   Candidate Docs → Score Calculation → Ranked Results
   ```

### Key Algorithms

1. **Inverted Index Construction**
   - For each document:
     - Preprocess text → tokens
     - For each token:
       - Update posting list
       - Increment term frequency
       - Track document frequency

2. **TF-IDF Calculation**
   - For each query term:
     - Calculate TF = term_freq / doc_length
     - Calculate IDF = log(total_docs / doc_freq)
     - TF-IDF = TF × IDF
   - Sum TF-IDF scores for all query terms
   - Sort documents by score

3. **Autocomplete**
   - Filter terms by prefix match
   - Sort by document frequency (popularity)

## 📁 Project Structure

```
MiniSearchEngine/
│
├── Source Code/
│   ├── Preprocessor.h/cpp        # Text preprocessing
│   ├── InvertedIndex.h/cpp       # Inverted index data structure
│   ├── TFIDFCalculator.h/cpp     # TF-IDF scoring
│   ├── PerformanceTracker.h/cpp  # Performance monitoring
│   ├── SearchEngine.h/cpp        # Main search engine
│   └── main.cpp                  # Driver program
│
├── README.md                     # This file
├── compile.sh                    # Build script
└── download_dataset.sh           # Dataset downloader script
```

## 🔍 Implementation Details

### Stopword List

The preprocessor includes 60+ common English stopwords including:
- Articles: a, an, the
- Pronouns: he, she, it, they
- Prepositions: in, on, at, by, for
- Common verbs: is, are, was, were, have, has

### TF-IDF Formula

```
TF(t, d) = count(t, d) / |d|

IDF(t, D) = log(|D| / |{d ∈ D : t ∈ d}|)

TF-IDF(t, d, D) = TF(t, d) × IDF(t, D)

Query Score(q, d) = Σ TF-IDF(t, d, D) for all t in q
```

Where:
- `t` = term
- `d` = document
- `D` = document collection
- `|d|` = document length
- `|D|` = total documents

### Document Ranking

Documents are ranked by:
1. Sum of TF-IDF scores for all query terms
2. Descending order (highest score first)
3. Top-K results returned (default: 10)

## 🧪 Testing Recommendations

1. **Test with small dataset first** (10-20 documents)
2. **Verify preprocessing** (check tokenization, stopword removal)
3. **Validate TF-IDF scores** (manual calculation comparison)
4. **Performance benchmarking** (vary dataset sizes)
5. **Edge cases** (empty queries, no results, single term queries)

## 🎓 Learning Outcomes

This project demonstrates:

- **Algorithm Design**: Efficient search algorithms
- **Data Structure Selection**: Choosing appropriate structures for different operations
- **Space-Time Tradeoffs**: Balancing memory and speed
- **Real-world Problem Solving**: Building production-like systems
- **Code Organization**: Modular, maintainable design
- **Performance Analysis**: Understanding complexity and optimization

## 📚 References

- **Inverted Index**: Classic information retrieval data structure
- **TF-IDF**: Term Frequency-Inverse Document Frequency ranking
- **20 Newsgroups Dataset**: Standard IR benchmark dataset
- **C++ Standard Library**: std::unordered_map, std::vector, etc.

## 👨‍💻 Author

Developed by Aryan Manu as part of MACS Society DSA Project