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
    
    struct inner_node : node
    {
        /* TODO 3.2.1 */
        
     private :
        
        inner_node() : first_child_node(nullptr) is_leaf(false)  {}
    
    public:
        
        void insert(key_type key, node *right_node)
        {
            map.push_back(std::make_pair(key, right_node));
        }
        
        inner_node(node *first_lower_node)
        {
            first_child_node = first_lower_node;
        }
        
        // check if it contains k elements 
        
        bool contains(int k)
        {
            return (k == map.size());
        }
        
        bool is_child_leaf()
        {
            return is_leaf(first_child_node);
        }
        
        key_type first_key()
        {
            return ((map.at(0)).first);
        }
        
    private:
        
        std::vector<std::pair<key_type, node*>> map;
        node *first_child_node;
    };
    
    struct node
    {
        bool is_leaf()
        {
            return is_leaf;
        }
        
        bool is_leaf;
    }

    struct leaf_node : node
    {
        /* TODO 3.2.1 */
        
    private :
        
        leaf_node() : next_leaf_node(nullptr) is_leaf(true) {}
        
    public:
        
        void insert(value_type element)
        {
            map.push_back(element);
        }
        
        void insert_next_leaf(leaf_node *next_leaf)
        {
            next_leaf_node = next_leaf;
        }
        
        const Key first_key()
        {
            return ((map.at(0)).first);
        }
        
        bool contains(int k)
        {
            return (k == map.size());
        }
        
        
    private :
        std::vector<value_type> map;
        leaf_node *next_leaf_node;
    };


    /*--- Factory methods --------------------------------------------------------------------------------------------*/
    template<typename It>
    static BPlusTree Bulkload(It begin, It end) {
        /* TODO 3.2.2 */
       
        
       int size_inode = 5;
       int size_leaf = 10;
       inner_node *current_root;
       leaf_node *current_leaf;
       
       current_root = nullptr;
       current_leaf = new leaf_node();
       
       for(It it = begin; it!=end; ++it)
       {
           if(current_root != nullptr)
           {
                current_leaf->insert(std::make_pair(it->first,it->second));
                /* check if the current leaf_node is full */
                if(current_leaf->contains(size_leaf))
                {
                    /* if current root is full */
                    if(current_root.contains(size_inode))
                    {
                        /* create a new root node based on the previous one */
                        current_root =  new inner_node(current_root);
                        /* reset the current_leaf node to an empty one */
                        current_leaf = new leaf_node();
                    } 
                    // we can still insert in the current root
                    else
                    {
                        /* if child pointers are nodes */
                        if(current_root->is_child_leaf())
                        {
                            current_root.insert(current_leaf->first_key(),current_leaf);
                        }
                        else
                        {
                            /* build the tree with the length of highest node */
                        }
                    }
                }
           }
           else
           {
               current_leaf->insert(std::make_pair(it->first,it->second));
               /* check if the first leaf_node is full */
               if(current_leaf->contains(size_leaf))
               {
                   /* create the inner_node */
                    current_root =  new inner_node(current_leaf);
                    /* reset the current_leaf node to an empty one */
                    current_leaf = new leaf_node();
               }
           }
       }
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
    inner_node *root_node;
};

}
