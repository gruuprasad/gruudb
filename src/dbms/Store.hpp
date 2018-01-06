/*--- Store.hpp --------------------------------------------------------------------------------------------------------
 *
 * This file provides the interface of a database store.  The database store is responsible for organizing the data
 * physically and provides a general interface for other database components.  The database store implements physical
 * data independence.
 *
 *--------------------------------------------------------------------------------------------------------------------*/


#pragma once

#include "dbms/assert.hpp"
#include "dbms/Schema.hpp"
#include "dbms/util.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <type_traits>
#include <typeinfo>
#include <vector>


namespace dbms {

struct Store;
struct RowStore;
struct ColumnBase;
struct GenericColumn;
template<typename T> struct Column;
struct ColumnStore;

namespace iterator {

template<bool C>
struct rowstore_iterator
{
    friend struct RowStore;

    static constexpr bool is_const = C;
    using store_type = typename std::conditional_t<is_const, const RowStore, RowStore>;
    using void_type = typename std::conditional_t<is_const, const void*, void*>;

    rowstore_iterator(store_type &store, std::size_t idx)
        : store_(store)
        , row_(static_cast<uint8_t*>(store.data_) + store_.row_size_ * idx)
    { }

    rowstore_iterator & operator++() { row_ += store_.row_size_; return *this; }
    rowstore_iterator & operator--() { row_ -= store_.row_size_; return *this; }
    rowstore_iterator & operator++(int) {
        rowstore_iterator ret = *this;
        this->operator++();
        return ret;
    }
    rowstore_iterator & operator--(int) {
        rowstore_iterator ret = *this;
        this->operator--();
        return ret;
    }

    void_type operator*() const { return row_; }

    bool operator==(rowstore_iterator other) const { return this->row_ == other.row_; }
    bool operator!=(rowstore_iterator other) const { return not operator==(other); }

    template<typename T>
    std::conditional_t<is_const, const T&, T&> get(std::size_t offset) const;

    private:
    store_type &store_;
    uint8_t *row_;
};

template<bool C, typename T>
struct column_iterator
{
    friend struct Column<T>;

    static constexpr bool is_const = C;
    using column_type = typename std::conditional_t<is_const, const Column<T>, Column<T>>;
    using reference_type = typename std::conditional_t<is_const, const T&, T&>;
    using pointer_type = typename std::conditional_t<is_const, const T*, T*>;

    column_iterator(column_type &column, std::size_t idx) : column_(column) , idx_(idx) { }

    column_iterator & operator++() { ++idx_; return *this; }
    column_iterator & operator--() { --idx_; return *this; }
    column_iterator operator++(int) {
        column_iterator ret = *this;
        this->operator++();
        return ret;
    }
    column_iterator operator--(int) {
        column_iterator ret = *this;
        this->operator--();
        return ret;
    }

    bool operator==(column_iterator other) const { return this->idx_ == other.idx_; }
    bool operator!=(column_iterator other) const { return not operator==(other); }

    reference_type operator*() const;
    pointer_type operator->() const { return & this->operator*(); }

    private:
    column_type &column_;
    std::size_t idx_;
};

}

/**
 * General interface to the different stores.
 * Note that there are no generalized data access methods.
 */
struct Store
{
    /** Returns the number of used rows. */
    virtual std::size_t size() const = 0;
    /** Returns the number of bytes used by the store. */
    virtual std::size_t size_in_bytes() const = 0;
    /** Returns the number of allocated rows. */
    virtual std::size_t capacity() const = 0;
    /** Returns the number of bytes allocated by the store. */
    virtual std::size_t capacity_in_bytes() const = 0;

    virtual void dump(std::ostream &) const = 0;
    virtual void dump() const = 0;
};

/**
 * This class implements Char(N).
 */
template<std::size_t N>
struct Char
{
    static_assert(N != 0, "Char cannot be empty");

    Char() = default;
    Char(const char *str) {
        std::size_t i = 0;
        while (i != N - 1 and *str)
            data[i++] = *str++;
        assert(i < N);
        data[i] = 0;
    }

    bool operator==(const Char &other) const { return streq(this->data, other.data); }
    bool operator!=(const Char &other) const { return not operator==(other); }

    operator const char*() const { return data; }

    friend std::ostream & operator<<(std::ostream &out, const Char &chr) {
        return out << "Char(" << N << ") \"" << chr.data << '"';
    }
    DECLARE_DUMP

    char data[N];
};

struct Varchar
{
    private:
    Varchar() : value(nullptr) { }

    public:
    friend void swap(Varchar &first, Varchar &second) {
        using std::swap;
        swap(first.value, second.value);
    }

