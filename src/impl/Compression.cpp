#include "impl/Compression.hpp"
#include "impl/ColumnStore.hpp"
#include "dbms/Store.hpp"


using namespace dbms;


ColumnStore * dbms::compress_columnstore_lineitem(const Relation &relation, const ColumnStore &store)
{
    Column<RLE<uint8_t>> * col0 = new Column<RLE<uint8_t>>();
    Column<RLE<uint64_t>> * col1 = new Column<RLE<uint64_t>>();
    Column<RLE<uint8_t>> * col2 = new Column<RLE<uint8_t>>();
    Column<RLE<uint64_t>> * col3 = new Column<RLE<uint64_t>>();
    Column<RLE<uint32_t>> * col4 = new Column<RLE<uint32_t>>();
    Column<RLE<uint64_t>> * col5 = new Column<RLE<uint64_t>>();
    Column<Dictionary<Char<26>>> * col6 = new Column<Dictionary<Char<26>>>();
    Column<RLE<uint32_t>> * col7 = new Column<RLE<uint32_t>>();
    Column<RLE<uint32_t>> * col8 = new Column<RLE<uint32_t>>();
    Column<Dictionary<uint32_t>> * col9 = new Column<Dictionary<uint32_t>>();
    Column<Dictionary<uint32_t>> * col10 = new Column<Dictionary<uint32_t>>();
    Column<RLE<uint32_t>> * col11 = new Column<RLE<uint32_t>>();
    Column<RLE<uint32_t>> * col12 = new Column<RLE<uint32_t>>();
    Column<RLE<Char<11>>> * col13 = new Column<RLE<Char<11>>>();
    Column<Char<45>> * col14 = new Column<Char<45>>();
    Column<RLE<uint64_t>> * col15 = new Column<RLE<uint64_t>>();

    auto orig_col0_it = store.get_column<uint8_t>(0).begin();
    auto orig_col1_it = store.get_column<uint64_t>(1).begin();
    auto orig_col2_it = store.get_column<uint8_t>(2).begin();
    auto orig_col3_it = store.get_column<uint64_t>(3).begin();
    auto orig_col4_it = store.get_column<uint32_t>(4).begin();
    auto orig_col5_it = store.get_column<uint64_t>(5).begin();
    auto orig_col6_it = store.get_column<Char<26>>(6).begin();
    auto orig_col7_it = store.get_column<uint32_t>(7).begin();
    auto orig_col8_it = store.get_column<uint32_t>(8).begin();
    auto orig_col9_it = store.get_column<uint32_t>(9).begin();
    auto orig_col10_it = store.get_column<uint32_t>(10).begin();
    auto orig_col11_it = store.get_column<uint32_t>(11).begin();
    auto orig_col12_it = store.get_column<uint32_t>(12).begin();
    auto orig_col13_it = store.get_column<Char<11>>(13).begin();
    auto orig_col14_it = store.get_column<Char<45>>(14).begin();
    auto orig_col15_it = store.get_column<uint64_t>(15).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col0->push_back(*orig_col0_it++);
        col1->push_back(*orig_col1_it++);
        col2->push_back(*orig_col2_it++);
        col3->push_back(*orig_col3_it++);
        col4->push_back(*orig_col4_it++);
        col5->push_back(*orig_col5_it++);
        col6->push_back(*orig_col6_it++);
        col7->push_back(*orig_col7_it++);
        col8->push_back(*orig_col8_it++);
        col9->push_back(*orig_col9_it++);
        col10->push_back(*orig_col10_it++);
        col11->push_back(*orig_col11_it++);
        col12->push_back(*orig_col12_it++);
        col13->push_back(*orig_col13_it++);
        col14->push_back(*orig_col14_it++);
        col15->push_back(*orig_col15_it++);
    }

    ColumnStore* compress_colstore = new ColumnStore(ColumnStore::Create_Explicit({col0, col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11, col12, col13, col14, col15}));

    return compress_colstore;
}
