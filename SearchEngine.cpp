#include "SearchEngine.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cstring>

using namespace std;

SearchEngine::SearchEngine() {
    tfidfCalculator = nullptr;
}

bool SearchEngine::loadDocuments(const string& directoryPath) {
    performanceTracker.startTimer("document_loading");
    
    documents.clear();
    documentPaths.clear();
    
    vector<string> filePaths = readDirectory(directoryPath);
    
    if (filePaths.empty()) {
        cerr << "Error: No documents found in directory: " << directoryPath << endl;
        performanceTracker.endTimer("document_loading");
        return false;
    }
    
    cout << "Found " << filePaths.size() << " files. Loading documents..." << endl;
    
    for (size_t i = 0; i < filePaths.size(); ++i) {
        string content = readFile(filePaths[i]);
        if (!content.empty()) {
            documents.push_back(content);
            documentPaths.push_back(filePaths[i]);
        }
        
        // Show progress for large datasets
        if ((i + 1) % 1000 == 0) {
            cout << "  Loaded " << (i + 1) << " files..." << endl;
        }
    }
    
    size_t memoryUsage = documents.size() * sizeof(string);
    for (const auto& doc : documents) {
        memoryUsage += doc.size();
    }
    performanceTracker.recordMemoryUsage("document_loading", memoryUsage);
    
    performanceTracker.endTimer("document_loading");
    
    cout << "Successfully loaded " << documents.size() << " documents." << endl;
    return !documents.empty();
}

vector<string> SearchEngine::readDirectory(const string& directoryPath) {
    vector<string> files;
    readDirectoryRecursive(directoryPath, files);
    return files;
}

void SearchEngine::readDirectoryRecursive(const string& directoryPath, vector<string>& files) {
    DIR* dir = opendir(directoryPath.c_str());
    if (dir == nullptr) {
        cerr << "Error: Cannot open directory: " << directoryPath << endl;
        return;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        string fullPath = directoryPath + "/" + string(entry->d_name);
        
        struct stat fileStat;
        if (stat(fullPath.c_str(), &fileStat) == 0) {
            if (S_ISREG(fileStat.st_mode)) {
                // It's a regular file, add it to the list
                files.push_back(fullPath);
            } else if (S_ISDIR(fileStat.st_mode)) {
                // It's a directory, recursively search it
                readDirectoryRecursive(fullPath, files);
            }
        }
    }
    
    closedir(dir);
}

string SearchEngine::readFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Warning: Cannot open file: " << filePath << endl;
        return "";
    }
    
    ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void SearchEngine::buildIndex() {
    performanceTracker.startTimer("index_building");
    
    index.clear();
    
    for (size_t i = 0; i < documents.size(); ++i) {
        vector<string> tokens = preprocessor.preprocess(documents[i]);
        index.addDocument(i, tokens);
    }
    
    // Initialize TF-IDF calculator after index is built
    tfidfCalculator = make_unique<TFIDFCalculator>(index);
    
    size_t memoryUsage = index.getTotalPostings() * sizeof(DocumentInfo) + 
                         index.getTotalTerms() * 50; // Approximate
    performanceTracker.recordMemoryUsage("index_building", memoryUsage);
    
    performanceTracker.endTimer("index_building");
    
    cout << "Index built successfully." << endl;
    cout << "  Total documents: " << index.getTotalDocuments() << endl;
    cout << "  Total terms: " << index.getTotalTerms() << endl;
    cout << "  Total postings: " << index.getTotalPostings() << endl;
}

vector<SearchResult> SearchEngine::search(const string& query, int topK) {
    if (!tfidfCalculator) {
        cerr << "Error: Index not built yet. Please call buildIndex() first." << endl;
        return vector<SearchResult>();
    }
    
    performanceTracker.startTimer("search_query");
    
    // Preprocess query
    vector<string> queryTerms = preprocessor.preprocess(query);
    
    if (queryTerms.empty()) {
        performanceTracker.endTimer("search_query");
        return vector<SearchResult>();
    }
    
    // Calculate TF-IDF scores
    vector<SearchResult> results = tfidfCalculator->calculateTFIDF(queryTerms);
    
    // Limit to top K results
    if (results.size() > static_cast<size_t>(topK)) {
        results.resize(topK);
    }
    
    performanceTracker.endTimer("search_query");
    
    return results;
}

string SearchEngine::getDocument(int docId) const {
    if (docId >= 0 && docId < static_cast<int>(documents.size())) {
        return documents[docId];
    }
    return "";
}

PerformanceTracker& SearchEngine::getPerformanceTracker() {
    return performanceTracker;
}

size_t SearchEngine::getTotalDocuments() const {
    return index.getTotalDocuments();
}

size_t SearchEngine::getTotalTerms() const {
    return index.getTotalTerms();
}

size_t SearchEngine::getTotalPostings() const {
    return index.getTotalPostings();
}

vector<string> SearchEngine::autocomplete(const string& prefix, int maxSuggestions) const {
    vector<string> suggestions;
    string lowerPrefix = preprocessor.toLowercase(prefix);
    
    vector<string> allTerms = index.getAllTerms();
    
    for (const auto& term : allTerms) {
        if (term.size() >= lowerPrefix.size() && 
            term.substr(0, lowerPrefix.size()) == lowerPrefix) {
            suggestions.push_back(term);
            
            if (suggestions.size() >= static_cast<size_t>(maxSuggestions)) {
                break;
            }
        }
    }
    
    // Sort by document frequency (more common terms first)
    sort(suggestions.begin(), suggestions.end(),
              [this](const string& a, const string& b) {
                  return index.getDocumentFrequency(a) > index.getDocumentFrequency(b);
              });
    
    return suggestions;
}

vector<string> SearchEngine::getQuerySuggestions(const string& query,
                                                 const vector<string>& searchHistory,
                                                 int maxSuggestions) const {
    vector<string> suggestions;
    
    // Preprocess query
    vector<string> queryTerms = preprocessor.preprocess(query);
    
    if (queryTerms.empty()) {
        return suggestions;
    }
    
    // Find similar queries from history (simple keyword matching)
    map<string, int> suggestionScores; // query -> score
    
    for (const auto& historyQuery : searchHistory) {
        vector<string> historyTerms = preprocessor.preprocess(historyQuery);
        
        // Calculate similarity (number of common terms)
        int commonTerms = 0;
        for (const auto& term : queryTerms) {
            if (find(historyTerms.begin(), historyTerms.end(), term) != historyTerms.end()) {
                commonTerms++;
            }
        }
        
        if (commonTerms > 0) {
            suggestionScores[historyQuery] = commonTerms;
        }
    }
    
    // Convert to vector and sort by score
    vector<pair<string, int>> sortedSuggestions(
        suggestionScores.begin(), suggestionScores.end());
    
    sort(sortedSuggestions.begin(), sortedSuggestions.end(),
              [](const pair<string, int>& a, const pair<string, int>& b) {
                  return a.second > b.second;
              });
    
    for (size_t i = 0; i < sortedSuggestions.size() && 
                       i < static_cast<size_t>(maxSuggestions); ++i) {
        suggestions.push_back(sortedSuggestions[i].first);
    }
    
    return suggestions;
}
