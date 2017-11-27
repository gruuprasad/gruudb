#include "dbms/Loader.hpp"

#include "dbms/util.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/RowStore.hpp"
#include <cstring>
#include <err.h>
#include <limits>


using namespace dbms;

bool isnum(int c) { return '0' <= c and c <= '9'; }

Loader::Loader(const char *filename, const int delimiter)
    : delimiter(delimiter)
    , filename_(nonnull(filename))
    , file_(fopen(filename, "r"))
{
    if (not file_)
        err(EXIT_FAILURE, "Failed to open file '%s'", filename_);
}

Loader::~Loader()
{
    if (fclose(file_))
        warn("Failed to close file '%s'", filename_);
}

void Loader::parse_header(const Relation &relation)
{
    offsets_.reserve(relation.size());

    for (std::size_t i = 0; i != relation.size(); ++i) {
        std::string name(read());
        const Attribute &attr = relation[name];
        offsets_.push_back(attr.offset());
    }
}

const char * Loader::read()
{
    strbuf_.clear();
    int c = getc_unlocked(file_);
    while (not feof(file_) and c != delimiter and c != '\n') {
        strbuf_.push_back(c);
        c = getc_unlocked(file_);
    }
    strbuf_.push_back(0);

    return &strbuf_[0];
}

int64_t Loader::read_int()
{
    int64_t res = 0;
    int c = '0';
    while (isnum(c)) {
        res = res * 10 + (c - '0');
        c = getc_unlocked(file_);
    }
    return res;
}

int64_t Loader::read_fixedpoint()
{
    int64_t pre = read_int();
    int64_t post = read_int();
    return pre * 100 + post;
}

int Loader::read_char() {
    int c = getc_unlocked(file_);
    getc_unlocked(file_);
    return c;
}

uint32_t Loader::read_date()
{
    uint32_t year, month, day;
    fscanf(file_, "%u-%u-%u", &year, &month, &day);
    getc_unlocked(file_);
    return date_to_int(year, month, day);
}

std::size_t Loader::load_LineItem(const char *filename, const Relation &relation, RowStore &store,
                                  const std::size_t max_rows)
{
    Loader loader(filename, '|');
    loader.parse_header(relation);

    std::size_t num_rows = 0;
    while (num_rows != max_rows) {
        uint32_t orderkey        = loader.read_int();
        uint32_t partkey         = loader.read_int();
        uint32_t suppkey         = loader.read_int();
        int32_t linenumber       = loader.read_int();
        int64_t quantity         = loader.read_int();
        int64_t extendedprice    = loader.read_fixedpoint();
        int64_t discount         = loader.read_fixedpoint();
        int64_t tax              = loader.read_fixedpoint();
        char returnflag          = loader.read_char();
        char linestatus          = loader.read_char();
        uint32_t shipdate        = loader.read_date();
        uint32_t commitdate      = loader.read_date();
        uint32_t receiptdate     = loader.read_date();
        Char<26> shipinstruct;     loader.read(shipinstruct);
        Char<11> shipmode;         loader.read(shipmode);
        Char<45> comment;          loader.read(comment);

        if (feof(loader.file_)) break;

        auto it = store.append(1);
        ++num_rows;

        it.get<uint32_t>(loader.offsets_[0]) = orderkey;
        it.get<uint32_t>(loader.offsets_[1]) = partkey;
        it.get<uint32_t>(loader.offsets_[2]) = suppkey;
        it.get< int32_t>(loader.offsets_[3]) = linenumber;
        it.get< int64_t>(loader.offsets_[4]) = quantity;
        it.get< int64_t>(loader.offsets_[5]) = extendedprice;
        it.get< int64_t>(loader.offsets_[6]) = discount;
        it.get< int64_t>(loader.offsets_[7]) = tax;
        it.get<unsigned char>(loader.offsets_[8]) = returnflag;
        it.get<unsigned char>(loader.offsets_[9]) = linestatus;
        it.get<uint32_t>(loader.offsets_[10]) = shipdate;
        it.get<uint32_t>(loader.offsets_[11]) = commitdate;
        it.get<uint32_t>(loader.offsets_[12]) = receiptdate;
        it.get<Char<26>>(loader.offsets_[13]) = shipinstruct;
        it.get<Char<11>>(loader.offsets_[14]) = shipmode;
        it.get<Char<45>>(loader.offsets_[15]) = comment;
    }

    return num_rows;
}

