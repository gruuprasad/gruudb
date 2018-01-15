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

        the_iterator(pointer_type &pair, std::size_t idx ) : pair_(pair), idx_(idx) { }

        /** Compare this iterator with an other iterator for equality. */
        bool operator==(the_iterator other) const { return (this->idx_ == other.idx_)&&(this->pair.first() == other->pair.first())&&(this->pair.second() == other->pair.second()) ;}
        bool operator!=(the_iterator other) const { not operator==(other); }

        /** Advance the iterator to the next element. */
        the_iterator & operator++() {
           ++idx_; return *this;
        }

        /** Return a pointer to the designated element. */
        pointer_type operator->() const { return & this->operator*(); }
        /** Return a reference to the designated element */
        reference_type operator*() const { /* TODO 3.2.3 */ dbms_unreachable("Not implemented."); }

        /* TODO 3.2.3 - declare fields */
        private:
            pointer_type &pair_;
            std::size_t idx_;
            
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
     private :
        
        inner_node() : first_child_node(nullptr) is_leaf(false)  {}
    
    public:
        
        ~inner_node()
        { 
            for( auto element : map)
                delete map.second;
            delete first_child_node;
        }
        
        void insert(node *node)
        {
            if(first_child_node != nullptr)
            {
                map.push_back(std::make_pair(node.first_key(), node));
            }
            else
            {
                first_child_node = node;
            }
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
        
    private :
        
        leaf_node() : next_leaf_node(nullptr) is_leaf(true) {}
        
    public:
        
        ~leaf_node() { delete next_leaf_node; }
        
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

       int size_inode = 5;
       int size_leaf = 10;
       leaf_node *current_leaf;
       // list of last node of each row of the B+ tree
       std::vector<node*> map_last_node;
       current_leaf = new leaf_node();
       
       // we can check if our B+ tree is ok if the size of this map is equal to the theoric height if the B+ tree 
       map_last_node.push_back(new inner_node().insert(current_leaf));
       
       for(It it = begin; it!=end; ++it)
       {
           /* check if the current leaf_node is full */
            if(current_leaf->contains(size_leaf))
            {
                /* reset the current_leaf node to an empty one */
                leaf_node *new_leaf = new leaf_node();
                current_leaf.insert_next_leaf(new_leaf);
                current_leaf = new_leaf;
                current_leaf->insert(*it);
                
                // node who is going to be inserted in the upper node
                node * current_new_node = current_leaf;
                
                /* update all upper node */
                for(auto node_iterator= map_last_node.begin(); node_iterator!=end(); node_iterator++)
                {
                    // if current parrent full
                    if(node_iterator->contains(size_inode))
                    {
                        if(node_iterator == map_last_node.back())
                        {
                            map_last_node.push_back(new inner_node().insert(*node_iterator));
                        }
                        inner_node *new_node = new inner_node();
                        new_node.insert(current_new_node);
                        current_new_node = new_node;
                        *node_iterator = new_node;
                    }
                    else
                    {
                        node_iterator->insert(current_new_node);
                        break;
                    }
                }
            }
            else
            {
                current_leaf->insert(std::make_pair(it->first,it->second));
            }
       }
       
       // root node is the last element of map_last_node
        static BPlusTree bptree(map_last_node.back());
        return bptree;
    }


    /*--- Start of B+-Tree code --------------------------------------------------------------------------------------*/
    private:
    BPlusTree(node * the_root_node) { root_node = the_root_node; }

    public:
    BPlusTree(const BPlusTree&) = delete;
    BPlusTree(BPlusTree&&) = default;

    ~BPlusTree() { delete root_node; }


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
    inner_node *root_node;
};

}
