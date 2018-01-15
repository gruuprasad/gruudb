#pragma once

#include "dbms/Store.hpp"
#include <typeinfo>


namespace dbms {

/**
 * This class implements a run for run length encoding.
 */
template<typename T>
struct RLE
{
    RLE(T value) : value(value), count(1) { }
    RLE(T value, uint32_t count) : value(value), count(count) { }

    friend std::ostream & operator<<(std::ostream &out, const RLE &rle) {
        return out << "RLE(" << rle.value << ", " << rle.count << ')';
    }
    DECLARE_DUMP

    T value;
    uint32_t count;
};

template<typename T, typename S = uint32_t>
struct Dictionary;

namespace iterator {

/**
 * RLE column iterator.
 */
template<typename T>
struct rle_column_iterator
{
    friend struct Column<RLE<T>>;

    using column_type = const Column<RLE<T>>;
    using reference_type = const T&;
    using pointer_type = const T&;

    rle_column_iterator(RLE<T> *run, std::size_t idx) : run_(run), idx_(idx) { }

    rle_column_iterator & operator++() {
        ++idx_;
        if (idx_ == run_->count) {
            idx_ = 0;
            ++run_;
        }
        return *this;
    }
    rle_column_iterator & operator--() {
        if (idx_ == 0) {
            --run_;
            idx_ = run_->count;
        }
        --idx_; return *this;
    }
    rle_column_iterator operator++(int) {
        rle_column_iterator ret = *this;
        this->operator++();
        return ret;
    }
    rle_column_iterator operator--(int) {
        rle_column_iterator ret = *this;
        this->operator--();
        return ret;
    }

    bool operator==(rle_column_iterator other) const {
        return this->run_ == other.run_ and this->idx_ == other.idx_;
    }
    bool operator!=(rle_column_iterator other) const { return not operator==(other); }

    reference_type operator*() const { return run_->value; }
    pointer_type operator->() const { return & this->operator*(); }

    friend std::ostream & operator<<(std::ostream &out, rle_column_iterator it) {
        return out << "rle_column_iterator<" << typeid(T).name() << "> (" << *it.run_ << ", " << it.idx_ << ')';
    }

    private:
    RLE<T> *run_;     ///< the current run
    std::size_t idx_; ///< index within the current run
};

}

/**
 * Specialize Column for RLE containers.
 */
template<typename T>
struct Column<RLE<T>> : GenericColumn
{
    using rle_type = RLE<T>;

    Column() : GenericColumn(sizeof(RLE<T>)) { }

    /* Iterator. */
    friend struct iterator::rle_column_iterator<T>;
    using const_iterator = iterator::rle_column_iterator<T>;
    const_iterator begin() const {
        return const_iterator(&static_cast<RLE<T>*>(data_)[0], 0);
    }
    const_iterator end() const {
        RLE<T> &run = static_cast<RLE<T>*>(data_)[size_];
        return const_iterator(&run, 0);
    }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    /* RLE run iterator. */
    private:
    template<bool C> using the_run_iterator = iterator::column_iterator<C, RLE<T>>;
    template<bool C, typename X> friend struct iterator::column_iterator;
    public:
    using run_iterator = the_run_iterator<false>;
    using const_run_iterator = the_run_iterator<true>;
    run_iterator runs_begin() { return run_iterator(*this, 0); }
    run_iterator runs_end() { return run_iterator(*this, size_); }
    const_run_iterator runs_begin() const { return const_run_iterator(*this, 0); }
    const_run_iterator runs_end() const { return const_run_iterator(*this, size_); }
    const_run_iterator runs_cbegin() const { return runs_begin(); }
    const_run_iterator runs_cend() const { return runs_end(); }

    virtual std::size_t size() const { return num_rows_; }
    virtual std::size_t capacity() const { return num_rows_; }
    std::size_t num_runs() const { return size_; }

    /** Appends an element at the end of the column. */
    void push_back(T value);

    friend std::ostream & operator<<(std::ostream &out, const Column<RLE<T>> &column) {
        return out << "Column<RLE<" << typeid(T).name() << ">> (" << column.num_rows_ << " elements, "
                   << column.elem_size_ << "B)";
    }
    DECLARE_DUMP_VIRTUAL

    private:
    std::size_t num_rows_ = 0;
};

/**
 * Specialize Column for generic dictionary compression.
 */
template<typename T, typename S>
struct Column<Dictionary<T, S>> : public Column<typename Dictionary<T, S>::index_type>
{
    using dictionary_type = Dictionary<T, S>;
    using Base = Column<typename dictionary_type::index_type>;

    /** Appends an element at the end of the column. */
    void push_back(T value);

    /** Returns the underlying dictionary. */
    const dictionary_type & get_dictionary() const { return dict_; }

    friend std::ostream & operator<<(std::ostream &out, const Column &column) {
        return out << "Column<Dictionary<" << typeid(T).name() << "> (" << column.size() << '/' << column.capacity()
                   << " elements, " << column.dict_.size() << " dictionary entries)";
    }
    DECLARE_DUMP_VIRTUAL

    private:
    dictionary_type dict_;
};

/**
 * Specialize the RLE column for dictionary compression.
 */
template<typename T, typename S>
struct Column<RLE<Dictionary<T, S>>> : public Column<RLE<typename Dictionary<T, S>::index_type>>
{
    using dictionary_type = Dictionary<T, S>;
    using rle_type = RLE<typename dictionary_type::index_type>;
    using Base = Column<rle_type>;

    /** Appends an element at the end of the column. */
    void push_back(T value);

    /** Returns the underlying dictionary. */
    const dictionary_type & get_dictionary() const { return dict_; }

    virtual std::size_t size_in_bytes() const { return Base::size_in_bytes() + dict_.size() * sizeof(T); }
    virtual std::size_t capacity_in_bytes() const { return Base::capacity_in_bytes() + dict_.size() * sizeof(T); }

    friend std::ostream & operator<<(std::ostream &out, const Column &column) {
        return out << "Column<RLE<Dictionary<" << typeid(T).name() << "> (" << column.size() << '/'
                   << column.capacity() << " elements, " << column.dict_.size() << " dictionary entries, "
                   << column.num_runs() << " runs)";
    }
    DECLARE_DUMP_VIRTUAL

    private:
    dictionary_type dict_;
};

/** This method takes a ColumnStore and its Relation, and returns a new, compressed ColumnStore instance. */
ColumnStore * compress_columnstore_lineitem(const Relation &relation, const ColumnStore &store);

}
