#include "TFIDFCalculator.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <set>

using namespace std;

TFIDFCalculator::TFIDFCalculator(const InvertedIndex& idx) : index(idx) {}

double TFIDFCalculator::calculateTF(const string& term, int docId, int docLength) const {
    if (docLength == 0) return 0.0;
    
    int termFreq = index.getTermFrequency(term, docId);
    
    return static_cast<double>(termFreq) / static_cast<double>(docLength);
}

double TFIDFCalculator::calculateIDF(const string& term) const {
    return getIDF(term);
}

double TFIDFCalculator::getIDF(const string& term) const {
    auto cacheIt = idfCache.find(term);
    if (cacheIt != idfCache.end()) {
        return cacheIt->second;
    }
    
    int totalDocs = index.getTotalDocuments();
    int docFreq = index.getDocumentFrequency(term);
    
    if (docFreq == 0 || totalDocs == 0) {
        idfCache[term] = 0.0;
        return 0.0;
    }
    
    double idf = log(static_cast<double>(totalDocs) / static_cast<double>(docFreq));
    idfCache[term] = idf;
    
    return idf;
}

double TFIDFCalculator::calculateTermTFIDF(const string& term, int docId, int docLength) const {
    double tf = calculateTF(term, docId, docLength);
    double idf = getIDF(term);
    return tf * idf;
}

vector<SearchResult> TFIDFCalculator::calculateTFIDF(const vector<string>& queryTerms) {
    set<int> candidateDocs;
    
    for (const auto& term : queryTerms) {
        const PostingList* postingList = index.getPostingList(term);
        if (postingList) {
            for (const auto& docInfo : postingList->documents) {
                candidateDocs.insert(docInfo.docId);
            }
        }
    }
    
    unordered_map<int, double> docScores;
    
    for (int docId : candidateDocs) {
        double score = 0.0;
        int docLength = index.getDocumentLength(docId);
        
        for (const auto& term : queryTerms) {
            score += calculateTermTFIDF(term, docId, docLength);
        }
        
        if (score > 0.0) {
            docScores[docId] = score;
        }
    }
    
    vector<SearchResult> results;
    for (const auto& pair : docScores) {
        results.push_back(SearchResult(pair.first, pair.second));
    }
    
    sort(results.begin(), results.end());
    
    return results;
}
