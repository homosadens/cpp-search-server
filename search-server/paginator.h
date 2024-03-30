//Вставьте сюда своё решение из урока «‎Очередь запросов».‎
#pragma once
#include <iostream>
#include <vector>
#include "document.h"

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator range_begin, Iterator range_end)
        : begin_(range_begin), end_(range_end), size_(distance(begin_, end_))
    {
    }

    Iterator begin() const {
        return begin_;
    }

    Iterator end() const {
        return end_;
    }

    Iterator size() const {
        return size_;
    }

private:
    Iterator begin_, end_;
    size_t size_;
};

ostream& operator<< (ostream& out, const Document& document)
{
    out << "{ document_id = "s << document.id << ", relevance = "s << document.relevance << ", rating = "s << document.rating << " }"s;
    return out;
}

template <typename Iterator>
ostream& operator<<(ostream& out, const IteratorRange<Iterator>& range) {
    for (Iterator it = range.begin(); it != range.end(); ++it) {
        out << *it;
    }
    return out;
}

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t size) {
        Iterator begin_new = begin;
        while (begin_new != end)
        {
            if (distance(begin_new, end) < size)
            {
                pages_.push_back({ begin_new, end });
                break;
            }
            pages_.push_back({ begin_new, next(begin_new, size)});
            advance(begin_new, size);
        }
    }

    auto begin() const {
        return pages_.begin();
    }

    auto end() const {
        return pages_.end();
    }

    auto size() const {
        return pages_.size();
    }

private:
    vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}