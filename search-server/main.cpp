#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

struct Document {
    int id;
    double relevance;
};

struct Query {
    set<string> plus_words;
    set<string> minus_words;
}; 

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }
    
    //добавление данных нового документа в словарь для поиска слов в документах сервера, 
    //хранящий вместе с id документа его релевантность, вычисленную по формуле TF  
    void AddDocument(int document_id, const string& document) {
        document_count_++; 
        const vector<string> words = SplitIntoWordsNoStop(document);
        int doc_words_num = words.size();
        
        for (auto word : words) {
            word_to_document_freqs_[word][document_id] += 1. / doc_words_num;
        } 
    }
 
    //сортировка документов по релевантности, сокращение их количества до MAX_RESULT_DOCUMENT_COUNT самых релевантных
    vector<Document> FindTopDocuments(const string& raw_query) const {
        Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);
        
        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    
    //словарь, который хранит слова документов, их id и релевантность
    map<string, map<int, double>> word_to_document_freqs_;
    
    int document_count_ = 0;

    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    // разбиение строки text на слова, исключая стоп-слова 
    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }
 
    //разбиение строки запроса на слова (исключая стоп-слова), добавление их в структуру Query
    Query ParseQuery(const string& text) const {
        Query query_words; 
        
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-') {
                query_words.minus_words.insert(word.substr(1));
            }
            else {
                query_words.plus_words.insert(word);
            }
        }
        return query_words;
    }

    double IdfFormula(int all_docs, int doc_words) {
        return log (static_cast<double>(all_docs) / doc_words); 
    }

    //Возвращает вектор документов, содержащих плюс-слова и не содержащих минус-слова.
    //Для каждого документа из возвращаемого вектора вычисляется релевантность по формуле TF-IDF
    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map<int, double> document_to_relevance;
        
        for (auto plus_word : query_words.plus_words) {
            double idf;
            if (word_to_document_freqs_.count(plus_word) > 0) {
                map<int, double> doc_to_tf = word_to_document_freqs_.at(plus_word);
                if (doc_to_tf.size() == document_count_) {
                    idf = 0;
                } else {
                    idf = IdfFormula(document_count_, doc_to_tf.size());
                }
                for (const auto& [id, tf] : doc_to_tf) { 
                    if (doc_to_tf.count(id) > 0) {
                        document_to_relevance[id] += idf * tf;
                    }
                }
            }
        }
          
        for (auto minus_word : query_words.minus_words) {
            if (word_to_document_freqs_.count(minus_word) > 0) {
                map <int, double> doc_to_tf = word_to_document_freqs_.at(minus_word);
                for (const auto& [id, tf] : doc_to_tf) {
                    document_to_relevance.erase(id);
                }
            }
        }       
        
        for (const auto& [id, rel]: document_to_relevance) {
            Document doc;
            doc.id = id;
            doc.relevance = rel;
            matched_documents.push_back(doc);
        } 
        return matched_documents; 
    } 
};


//Создание поискового сервера: чтение стоп-слов, количества документов и самих документов
SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }
    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] :  search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }  
}
