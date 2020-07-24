// (c) 2016 Yakov Litvitskiy <thedsi100@gmail.com>
// See LICENSE.txt
#ifndef GUARD_EnumReflection_H_INCLUDED
#define GUARD_EnumReflection_H_INCLUDED
#pragma once

#include <string>
#include <vector>
#include <cassert>

//-------------------------------- Public Interface --------------------------------

// Declare an enumeration inside a class
#define Z_ENUM( enumName, enumType, ... )\
                              Z_ENUM_DETAIL_MAKE( class, enumName, enumType, __VA_ARGS__ )

// Declare an enumeration inside a namespace
#define Z_ENUM_NS( enumName, enumType, ... )\
                              Z_ENUM_DETAIL_MAKE( namespace, enumName, enumType, __VA_ARGS__ )

// Provides access to information about an enum declared with Z_ENUM or Z_ENUM_NS
class EnumReflector
{
public:
    // Returns a reference to EnumReflector object which can be used
    // to retrieve information about the enumeration declared with Z_ENUM or Z_ENUM_NS
    template<typename EnumType>
    static const EnumReflector& For(EnumType val = EnumType());

    // Represents an enumerator (value) of an enumeration
    class Enumerator
    {
    public:
        // Returns enumerator name
        const std::string& Name() const;

        // Returns enumerator value
        int Value() const;

        // Returns enumerator index
        int Index() const;

        // Returns parent reflector object
        const EnumReflector& Reflector() const;

        // Check if this is an valid Enumerator
        bool IsValid() const;
        operator bool() const;

        // Check if two objects are the same
        bool operator!=(const Enumerator& rhs) const;

        // Moves on to the next Enumerator in enum
        Enumerator& operator++();

        // Provided for compatibility with range-based for construct
        const Enumerator& operator*() const;

    private:
        friend class EnumReflector;
        Enumerator(const EnumReflector&, int);
        const EnumReflector& _er;
        int _index;
    };

    // Returns Enumerator count
    int Count() const;

    // Returns an Enumerator with specified name or invalid Enumerator if not found
    Enumerator Find(const std::string& name) const;

    // Returns an Enumerator with specified value or invalid Enumerator if not found
    Enumerator Find(int value) const;

    // Returns the enumeration name
    const std::string& EnumName() const;

    // Returns Enumerator at specified index
    Enumerator At(int index) const;
    Enumerator operator[](int index) const;
    
    // In some cases Enumerators can be used as iterators. The following functions
    // are provided e.g. for compatibility with range-based for construct:

    // Returns the first Enumerator
    Enumerator begin() const;

    // Returns an invalid Enumerator
    Enumerator end() const;

public:
    // Constructor. Used internally by Z_ENUM and Z_ENUM_NS
    template<typename EnumType>
    EnumReflector(const EnumType*, size_t, const char*, const char*);
    EnumReflector(EnumReflector&&);
    ~EnumReflector();
private:
    struct Private
    {
        struct Enumerator
        {
            std::string name;
            int value;
        };
        std::vector<Enumerator> values;
        std::string enumName;
    };
    Private* _data;
};

//----------------------------- Implementation Details -----------------------------

#define Z_ENUM_DETAIL_SPEC_namespace \
    extern "C"{/* Protection from being used inside a class body */} \
    inline