    Varchar(const char *value) : value(strdup(value)) { }
    ~Varchar() { free((void*) value); }
    Varchar(const Varchar &other) : value(strdup(other.value)) { }
    Varchar(Varchar &&other) { swap(*this, other); }

    Varchar & operator=(Varchar other) {
        swap(*this, other);
        return *this;
    }

    operator const char*() const { return value; }

    bool operator==(Varchar other) const { return streq(this->value, other.value); }
    bool operator!=(Varchar other) const { return not this->operator==(other); }

    friend std::ostream & operator<<(std::ostream &out, const Varchar &vc) {
        return out << "Varchar \"" << vc.value << '"';
    }
    DECLARE_DUMP

    const char *value;
};
static_assert(sizeof(Varchar) == sizeof(const char*), "Varchar has incorrect size");

/**
 * This class implements a row store.  It stores tuples by placing the attributes in row-major order.
 */
struct RowStore final : public Store
{
    private:
    RowStore() : data_(nullptr), size_(0), capacity_(0) { }

    public:
    ~RowStore();
    RowStore(const RowStore&) = delete;
    RowStore(RowStore&&) = default;

    /* Factory methods. */
    static RowStore Create_Naive(const Relation &relation);
    static RowStore Create_Optimized(const Relation &relation);
    static RowStore Create_Explicit(const Relation &relation, std::size_t *order);

    std::size_t size() const { return size_; }
    std::size_t size_in_bytes() const { return size_ * row_size_; }
    std::size_t capacity() const { return capacity_; }
    std::size_t capacity_in_bytes() const { return capacity_ * row_size_; }

    std::size_t num_attributes() const { return num_attributes_; }
    std::size_t row_size() const { return row_size_; }

    /* Iterator. */
    private:
    template<bool C> using the_iterator = iterator::rowstore_iterator<C>;
    template<bool C> friend struct iterator::rowstore_iterator;
    public:
    using iterator = the_iterator<false>;
    using const_iterator = the_iterator<true>;

    iterator begin() { return iterator(*this, 0); }
    iterator end() { return iterator(*this, size_); }
    const_iterator begin() const { return const_iterator(*this, 0); }
    const_iterator end() const { return const_iterator(*this, size_); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    /** Increases the capacity of the store to a value greater or equal to new_cap. */
    void reserve(std::size_t new_cap);
    /** Appends n_rows fresh rows at the end of the store and returns an iterator to the first fresh row.  The capacity
     * is increased if necessary. */
    iterator append(std::size_t n_rows);

    friend std::ostream & operator<<(std::ostream &out, const RowStore &row_store) {
        out << "RowStore (" << row_store.size_ << '/' << row_store.capacity_ << " rows, "
            << row_store.row_size_ << "B, "
            << row_store.num_attributes_ << " attributes, offsets: [";
        for (std::size_t i = 0; i != row_store.num_attributes_; ++i) {
            if (i) out << ", ";
            out << row_store.offsets_[i];
        }
        return out << "])";
    }
    DECLARE_DUMP_VIRTUAL

    private:
    void *data_;
    std::size_t *offsets_; ///< the offsets of the different attributes within the row
    std::size_t num_attributes_; ///< number of attributes
    std::size_t row_size_; ///< size of a row in bytes
    std::size_t size_; ///< number of used rows
    std::size_t capacity_; ///< number of allocated rows
};


/**
 * A base class for all column classes.  Provides a virtual destructor for simple cleanup, and some other functions for
 * general status reports.
 */
struct ColumnBase
{
    virtual ~ColumnBase() { }

    virtual std::size_t size() const = 0;
    virtual std::size_t size_in_bytes() const = 0;
    virtual std::size_t capacity() const = 0;
    virtual std::size_t capacity_in_bytes() const = 0;

    virtual void dump(std::ostream&) const = 0;
    virtual void dump() const = 0;
};

/**
 * This class implements a generic column.  It can store elements of any type simply by taking the required element size
 * as a constructor argument.
 * NOTE: The element size provided to the constructor should accommodate for required padding bytes.  Padding is *not*
 * done by the column itself.
 */
struct GenericColumn : ColumnBase
{
    GenericColumn(std::size_t elem_size) : data_(nullptr), size_(0), capacity_(0), elem_size_(elem_size) { reserve(8); }
    ~GenericColumn() { free(data_); }
    GenericColumn(const GenericColumn &other) = delete;
    GenericColumn(GenericColumn&&) = default;

    virtual std::size_t size() const { return size_; }
    virtual std::size_t size_in_bytes() const { return size_ * elem_size_; }
    virtual std::size_t capacity() const { return capacity_; }
    virtual std::size_t capacity_in_bytes() const { return capacity_ * elem_size_; }
    std::size_t elem_size() const { return elem_size_; }

