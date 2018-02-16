#pragma once

#include "dbms/assert.hpp"
#include "dbms/util.hpp"
#include <cstdint>
#include <functional>
#include <utility>


namespace dbms {

template<
    typename Key,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
struct HashTable
{
    using key_type = Key;
    using hasher = Hash;
    using key_equal = KeyEqual;
    using size_type = std::size_t;

    using reference = Key&;
    using const_reference = const Key&;
    using pointer = Key*;
    using const_pointer = const Key*;

    private:
    template<bool C>
    struct the_iterator
    {
        friend struct HashTable;

        static constexpr bool is_const = C;
        using pointer_type = std::conditional_t<is_const, const_pointer, pointer>;
        using reference_type = std::conditional_t<is_const, const_reference, reference>;
        using table_type = std::conditional_t<is_const, const HashTable, HashTable>;

        the_iterator(table_type &container, std::size_t idx) : container_(container), idx_(idx) {  }

        /** Compare this iterator with an other iterator for equality. */
        bool operator==(the_iterator other) const { return this->idx_ == other.idx_; }
        bool operator!=(the_iterator other) const { return not operator==(other); }

        /** Advance the iterator to the next element. */
        the_iterator & operator++() {
            if (idx_ == container_.capacity_)
                return *this;
            else {
                idx_++;
                while (idx_ < container_.capacity_ && container_.table_[idx_].first == false)
                    idx_++;
            }
            return *this;
        }

        /** Return a reference to the designated element */
        reference_type operator*() const { return container_.table_[idx_].second; }
        /** Return a pointer to the designated element. */
        pointer_type operator->() const { return & this->operator*(); }

        private:
        table_type &container_;
        std::size_t idx_;
    };
    public:
    using iterator = the_iterator<false>;
    using const_iterator = the_iterator<true>;

    iterator begin() { return iterator(*this, min_index); }
    iterator end()   { return iterator(*this, capacity_); }
    const_iterator begin() const { return const_iterrator(*this, min_index); }
    const_iterator end()   const { return const_iterator(*this, capacity_); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

    HashTable(std::size_t capacity = 131072): table_(nullptr), size_(0), capacity_(capacity), min_index(capacity)
    {
        table_ = new std::pair<bool, key_type>[capacity];
    }

    ~HashTable() {
        delete[] table_;
    }

    size_type size() const { return size_; }
    size_type capacity() const { return capacity_; }

    std::size_t find_helper(const key_type &key) {
        std::size_t index = hasher{}(key) % capacity_;
        while (!key_equal{}(table_[index].second, key) && table_[index].first == true)
            index = (index + 1) % capacity_;
        if (key_equal{}(table_[index].second, key)) 
            return index;
        else
            return capacity_;
    }

    /** If an element with key exists, returns an iterator to that element.  Otherwise, returns end(). */
    iterator find(const key_type &key) {
        auto index = find_helper(key);
        if (index == capacity_)
            return iterator(*this, capacity_);
        else
            return iterator(*this, index); 

    }

    const_iterator find(const key_type &key) const {
        auto index = find_helper(key);
        if (index == capacity_)
            return const_iterator(*this, capacity_);
        else
            return const_iterator(*this, index); 
    }

    double load_factor() {
        return (double) size() / capacity();
    }

    private:
    std::pair<std::size_t, bool> insert_helper(const key_type &key) {
        std::size_t index = hasher{}(key) % capacity_;
        if (index < min_index)
            min_index = index;
        while (table_[index].first == true) {
            if (key_equal{}(table_[index].second, key))
                return std::make_pair(index, false);
            index = (index + 1) % capacity_;
        }
        table_[index] = std::make_pair(true, key);
        size_++;
        return std::make_pair(index, true);
    }

    void resize() {
        std::size_t old_capacity = capacity_;
        std::pair<bool, key_type> * old_table = table_;
        capacity_ += 2 * capacity_;
        size_ = 0;
        table_ = new std::pair<bool, key_type>[capacity_];

        for (std::size_t i = 0; i < old_capacity; ++i) {
            if (old_table[i].first == true) 
                insert_helper(old_table[i].second);
        }

        delete[] old_table;
    }

    public:

    /** Returns an iterator to the element in the table and a flag whether insertion succeeded.  The flag is true, if
     * the element was newly inserted into the table, and false otherwise.  The iterator designates the newly inserted
     * element respectively the element already present in the table. */
    std::pair<iterator, bool> insert(const key_type &key) {
        if (load_factor() > .85) {
            resize();
        }
        auto result = insert_helper(key);
        return std::make_pair(iterator(*this, result.first), result.second);
    }

    private:
    std::pair<bool, key_type> *table_;
    std::size_t size_;
    std::size_t capacity_;
    std::size_t min_index;
};

template<
    typename Key,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
using hash_set = HashTable<Key, Hash, KeyEqual>;

namespace {

template<typename Key, typename Value, typename Hash>
struct map_hash
{
    std::size_t operator()(const std::pair<Key, Value> &pair) const {
        return Hash{}(pair.first);
    }
};

template<typename Key, typename Value, typename KeyEqual>
struct map_equal
{
    bool operator()(const std::pair<Key, Value> &first, const std::pair<Key, Value> &second) const {
        return KeyEqual{}(first.first, second.first);
    }
};

}

template<
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
struct hash_map
{
    public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using hasher = map_hash<const Key, Value, Hash>;
    using key_equal = map_equal<const Key, Value, KeyEqual>;
    using size_type = std::size_t;

    using reference = Key&;
    using const_reference = const Key&;
    using pointer = Key*;
    using const_pointer = const Key*;

    private:
    using table_type = HashTable<std::pair<key_type, mapped_type>, hasher, key_equal>;

    public:
    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::const_iterator;


    public:
    hash_map() { }
    /* OPTIONAL TODO 3.1 - define alternative constructors */

    iterator begin() { return table_.begin(); }
    iterator end() { return table_.end(); }
    const_iterator begin() const { return table_.begin(); }
    const_iterator end() const { return table_.end(); }
    const_iterator cbegin() const { return table_.cbegin(); }
    const_iterator cend() const { return table_.cend(); }

    size_type size() const { return table_.size(); }
    size_type capacity() const { return table_.capacity(); }

    iterator find(const key_type &key) { return table_.find({key, mapped_type()}); }
    const_iterator find(const key_type &key) const { return table_.find({key, mapped_type()}); }

    std::pair<iterator, bool> insert(const value_type &value) { return table_.insert(value); }

    mapped_type & operator[](const key_type& key) {
        return table_.insert(std::make_pair(key, mapped_type())).first->second;
    }

    private:
    table_type table_;
};

}
