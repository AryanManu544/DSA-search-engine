#include "Preprocessor.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

using namespace std;

Preprocessor::Preprocessor() {
    initializeStopwords();
}

void Preprocessor::initializeStopwords() {
    // Common English stopwords
    string stopwordsList[] = {
        "a", "an", "and", "are", "as", "at", "be", "by", "for", "from",
        "has", "he", "in", "is", "it", "its", "of", "on", "that", "the",
        "to", "was", "were", "will", "with", "the", "this", "but", "they",
        "have", "had", "what", "said", "each", "which", "their", "time",
        "if", "up", "out", "many", "then", "them", "these", "so", "some",
        "her", "would", "make", "like", "into", "him", "has", "two", "more",
        "very", "after", "words", "long", "than", "first", "been", "call",
        "who", "oil", "its", "now", "find", "down", "day", "did", "get",
        "come", "made", "may", "part"
    };
    
    for (const auto& word : stopwordsList) {
        stopwords.insert(word);
    }
}

string Preprocessor::toLowercase(const string& text) const {
    string result = text;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

string Preprocessor::removePunctuation(const string& text) const {
    string result;
    for (char c : text) {
        if (isalnum(c) || c == ' ' || c == '\n' || c == '\t') {
            result += c;
        } else {
            result += ' '; 
        }
    }
    return result;
}

vector<string> Preprocessor::tokenize(const string& text) const {
    vector<string> tokens;
    istringstream iss(text);
    string token;
    
    while (iss >> token) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

bool Preprocessor::isStopword(const string& word) const {
    return stopwords.find(word) != stopwords.end();
}

vector<string> Preprocessor::removeStopwords(const vector<string>& tokens) const {
    vector<string> filtered;
    for (const auto& token : tokens) {
        if (!isStopword(token)) {
            filtered.push_back(token);
        }
    }
    return filtered;
}

vector<string> Preprocessor::preprocess(const string& text) const {
    string processed = toLowercase(text);
    processed = removePunctuation(processed);
    vector<string> tokens = tokenize(processed);
    tokens = removeStopwords(tokens);
    return tokens;
}
