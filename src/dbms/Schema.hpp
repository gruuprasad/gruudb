/*--- Schema.hpp -------------------------------------------------------------------------------------------------------
 *
 * This file implements types and schemas.
 *
 *--------------------------------------------------------------------------------------------------------------------*/


#pragma once

#include "dbms/assert.hpp"
#include "dbms/macros.hpp"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <string>
#include <unordered_map>


namespace dbms {

struct Relation;

/**
 * This class represents an attribute in a SQL database schema.
 * Attributes have a type, a size, and a name.  When an attribute is part of a relation, it is also assigned an offset
 * within that relation.  Therefore, an attribute instance is owned by at most one relation.
 */
struct Attribute
{
    friend struct Relation;

#define Type(X) \
    X(TY_Char), \
    X(TY_Int), \
    X(TY_Float), \
    X(TY_Double), \
    X(TY_Varchar),

DECLARE_ENUM(Type);

    Attribute(Type type, std::size_t size, const char *name) : name(name) , type(type) , size(size) { }

    public:
    std::string name; ///< the name
    Type type; ///< the type
    std::size_t size; ///< the size in bytes
    private:
    std::size_t offset_; ///< the offset of the attribute within the relation

    public:
    /* Default constructor. */
    Attribute() = default;

    /* Convenience constructors. */
    static Attribute Int1(const char *name) { return Attribute(TY_Int, 1, name); }
    static Attribute Int2(const char *name) { return Attribute(TY_Int, 2, name); }
    static Attribute Int4(const char *name) { return Attribute(TY_Int, 4, name); }
    static Attribute Int8(const char *name) { return Attribute(TY_Int, 8, name); }
    static Attribute Float (const char *name) { return Attribute(TY_Float,  sizeof(float), name); }
    static Attribute Double(const char *name) { return Attribute(TY_Double, sizeof(double), name); }
    static Attribute Char(const char *name, std::size_t size) { return Attribute(TY_Char, size, name); }
    static Attribute Varchar(const char *name, std::size_t size) { return Attribute(TY_Varchar, size, name); }

    const char * type_name() const {
        static const char *names[] = { ENUM_TO_STR(Type) };
        return names[type];
    }

    std::size_t offset() const { return offset_; }

    friend std::ostream & operator<<(std::ostream &out, const Attribute &attr) {
        return out << "Attribute(\"" << attr.name << "\", " << attr.type_name() << ", " << attr.size << ")";
    }
    DECLARE_DUMP

#undef Type
};

/**
 * This class represents a relation in a SQL database schema.
 * A relation is an ordered list of attributes.  Upon creation, each attribute is assigned its offset within the
 * relation.
 */
struct Relation
{
    Relation(const char *name, Attribute *attributes, std::size_t size) : name(name), size_(size) {
        attributes_ = static_cast<decltype(attributes_)>(malloc(sizeof(*attributes_) * size));
        while (size--) {
            auto &attr = attributes[size];
            new (&attributes_[size]) Attribute(attr);
            attributes_[size].offset_ = size;
            std::string name(attr.name);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            name_to_attribute_[name] = size;
        }
    }

    Relation(const char *name, std::initializer_list<Attribute> attributes) : name(name), size_(attributes.size()) {
        std::transform(this->name.begin(), this->name.end(), this->name.begin(), ::tolower);
        attributes_ = static_cast<decltype(attributes_)>(malloc(sizeof(*attributes_) * attributes.size()));
        auto it = attributes.begin();
        for (std::size_t i = 0; i != attributes.size(); ++i, ++it) {
            new (&attributes_[i]) Attribute(*it);
            attributes_[i].offset_ = i;
            std::string name(it->name);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            name_to_attribute_[name] = i;
        }
    }

    ~Relation() { free(attributes_); }

    Attribute * begin() { return attributes_; }
    Attribute * end() { return attributes_ + size_; }
    const Attribute * begin() const { return attributes_; }
    const Attribute * end() const { return attributes_ + size_; }
    const Attribute * cbegin() const { return attributes_; }
    const Attribute * cend() const { return attributes_ + size_; }

    std::size_t size() const { return size_; }

    const Attribute & operator[](std::size_t i) const {
        assert(i < size_, "index out of bounds");
        return attributes_[i];
    }

    const Attribute & operator[](std::string s) const {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        const std::size_t idx = name_to_attribute_.at(s);
        return attributes_[idx];
    }

    friend std::ostream & operator<<(std::ostream &out, const Relation &relation) {
        out << "Relation \"" << relation.name << "\" (\n";
        for (std::size_t i = 0; i != relation.size_; ++i)
            out << "    [" << relation.attributes_[i].offset() << "]: " << relation.attributes_[i] << ",\n";
        out << ")";
        return out;
    }
    DECLARE_DUMP

    std::string name;
    private:
    Attribute *attributes_; ///< array of attributes
    std::size_t size_; ///< number of attributes
    std::unordered_map<std::string, std::size_t> name_to_attribute_;
};

}
