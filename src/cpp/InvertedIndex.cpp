#include "InvertedIndex.h"
#include <algorithm>

using namespace std;

InvertedIndex::InvertedIndex() : totalDocuments(0) {}

void InvertedIndex::addTerm(const string& term, int docId) {
    if (index.find(term) == index.end()) {
        index[term] = PostingList();
    }
    
    PostingList& postingList = index[term];
    
    bool found = false;
    for (auto& docInfo : postingList.documents) {
        if (docInfo.docId == docId) {
            docInfo.frequency++;
            found = true;
            break;
        }
    }
    
    if (!found) {
        postingList.documents.push_back(DocumentInfo(docId, 1));
        postingList.documentFrequency++;
    }
}

void InvertedIndex::addDocument(int docId, const vector<string>& tokens) {
    documentLengths[docId] = tokens.size();
    
    for (const auto& term : tokens) {
        if (!term.empty()) {
            addTerm(term, docId);
        }
    }
    
    totalDocuments = max(totalDocuments, docId + 1);
}

const PostingList* InvertedIndex::getPostingList(const string& term) const {
    auto it = index.find(term);
    if (it != index.end()) {
        return &(it->second);
    }
    return nullptr;
}

int InvertedIndex::getTotalDocuments() const {
    return totalDocuments;
}

int InvertedIndex::getDocumentFrequency(const string& term) const {
    auto it = index.find(term);
    if (it != index.end()) {
        return it->second.documentFrequency;
    }
    return 0;
}

int InvertedIndex::getTermFrequency(const string& term, int docId) const {
    auto it = index.find(term);
    if (it != index.end()) {
        const PostingList& postingList = it->second;
        for (const auto& docInfo : postingList.documents) {
            if (docInfo.docId == docId) {
                return docInfo.frequency;
            }
        }
    }
    return 0;
}

vector<string> InvertedIndex::getAllTerms() const {
    vector<string> terms;
    for (const auto& pair : index) {
        terms.push_back(pair.first);
    }
    return terms;
}

int InvertedIndex::getDocumentLength(int docId) const {
    auto it = documentLengths.find(docId);
    if (it != documentLengths.end()) {
        return it->second;
    }
    return 0;
}

void InvertedIndex::clear() {
    index.clear();
    documentLengths.clear();
    totalDocuments = 0;
}

size_t InvertedIndex::getTotalTerms() const {
    return index.size();
}

size_t InvertedIndex::getTotalPostings() const {
    size_t total = 0;
    for (const auto& pair : index) {
        total += pair.second.documents.size();
    }
    return total;
}
