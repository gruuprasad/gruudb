#pragma once

#include "dbms/assert.hpp"
#include "dbms/macros.hpp"
#include "dbms/util.hpp"
#include <algorithm>
#include <utility>
#include <vector>


namespace dbms {

template<
    typename Key,
    typename Value,
    typename Compare = std::less<Key>>
struct BPlusTree
{
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;
    using size_type = std::size_t;
    using key_compare = Compare;

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    /*--- Iterator ---------------------------------------------------------------------------------------------------*/
    private:
    template<bool C>
    struct the_iterator
    {
        friend struct BPlusTree;

        static constexpr bool is_const = C;
        using pointer_type = std::conditional_t<is_const, const_pointer, pointer>;
        using reference_type = std::conditional_t<is_const, const_reference, reference>;

        the_iterator() { /* TODO 3.2.3 */ }

        /** Compare this iterator with an other iterator for equality. */
        bool operator==(the_iterator other) const { /* TODO 3.2.3 */ dbms_unreachable("Not implemented."); }
        bool operator!=(the_iterator other) const { /* TODO 3.2.3 */ dbms_unreachable("Not implemented."); }

        /** Advance the iterator to the next element. */
        the_iterator & operator++() {
            /* TODO 3.2.3 */
            dbms_unreachable("Not implemented.");
        }

        /** Return a pointer to the designated element. */
        pointer_type operator->() const { /* TODO 3.2.3 */ dbms_unreachable("Not implemented."); }
        /** Return a reference to the designated element */
        reference_type operator*() const { /* TODO 3.2.3 */ dbms_unreachable("Not implemented."); }

        /* TODO 3.2.3 - declare fields */
    };
    public:
    using iterator = the_iterator<false>;
    using const_iterator = the_iterator<true>;

    /*--- Range Type -------------------------------------------------------------------------------------------------*/
    private:
    template<bool C>
    struct the_range
    {
        friend struct BPlusTree;

        the_range(the_iterator<C> begin, the_iterator<C> end) : begin_(begin), end_(end) {
            key_compare lt;
            assert(not lt(end_->first, begin_->first)); // begin <= end
        }

        the_iterator<C> begin() const { return begin_; }
        the_iterator<C> end() const { return end_; }

        bool empty() const { return begin_ == end_; }

        friend std::ostream & operator<<(std::ostream &out, the_range range) {
            return out << "the_range<" << (C ? "true" : "false") << "> from "
                       << range.begin_->first << " to " << range.end_->first;
        }
        DECLARE_DUMP

        private:
        the_iterator<C> begin_;
        the_iterator<C> end_;
    };
    public:
    using range = the_range<false>;
    using const_range = the_range<true>;

    /*--- Tree Node Data Types ---------------------------------------------------------------------------------------*/
    struct inner_node
    {
        /* TODO 3.2.1 */
    };

    struct leaf_node
    {
        /* TODO 3.2.1 */
    };


    /*--- Factory methods --------------------------------------------------------------------------------------------*/
    template<typename It>
    static BPlusTree Bulkload(It begin, It end) {
        /* TODO 3.2.2 */
        dbms_unreachable("Not implemented.");
    }


    /*--- Start of B+-Tree code --------------------------------------------------------------------------------------*/
    private:
    BPlusTree(/* TODO 3.2.2 */) { /* TODO 3.2.2 */ dbms_unreachable("Not implemented."); }

    public:
    BPlusTree(const BPlusTree&) = delete;
    BPlusTree(BPlusTree&&) = default;

    ~BPlusTree() { /* TODO 3.2.1 */ dbms_unreachable("Not implemented."); }


    iterator begin() { /* TODO 3.2.3 */ dbms_unreachable("Not implemented."); }
    iterator end()   { /* TODO 3.2.3 */ dbms_unreachable("Not implemented."); }
    const_iterator begin() const { /* TODO 3.2.3 */ dbms_unreachable("Not implemented."); }
    const_iterator end()   const { /* TODO 3.2.3 */ dbms_unreachable("Not implemented."); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

    const_iterator find(const key_type key) const {
        /* TODO 3.2.3 */
        dbms_unreachable("Not implemented.");
    }
    iterator find(const key_type &key) {
        /* TODO 3.2.3 */
        dbms_unreachable("Not implemented.");
    }

    const_range in_range(const key_type &lower, const key_type &upper) const {
        /* TODO 3.2.3 */
        dbms_unreachable("Not implemented.");
    }
    range in_range(const key_type &lower, const key_type &upper) {
        /* TODO 3.2.3 */
        dbms_unreachable("Not implemented.");
    }

    private:
    /* TODO 3.2.1 - declare fields */
};

}