std::size_t Loader::load_LineItem(const char *filename, const Relation &relation, ColumnStore &store,
                                  const std::size_t max_rows)
{
    Loader loader(filename, '|');
    loader.parse_header(relation);

    std::size_t num_rows = 0;
    while (num_rows != max_rows) {
        uint32_t orderkey        = loader.read_int();
        uint32_t partkey         = loader.read_int();
        uint32_t suppkey         = loader.read_int();
        int32_t linenumber       = loader.read_int();
        int64_t quantity         = loader.read_int();
        int64_t extendedprice    = loader.read_fixedpoint();
        int64_t discount         = loader.read_fixedpoint();
        int64_t tax              = loader.read_fixedpoint();
        char returnflag          = loader.read_char();
        char linestatus          = loader.read_char();
        uint32_t shipdate        = loader.read_date();
        uint32_t commitdate      = loader.read_date();
        uint32_t receiptdate     = loader.read_date();
        Char<26> shipinstruct;     loader.read(shipinstruct);
        Char<11> shipmode;         loader.read(shipmode);
        Char<45> comment;          loader.read(comment);

        if (feof(loader.file_)) break;

        ++num_rows;

        store.get_column<uint32_t>(loader.offsets_[0]).push_back(orderkey);
        store.get_column<uint32_t>(loader.offsets_[1]).push_back(partkey);
        store.get_column<uint32_t>(loader.offsets_[2]).push_back(suppkey);
        store.get_column< int32_t>(loader.offsets_[3]).push_back(linenumber);
        store.get_column< int64_t>(loader.offsets_[4]).push_back(quantity);
        store.get_column< int64_t>(loader.offsets_[5]).push_back(extendedprice);
        store.get_column< int64_t>(loader.offsets_[6]).push_back(discount);
        store.get_column< int64_t>(loader.offsets_[7]).push_back(tax);
        store.get_column<char>(loader.offsets_[8]).push_back(returnflag);
        store.get_column<char>(loader.offsets_[9]).push_back(linestatus);
        store.get_column<uint32_t>(loader.offsets_[10]).push_back(shipdate);
        store.get_column<uint32_t>(loader.offsets_[11]).push_back(commitdate);
        store.get_column<uint32_t>(loader.offsets_[12]).push_back(receiptdate);
        store.get_column<Char<26>>(loader.offsets_[13]).push_back(shipinstruct);
        store.get_column<Char<11>>(loader.offsets_[14]).push_back(shipmode);
        store.get_column<Char<45>>(loader.offsets_[15]).push_back(comment);
    }

    return num_rows;
}

std::size_t Loader::load_Orders(const char *filename, const Relation &relation, ColumnStore &store,
                                const std::size_t max_rows)
{
    Loader loader(filename, '|');
    loader.parse_header(relation);

    std::size_t num_rows = 0;
    while (num_rows != max_rows) {
        uint32_t orderkey = loader.read_int();
        uint32_t custkey = loader.read_int();
        char orderstatus = loader.read_char();
        uint64_t totalprice = loader.read_fixedpoint();
        uint32_t orderdate = loader.read_date();
        Char<16> orderpriority; loader.read(orderpriority);
        Char<16> clerk; loader.read(clerk);
        int32_t shippriority = loader.read_int();
        Char<80> comment; loader.read(comment);

        if (feof(loader.file_)) break;

        ++num_rows;

        store.get_column<uint32_t>(loader.offsets_[0]).push_back(orderkey);
        store.get_column<uint32_t>(loader.offsets_[1]).push_back(custkey);
        store.get_column<char>(loader.offsets_[2]).push_back(orderstatus);
        store.get_column<uint64_t>(loader.offsets_[3]).push_back(totalprice);
        store.get_column<uint32_t>(loader.offsets_[4]).push_back(orderdate);
        store.get_column<Char<16>>(loader.offsets_[5]).push_back(orderpriority);
        store.get_column<Char<16>>(loader.offsets_[6]).push_back(clerk);
        store.get_column<int32_t>(loader.offsets_[7]).push_back(shippriority);
        store.get_column<Char<80>>(loader.offsets_[8]).push_back(comment);
    }

    return num_rows;
}
