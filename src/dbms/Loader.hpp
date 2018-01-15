#pragma once

#include "dbms/assert.hpp"
#include "dbms/Store.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>


namespace dbms {

struct Loader
{
    static std::size_t load_LineItem(const char *filename, const Relation &relation, RowStore &store,
                                     const std::size_t max_rows = std::numeric_limits<std::size_t>::max());
    static std::size_t load_LineItem(const char *filename, const Relation &relation, ColumnStore &store,
                                     const std::size_t max_rows = std::numeric_limits<std::size_t>::max());
    static std::size_t load_Orders(const char *filename, const Relation &relation, ColumnStore &store,
                                   const std::size_t max_rows = std::numeric_limits<std::size_t>::max());

    Loader(const char *filename, const int delimiter);
    ~Loader();

    void parse_header(const Relation &relation);
    const char * read();
    template<std::size_t N> void read(Char<N> &chr);
    int64_t read_int();
    int64_t read_fixedpoint();
    int read_char();
    uint32_t read_date();

    const int delimiter;
    private:
    const char *filename_;
    FILE *file_;
    std::vector<std::size_t> offsets_;
    std::vector<char> strbuf_;
};

template<std::size_t N>
void Loader::read(Char<N> &chr)
{
    std::size_t i = 0;
    int c = getc_unlocked(file_);
    while (i != N - 1 and c != delimiter and c != '\n') {
        chr.data[i++] = c;
        c = getc_unlocked(file_);
    }
    assert(i < N);
    chr.data[i] = 0;
}

}
