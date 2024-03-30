//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include "search_server.h"

using namespace std;

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    
    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
        QueryResult query_result;
        query_result.matched_documents = server_.FindTopDocuments(raw_query, document_predicate);
        if (query_result.matched_documents.empty()) { 
            no_result_request_num_++;
        }
        AddToDeque();
        return query_result.matched_documents;
    }
    
    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);
    
    vector<Document> AddFindRequest(const string& raw_query);
    
    int GetNoResultRequests() const;
    
    private:
    
    struct QueryResult {
        // определите, что должно быть в структуре
        vector<Document> matched_documents;
    };
    
    deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int no_result_request_num_ = 0;
    const SearchServer& server_;
    
    void AddToDeque();
    
};