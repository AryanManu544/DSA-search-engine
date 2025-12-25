#ifndef TFIDF_CALCULATOR_H
#define TFIDF_CALCULATOR_H

#include "InvertedIndex.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

struct SearchResult {
    int docId;
    double score;
    
    SearchResult() : docId(-1), score(0.0) {}
    SearchResult(int id, double s) : docId(id), score(s) {}
    
    bool operator<(const SearchResult& other) const {
        return score > other.score; 
    }
};

class TFIDFCalculator {
public:
    TFIDFCalculator(const InvertedIndex& index);
    
    vector<SearchResult> calculateTFIDF(const vector<string>& queryTerms);
    
    double calculateTF(const string& term, int docId, int docLength) const;
    
    double calculateIDF(const string& term) const;
    
    double calculateTermTFIDF(const string& term, int docId, int docLength) const;
    
private:
    const InvertedIndex& index;
    
    mutable map<string, double> idfCache;
    
    double getIDF(const string& term) const;
};

#endif 