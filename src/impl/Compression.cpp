#include "impl/Compression.hpp"
#include "impl/ColumnStore.hpp"


using namespace dbms;


ColumnStore * dbms::compress_columnstore_lineitem(const Relation &relation, const ColumnStore &store)
{
    std::vector<ColumnBase*> compressed_columns;


    //access uncompressed store column 1
    Column<RLE<char>> * col1 = new Column<RLE<char>>();
    auto orig_col1_it = store.get_column<char>(0).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col1->push_back(*orig_col1_it);
        ++orig_col1_it;
    }

    //access uncompressed store column 2
    Column<RLE<int64_t>> * col2 = new Column<RLE<int64_t>>();
    auto orig_col2_it = store.get_column<int64_t>(1).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col2->push_back(*orig_col2_it);
        ++orig_col2_it;
    }
    //access uncompressed store column 3
    Column<RLE<char>> * col3 = new Column<RLE<char>>();
    auto orig_col3_it = store.get_column<char>(2).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col3->push_back(*orig_col3_it);
        ++orig_col3_it;
    }
    //access uncompressed store column 4
    Column<RLE<int64_t>> * col4 = new Column<RLE<int64_t>>();
    auto orig_col4_it = store.get_column<int64_t>(3).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col4->push_back(*orig_col4_it);
        ++orig_col4_it;
    }
    //access uncompressed store column 5
    Column<RLE<int32_t>> * col5 = new Column<RLE<int32_t>>();
    auto orig_col5_it = store.get_column<int32_t>(4).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col5->push_back(*orig_col5_it);
        ++orig_col5_it;
    }
    //access uncompressed store column 6
    Column<RLE<char>> * col6 = new Column<RLE<char>>();
    auto orig_col6_it = store.get_column<char>(5).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col6->push_back(*orig_col6_it);
        ++orig_col6_it;
    }
    //access uncompressed store column 5
    Column<RLE<int32_t>> * col7 = new Column<RLE<int32_t>>();
    auto orig_col7_it = store.get_column<int32_t>(6).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col7->push_back(*orig_col7_it);
        ++orig_col7_it;
    }
    //access uncompressed store column 5
    Column<RLE<int32_t>> * col8 = new Column<RLE<int32_t>>();
    auto orig_col8_it = store.get_column<int32_t>(7).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col8->push_back(*orig_col8_it);
        ++orig_col8_it;
    }
    //access uncompressed store column 5
    Column<RLE<int32_t>> * col9 = new Column<RLE<int32_t>>();
    auto orig_col9_it = store.get_column<int32_t>(8).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col9->push_back(*orig_col9_it);
        ++orig_col9_it;
    }
    //access uncompressed store column 5
    Column<RLE<int32_t>> * col10 = new Column<RLE<int32_t>>();
    auto orig_col10_it = store.get_column<int32_t>(9).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col10->push_back(*orig_col10_it);
        ++orig_col10_it;
    }
    //access uncompressed store column 5
    Column<RLE<int32_t>> * col11 = new Column<RLE<int32_t>>();
    auto orig_col11_it = store.get_column<int32_t>(10).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col11->push_back(*orig_col11_it);
        ++orig_col11_it;
    }
    //access uncompressed store column 5
    Column<RLE<int32_t>> * col12 = new Column<RLE<int32_t>>();
    auto orig_col12_it = store.get_column<int32_t>(11).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col12->push_back(*orig_col12_it);
        ++orig_col12_it;
    }
    //access uncompressed store column 5
    Column<RLE<int32_t>> * col13 = new Column<RLE<int32_t>>();
    auto orig_col13_it = store.get_column<int32_t>(12).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col13->push_back(*orig_col13_it);
        ++orig_col13_it;
    }
    //access uncompressed store column 5
    Column<RLE<char>> * col14 = new Column<RLE<char>>();
    auto orig_col14_it = store.get_column<char>(13).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col14->push_back(*orig_col14_it);
        ++orig_col14_it;
    }
    //access uncompressed store column 5
    Column<RLE<char>> * col15 = new Column<RLE<char>>();
    auto orig_col15_it = store.get_column<char>(14).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col15->push_back(*orig_col15_it);
        ++orig_col15_it;
    }
    //access uncompressed store column 5
    Column<RLE<int64_t>> * col16 = new Column<RLE<int64_t>>();
    auto orig_col16_it = store.get_column<int64_t>(15).begin();

    for (unsigned i = 0; i < store.size(); ++i) {
        col16->push_back(*orig_col16_it);
        ++orig_col16_it;
    }
    ColumnStore* compress_colstore = new ColumnStore(ColumnStore::Create_Explicit({col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11, col12, col13, col14, col15, col16}));

    return compress_colstore;
}