#define Z_ENUM_DETAIL_SPEC_class friend
#define Z_ENUM_DETAIL_STR(x) #x
#define Z_ENUM_DETAIL_MAKE(spec, enumName, enumType, ...) \
    enum enumName:enumType                                                                           \
    {                                                                                           \
        __VA_ARGS__                                                                             \
    };                                                                                          \
    Z_ENUM_DETAIL_SPEC_##spec const ::EnumReflector& _detail_reflector_(enumName)               \
    {                                                                                           \
        static const ::EnumReflector _reflector( []{                                            \
            static enumType _detail_sval;                                                            \
            _detail_sval = 0;                                                                   \
            struct _detail_val_t                                                                \
            {                                                                                   \
                _detail_val_t(const _detail_val_t& rhs) : _val(rhs) { _detail_sval = _val + 1; }\
                _detail_val_t(enumType val)                  : _val(val) { _detail_sval = _val + 1; }\
                _detail_val_t()                 : _val(_detail_sval){ _detail_sval = _val + 1; }\
                                                                                                \
                _detail_val_t& operator=(const _detail_val_t&) { return *this; }                \
                _detail_val_t& operator=(enumType) { return *this; }                                 \
                operator enumType() const { return _val; }                                           \
                enumType _val;                                                                       \
            } __VA_ARGS__;                                                                      \
            const enumType _detail_vals[] = { __VA_ARGS__ };                                         \
            return ::EnumReflector( _detail_vals, sizeof(_detail_vals)/sizeof(enumType),             \
                    #enumName, Z_ENUM_DETAIL_STR((__VA_ARGS__))  );                             \
        }() );                                                                                  \
        return _reflector;                                                                      \
    }

// EnumReflector

inline EnumReflector::EnumReflector(EnumReflector&& rhs)
    : _data(rhs._data)
{
    rhs._data = nullptr;
}

template<typename EnumType>
inline const EnumReflector & EnumReflector::For(EnumType val)
{
    return _detail_reflector_(val);
}

static bool IsIdentChar(char c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           (c == '_');
}

template<typename EnumType>
EnumReflector::EnumReflector(const EnumType* vals, size_t count, const char* name, const char* body)
    : _data(new Private)
{
    _data->enumName = name;
    _data->values.resize(count);
    enum states
    {
        state_start, // Before identifier
        state_ident, // In identifier
        state_skip, // Looking for separator comma
    } state = state_start;
    assert(*body == '(');
    ++body;
    const char* ident_start = nullptr;
    size_t value_index = 0;
    int level = 0;
    for (;;)
    {
        assert(*body);
        switch (state)
        {
        case state_start:
            if (IsIdentChar(*body))
            {
                state = state_ident;
                ident_start = body;
            }
            ++body;
            break;
        case state_ident:
            if (!IsIdentChar(*body))
            {
                state = state_skip;
                assert(value_index < count);
                _data->values[value_index].name = std::string(ident_start, body - ident_start);
                _data->values[value_index].value = vals[value_index];
                ++value_index;
            }
            else
            {
                ++body;
            }
            break;
        case state_skip:
            if (*body == '(')
            {
                ++level;
            }
            else if (*body == ')')
            {
                if (level == 0)
                {
                    assert(value_index == count);
                    return;
                }
                --level;
            }
            else if (level == 0 && *body == ',')
            {
                state = state_start;
            }
            ++body;
        }
    }
}

inline EnumReflector::Enumerator EnumReflector::At(int index) const
{
    return Enumerator(*this, index);
}

inline EnumReflector::Enumerator EnumReflector::operator[](int index) const
{
    return At(index);
}

inline EnumReflector::Enumerator EnumReflector::begin() const
{
    return At(0);
}

inline EnumReflector::Enumerator EnumReflector::end() const
{
    return At(Count());
}

// EnumReflector::Enumerator

inline EnumReflector::Enumerator::Enumerator(const EnumReflector& er, int index)
    : _er(er)
    , _index(index) 
{
}

inline int EnumReflector::Enumerator::Index() const
{
    return _index;
}

inline const EnumReflector& EnumReflector::Enumerator::Reflector() const
{
    return _er;
}

inline EnumReflector::Enumerator::operator bool() const
{
    return IsValid();
}

inline bool EnumReflector::Enumerator::IsValid() const
{
    return _index < _er.Count();
}

inline bool EnumReflector::Enumerator::operator!=(const Enumerator& rhs) const
{
    return _index != rhs._index;
}

inline EnumReflector::Enumerator& EnumReflector::Enumerator::operator++()
{
    ++_index;
    return *this;
}

inline const EnumReflector::Enumerator& EnumReflector::Enumerator::operator*() const
{
    return *this;
}

#endif//GUARD
