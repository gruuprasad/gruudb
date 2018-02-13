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

        the_iterator(BPlusTree &btree, std::size_t idx ) : btree_(btree), idx_(idx) { }

        /** Compare this iterator with an other iterator for equality. */
        bool operator==(the_iterator other) const { return this->idx_ == other.idx_;}
        bool operator!=(the_iterator other) const { return not operator==(other); }

        /** Advance the iterator to the next element. */
        the_iterator & operator++() {
            idx_++; return *this;
        }

        /** Return a pointer to the designated element. */
        pointer_type operator->() const { return &(this->operator*()); }
        /** Return a reference to the designated element */
        reference_type operator*() const { 
            std::size_t leaf_index = idx_ / 400;
            std::size_t offset = idx_ % 400;
            leaf_node* leaf_of_interest = btree_.first_leaf;
            for (auto i = 0; i < leaf_index; ++i) {
                leaf_of_interest = btree_.first_leaf->next_leaf_node;
            }
            return leaf_of_interest->map[offset];
        }

        private:
            BPlusTree &btree_;
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
    struct node
    {
        node(bool leaf) : m_leaf(leaf) {}
        virtual key_type first_key() = 0;
        virtual key_type last_key() = 0;
        bool is_leaf()
        {
            return m_leaf;
        }
        
        bool m_leaf;
    };
    
    struct inner_node : node
    {
    public:
        
        inner_node() : node(false), first_child_node(nullptr)  {}
        
        ~inner_node()
        { 
            for( auto element : map)
                delete element.second;
            delete first_child_node;
        }
        
        void insert(node *lower_node)
        {
            if(first_child_node != nullptr)
            {
                map.push_back(std::make_pair(lower_node->first_key(), lower_node));
            }
            else
            {
                first_child_node = lower_node;
            }
        }
        
        
        // check if it contains k elements 
        
        bool contains(int k)
        {
            return (k == static_cast<int>(map.size()));
        }
        
        bool is_child_leaf()
        {
            return first_child_node->is_leaf();
        }
        
        key_type first_key()
        {
            if (0 < static_cast<int>(map.size()))
            return ((map.at(0)).first);
            else 
            return first_child_node->last_key()+1;
        }
        
        key_type last_key()
        {
            return (map.back().first);
        }
        
        std::vector<std::pair<key_type, node*>> map;
        node *first_child_node;
    };
    
    

    struct leaf_node : node
    {
        
    public:
        leaf_node(std::size_t index = 0) : node(true), next_leaf_node(nullptr), index(index) {}

        ~leaf_node() { delete next_leaf_node; }
        
        void insert(value_type element)
        {
            map.push_back(element);
        }
        
        void insert_next_leaf(leaf_node *next_leaf)
        {
            next_leaf_node = next_leaf;
        }
        
        key_type first_key()
        {
            if (0 < static_cast<int>(map.size()))
            return ((map.at(0)).first);
            else
            return 0;
        }
        
        key_type last_key()
        {
            return (map.back().first);
        }
        
        bool contains(int k)
        {
            return (k == static_cast<int>(map.size()));
        }
        
        std::vector<value_type> map;
        leaf_node *next_leaf_node;
        std::size_t index;
    };


    /*--- Factory methods --------------------------------------------------------------------------------------------*/
    template<typename It>
    static BPlusTree Bulkload(It begin, It end) {

        std::size_t size = 0;
        std::size_t index = 0;
       int size_inode = 200;
       int size_leaf = 400;
       leaf_node *current_leaf;
       inner_node* n_inode;
       inner_node* new_root = nullptr;
       
       // list of last node of each row of the B+ tree
       std::vector<inner_node*> map_last_node;
       current_leaf = new leaf_node(index);
       leaf_node* first_leaf = current_leaf;
       
       // we can check if our B+ tree is ok if the size of this map is equal to the theoric height if the B+ tree 

       inner_node* inode = new inner_node;
       inode->insert(current_leaf);
       map_last_node.push_back(inode);
       
       for(It it = begin; it!=end; ++it)
       {
           /* check if the current leaf_node is full */
            if(current_leaf->contains(size_leaf))
            {
                /* reset the current_leaf node to an empty one */
                leaf_node *new_leaf = new leaf_node(++index);
                current_leaf->insert_next_leaf(new_leaf);
                current_leaf = new_leaf;
                current_leaf->insert(*it);
                
                // node who is going to be inserted in the upper node
                node * current_new_node = current_leaf;
                
                if((map_last_node.back())->contains(size_inode))
                {
                    
                    n_inode = new inner_node();
                    n_inode->insert(map_last_node.back());
                    new_root = n_inode;
                   // map_last_node.push_back(n_inode);
                }
                
                if(new_root != nullptr)
                {
                    if((map_last_node.back())->map.size()>=1)
                    {
                        new_root->insert(map_last_node.back());
                        map_last_node.push_back(new_root);
                        new_root = nullptr;
                    }
                }
                
                /* update all upper node */
                for(auto node_iterator= map_last_node.begin(); node_iterator!=map_last_node.end(); node_iterator++)
                {
                    // if current parent full
                    if((*node_iterator)->contains(size_inode))
                    {
                     /*   if(*node_iterator == map_last_node.back())
                        {
                            n_inode = new inner_node();
                            n_inode->insert(*node_iterator);
                            map_last_node.push_back(n_inode);
                        }
                    */
                        inner_node *new_node = new inner_node();
                        new_node->insert(current_new_node);
                        current_new_node = new_node;
                        *node_iterator = new_node;
                    }
                    else
                    {
                        (*node_iterator)->insert(current_new_node);
                        break;
                    }
                }
            }
            else
            {
                current_leaf->insert(*it);
            }
       size++;
       }

       if(new_root == nullptr)
       {
        // root node is the last element of map_last_node
           if((map_last_node.back())->map.size() ==0)
           {
                (map_last_node.back())->map.push_back(std::make_pair(((map_last_node.back())->first_child_node)->last_key()+1, nullptr));
           }
            BPlusTree bptree(map_last_node.back(), first_leaf, size);
            return bptree;
        }
        else
        {
            (map_last_node.back())->map.push_back(std::make_pair(((map_last_node.back())->first_child_node)->last_key()+1, nullptr));
            new_root->insert(map_last_node.back());
            BPlusTree bptree(new_root, first_leaf, size);
            return bptree;
        }
    }


    /*--- Start of B+-Tree code --------------------------------------------------------------------------------------*/
    private:
    BPlusTree(inner_node * the_root_node, leaf_node* first_leaf, std::size_t size):root_node(the_root_node), first_leaf(first_leaf), size_(size) {}

    public:
    BPlusTree(const BPlusTree&) = delete;
    BPlusTree(BPlusTree&&) = default;

    ~BPlusTree() { delete root_node; }


    iterator begin() { return iterator(*this, 0); }
    iterator end()   { return iterator(*this, size_); }
    const_iterator begin() const { return const_iterator(*this, 0); }
    const_iterator end()   const { return const_iterator(*this, size_); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

    std::pair<bool, std::size_t> find_helper(const key_type &key) {
        inner_node * node_t = root_node;
        int i = 0;
        while (!node_t->is_child_leaf()) {
            i = 0;
            if (i < node_t->map.size() && node_t->map[i].first > key) 
                node_t = static_cast<inner_node *>(node_t->first_child_node);
            else {
                while (i < node_t->map.size() && node_t->map[i].first <= key)
                    ++i;
                node_t = static_cast<inner_node *>(node_t->map[i-1].second);
                }
        }
        leaf_node* node_c;
        i = 0;
        if (i < node_t->map.size() && key < node_t->map[i].first) 
            node_c = static_cast<leaf_node *>(node_t->first_child_node);
        else {
            while (i < node_t->map.size() && key >= node_t->map[i].first)
                ++i;
            node_c = static_cast<leaf_node *>(node_t->map[i-1].second);
        }
        int j = 0;
        if(node_c !=nullptr)
        {
            while (j < node_c->map.size() && node_c->map[j].first <= key) {
                if (node_c->map[j].first == key)
                    return std::make_pair(true, (node_c->index * 400) + j);
                ++j;
            }
        }
        return std::make_pair(false, (node_c->index * 400) + j);
    }

    const_iterator find(const key_type key) const {
        auto result = find_helper(key);
        if (result.first == true)
            return const_iterator(*this, result.second);
        else 
            return const_iterator(*this, size_);
    }
    
    iterator find(const key_type &key) {
        auto result = find_helper(key);
        if (result.first == true)
            return iterator(*this, result.second);
        else
            return iterator(*this, size_);
    }

    const_range in_range(const key_type &lower, const key_type &upper) const {
        auto lower_result = find_helper(lower);
        auto upper_result = find_helper(upper);
        if (upper_result.first == true)
            return const_range(const_iterator(*this, lower_result.second), const_iterator(*this, upper_result.second));
        else
            return const_range(const_iterator(*this, lower_result.second), const_iterator(*this, upper_result.second -1));
    }

    range in_range(const key_type &lower, const key_type &upper) {
        auto lower_result = find_helper(lower);
        auto upper_result = find_helper(upper);
        if (upper_result.first == true)
            return range(iterator(*this, lower_result.second), iterator(*this, upper_result.second));
        else
            return range(iterator(*this, lower_result.second), iterator(*this, upper_result.second -1));
    }

    private:
    inner_node *root_node;
    leaf_node *first_leaf;
    std::size_t size_;
};

}