    /** Increases the capacity of the store to a value greater or equal to new_cap. */
    void reserve(std::size_t new_cap);

    friend std::ostream & operator<<(std::ostream &out, const GenericColumn &column) {
        return out << "GenericColumn (" << column.size_ << '/' << column.capacity_ << " elements, "
                   << column.elem_size_ << "B)";
    }
    DECLARE_DUMP_VIRTUAL

    protected:
    void *data_;
    std::size_t size_; ///< number of stored elements
    std::size_t capacity_; ///< number of allocated elements
    std::size_t elem_size_; ///< the size of an element in bytes
};

/**
 * This class implements a typed version of the generic column.
 */
template<typename T>
struct Column : GenericColumn
{
    Column() : GenericColumn(sizeof(T)) { }
    ~Column() {
        for (std::size_t i = 0; i != size_; ++i)
            static_cast<T*>(data_)[i].~T();
    }

    /* Iterator. */
    private:
    template<bool C> using the_iterator = iterator::column_iterator<C, T>;
    template<bool C, typename X> friend struct iterator::column_iterator;
    public:
    using iterator = the_iterator<false>;
    using const_iterator = the_iterator<true>;

    iterator begin() { return iterator(*this, 0); }
    iterator end() { return iterator(*this, size_); }
    const_iterator begin() const { return const_iterator(*this, 0); }
    const_iterator end() const { return const_iterator(*this, size_); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    virtual std::size_t size_in_bytes() const { return _size_in_bytes((T*) nullptr); }
    virtual std::size_t capacity_in_bytes() const { return _capacity_in_bytes((T*) nullptr); }

    void push_back(T value);

    friend std::ostream & operator<<(std::ostream &out, const Column<T> &column) {
        return out << "Column<" << typeid(T).name() << "> (" << column.size_ << '/' << column.capacity_ << " elements, "
                   << column.elem_size_ << "B)";
    }
    DECLARE_DUMP_VIRTUAL

    private:
    template<typename X> std::size_t _size_in_bytes(const X*) const { return size_ * sizeof(X); }
    template<typename X> std::size_t _capacity_in_bytes(const X*) const { return capacity_ * sizeof(X); }
    std::size_t _size_in_bytes(const Varchar*) const {
        std::size_t bytes = GenericColumn::size_in_bytes();
        for (auto elem : *this)
            bytes += strlen(elem.value) + 1;
        return bytes;
    }
    std::size_t _capacity_in_bytes(const Varchar*) const {
        std::size_t bytes = GenericColumn::capacity_in_bytes();
        for (auto elem : *this)
            bytes += strlen(elem.value) + 1;
        return bytes;
    }
};

/**
 * This class implements a column store.  It stores tuples by placing the attributes in column-major order.
 */
struct ColumnStore final : public Store
{
    private:
    ColumnStore() { }

    public:
    ~ColumnStore();
    ColumnStore(const ColumnStore&) = delete;
    ColumnStore(ColumnStore&&) = default;

    static ColumnStore Create_Naive(const Relation &relation);
    static ColumnStore Create_Explicit(std::initializer_list<ColumnBase*> columns);

    std::size_t size() const { return columns_[0]->size(); }
    std::size_t size_in_bytes() const;
    std::size_t capacity() const { return columns_[0]->capacity(); }
    std::size_t capacity_in_bytes() const;

    template<typename T>
    Column<T> & get_column(std::size_t offset);
    template<typename T>
    const Column<T> & get_column(std::size_t offset) const {
        return const_cast<ColumnStore*>(this)->get_column<T>(offset);
    }

    friend std::ostream & operator<<(std::ostream &out, const ColumnStore &store) {
        out << "ColumnStore (" << store.columns_.size() << " columns: [";
        for (std::size_t i = 0, end = store.columns_.size(); i != end; ++i) {
            if (i) out << ", ";
            out << store.columns_[i]->size() << '/' << store.columns_[i]->capacity();
        }
        return out << "])";
    }
    DECLARE_DUMP

    private:
    std::vector<ColumnBase*> columns_;
};

}

namespace std {

template<std::size_t N>
struct hash<dbms::Char<N>>
{
    std::size_t operator()(const dbms::Char<N> &chr) const { return StrHash{}(chr.data); }
};

template<std::size_t N>
struct equal_to<dbms::Char<N>>
{
    bool operator()(const dbms::Char<N> &first, const dbms::Char<N> &second) const {
        return streq(first.data, second.data);
    }
};

template<>
struct hash<dbms::Varchar>
{
    std::size_t operator()(const dbms::Varchar &varchar) const { return StrHash{}(varchar); }
};

template<>
struct equal_to<dbms::Varchar>
{
    bool operator()(const dbms::Varchar &first, const dbms::Varchar &second) const { return streq(first, second); }
};

}
