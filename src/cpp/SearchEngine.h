#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "Preprocessor.h"
#include "InvertedIndex.h"
#include "TFIDFCalculator.h"
#include "PerformanceTracker.h"
#include <string>
#include <vector>
#include <memory>

using namespace std;

class SearchEngine {
public:
    SearchEngine();
    
    bool loadDocuments(const string& directoryPath);
    
    void buildIndex();
    
    vector<SearchResult> search(const string& query, int topK = 10);
    
    string getDocument(int docId) const;
    
    PerformanceTracker& getPerformanceTracker();
    
    size_t getTotalDocuments() const;
    size_t getTotalTerms() const;
    size_t getTotalPostings() const;

    vector<string> getDictionaryTerms() const;
    
    vector<string> autocomplete(const string& prefix, int maxSuggestions = 10) const;
    
    vector<string> getQuerySuggestions(const string& query, 
                                      const vector<string>& searchHistory,
                                      int maxSuggestions = 5) const;
    
private:
    Preprocessor preprocessor;
    InvertedIndex index;
    unique_ptr<TFIDFCalculator> tfidfCalculator;
    PerformanceTracker performanceTracker;
    
    vector<string> documents; 
    vector<string> documentPaths;  
    
    vector<string> readDirectory(const string& directoryPath);
    
    void readDirectoryRecursive(const string& directoryPath, vector<string>& files);
    
    string readFile(const string& filePath);
};

#endif 
