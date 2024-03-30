//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».‎
#include "request_queue.h"
#include "search_server.h"
#include "document.h"
    
// сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
RequestQueue::RequestQueue(const SearchServer& search_server)
        : server_(search_server)
        {
}
    
vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    QueryResult query_result;
    query_result.matched_documents = server_.FindTopDocuments(raw_query, status);
    if (query_result.matched_documents.empty()) { 
        no_result_request_num_++;
    }
     AddToDeque();
     return query_result.matched_documents;
}
    
vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    QueryResult query_result;
    query_result.matched_documents = server_.FindTopDocuments(raw_query);
    if (query_result.matched_documents.empty()) { 
        no_result_request_num_++;
    }
    AddToDeque();
    return query_result.matched_documents;
}
    
int RequestQueue::GetNoResultRequests() const {
    return no_result_request_num_;
}
    
void RequestQueue::AddToDeque() {
QueryResult query_result;
    if (requests_.size() < 1440) {
        requests_.push_back(query_result);
    }
    else {
        requests_.pop_front();
        no_result_request_num_--;
        requests_.push_back(query_result);
    }
}
