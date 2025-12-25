#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

using namespace std;

struct DocumentInfo {
    int docId;
    int frequency;  
    
    DocumentInfo(int id, int freq) : docId(id), frequency(freq) {}
};

struct PostingList {
    vector<DocumentInfo> documents;
    int documentFrequency;  
    
    PostingList() : documentFrequency(0) {}
};

class InvertedIndex {
public:
    InvertedIndex();
    
    void addDocument(int docId, const vector<string>& tokens);
    
    const PostingList* getPostingList(const string& term) const;
    
    int getTotalDocuments() const;
    
    int getDocumentFrequency(const string& term) const;
    
    int getTermFrequency(const string& term, int docId) const;
    
    vector<string> getAllTerms() const;
    
    int getDocumentLength(int docId) const;
    
    void clear();
    
    size_t getTotalTerms() const;
    size_t getTotalPostings() const;
    
private:
    unordered_map<string, PostingList> index;
    
    unordered_map<int, int> documentLengths;
    
    int totalDocuments;
    
    void addTerm(const string& term, int docId);
};

#endif 
