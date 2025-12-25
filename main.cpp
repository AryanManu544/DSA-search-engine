#include "SearchEngine.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

void printHelp() {
    cout << "\n========== Mini Search Engine Commands ==========\n";
    cout << "Commands:\n";
    cout << "  search <query>          - Search for documents\n";
    cout << "  autocomplete <prefix>   - Get autocomplete suggestions\n";
    cout << "  suggest <query>         - Get query suggestions\n";
    cout << "  stats                   - Show index statistics\n";
    cout << "  performance             - Show performance report\n";
    cout << "  help                    - Show this help message\n";
    cout << "  quit / exit             - Exit the program\n";
    cout << "================================================\n\n";
}

void printSearchResults(const vector<SearchResult>& results, 
                       SearchEngine& engine, int maxPreview = 200) {
    if (results.empty()) {
        cout << "No results found.\n" << endl;
        return;
    }
    
    cout << "\nFound " << results.size() << " result(s):\n";
    cout << fixed << setprecision(4);
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        string docContent = engine.getDocument(result.docId);
        
        cout << "\n[" << (i + 1) << "] Document ID: " << result.docId 
             << " | Score: " << result.score << "\n";
        
        // Preview document content
        size_t previewLen = static_cast<size_t>(maxPreview);
        string preview = docContent.substr(0, previewLen);
        if (docContent.size() > previewLen) {
            preview += "...";
        }
        
        // Replace newlines with spaces for cleaner output
        replace(preview.begin(), preview.end(), '\n', ' ');
        replace(preview.begin(), preview.end(), '\r', ' ');
        
        cout << "Preview: " << preview << "\n";
    }
    cout << "\n";
}

int main(int argc, char* argv[]) {
    cout << "========================================\n";
    cout << "   Mini Search Engine - DSA Project\n";
    cout << "========================================\n\n";
    
    string datasetPath;
    
    if (argc < 2) {
        // Default to 20news-18828 if it exists
        if (system("test -d 20news-18828") == 0) {
            datasetPath = "20news-18828";
            cout << "No directory specified. Using default: " << datasetPath << endl;
        } else {
            cerr << "Usage: " << argv[0] << " <dataset_directory>" << endl;
            cerr << "Example: " << argv[0] << " ./20news-18828" << endl;
            cerr << "\nTo download the 20 Newsgroups dataset, run:" << endl;
            cerr << "  ./download_dataset.sh" << endl;
            return 1;
        }
    } else {
        datasetPath = argv[1];
    }
    
    SearchEngine engine;
    
    cout << "Loading documents from: " << datasetPath << endl;
    cout << "Reading files recursively from all subdirectories..." << endl;
    if (!engine.loadDocuments(datasetPath)) {
        cerr << "Failed to load documents. Exiting." << endl;
        cerr << "\nMake sure the dataset directory exists and contains text files." << endl;
        cerr << "To download the 20 Newsgroups dataset, run:" << endl;
        cerr << "  ./download_dataset.sh" << endl;
        return 1;
    }
    
    cout << "\nBuilding inverted index..." << endl;
    engine.buildIndex();
    
    cout << "\nIndex built successfully! Ready to search.\n" << endl;
    
    // Store search history for suggestions
    vector<string> searchHistory;
    
    printHelp();
    
    string command;
    while (true) {
        cout << "> ";
        getline(cin, command);
        
        if (command.empty()) {
            continue;
        }
        
        istringstream iss(command);
        string cmd;
        iss >> cmd;
        
        if (cmd == "quit" || cmd == "exit") {
            cout << "Goodbye!" << endl;
            break;
        }
        else if (cmd == "help") {
            printHelp();
        }
        else if (cmd == "search") {
            string query;
            getline(iss, query);
            
            // Remove leading space if any
            if (!query.empty() && query[0] == ' ') {
                query = query.substr(1);
            }
            
            if (query.empty()) {
                cout << "Error: Please provide a search query." << endl;
                continue;
            }
            
            // Add to search history
            searchHistory.push_back(query);
            
            vector<SearchResult> results = engine.search(query, 10);
            printSearchResults(results, engine);
        }
        else if (cmd == "autocomplete") {
            string prefix;
            iss >> prefix;
            
            if (prefix.empty()) {
                cout << "Error: Please provide a prefix." << endl;
                continue;
            }
            
            vector<string> suggestions = engine.autocomplete(prefix, 10);
            
            if (suggestions.empty()) {
                cout << "No suggestions found for: " << prefix << endl;
            } else {
                cout << "\nAutocomplete suggestions for '" << prefix << "':\n";
                for (size_t i = 0; i < suggestions.size(); ++i) {
                    cout << "  " << (i + 1) << ". " << suggestions[i] << endl;
                }
                cout << endl;
            }
        }
        else if (cmd == "suggest") {
            string query;
            getline(iss, query);
            
            if (!query.empty() && query[0] == ' ') {
                query = query.substr(1);
            }
            
            if (query.empty() || searchHistory.empty()) {
                cout << "No suggestions available (need search history)." << endl;
                continue;
            }
            
            vector<string> suggestions = engine.getQuerySuggestions(query, searchHistory, 5);
            
            if (suggestions.empty()) {
                cout << "No similar queries found." << endl;
            } else {
                cout << "\nQuery suggestions for '" << query << "':\n";
                for (size_t i = 0; i < suggestions.size(); ++i) {
                    cout << "  " << (i + 1) << ". " << suggestions[i] << endl;
                }
                cout << endl;
            }
        }
        else if (cmd == "stats") {
            cout << "\n========== Index Statistics ==========\n";
            cout << "Total Documents: " << engine.getTotalDocuments() << endl;
            cout << "Total Terms: " << engine.getTotalTerms() << endl;
            cout << "Total Postings: " << engine.getTotalPostings() << endl;
            cout << "======================================\n\n";
        }
        else if (cmd == "performance") {
            engine.getPerformanceTracker().printReport();
        }
        else {
            cout << "Unknown command: " << cmd << endl;
            cout << "Type 'help' for available commands." << endl;
        }
    }
    
    // Print final performance report
    cout << "\n";
    engine.getPerformanceTracker().printReport();
    
    return 0;
}
