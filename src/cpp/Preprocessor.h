#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include <vector>
#include <set>

using namespace std;

class Preprocessor {
public:
    Preprocessor();
    
    vector<string> preprocess(const string& text) const;
    
    string toLowercase(const string& text) const;
    string removePunctuation(const string& text) const;
    vector<string> tokenize(const string& text) const;
    vector<string> removeStopwords(const vector<string>& tokens) const;
    
    bool isStopword(const string& word) const;
    
private:
    set<string> stopwords;
    void initializeStopwords();
};

#endif 
