#ifndef ENUMINFO_H
#define ENUMINFO_H

#include <string>
#include <array>
#include <map>
#include <unordered_map>
#include <type_traits>
#include <optional>
#include <stdexcept>

template<typename Enum>
struct Adaptor
{
    Enum val;
    constexpr operator Enum() const {return val;}

    Adaptor() : val(static_cast<Enum>(++_val)) {}
    template<typename U>
    constexpr Adaptor(U i) : val(static_cast<Enum>(i)) {_val = i;}

    template<typename U>
    constexpr const Adaptor operator=(const U) const {return *this;}

private:
    static std::underlying_type_t<Enum> _val;
};

template<typename Enum>
std::underlying_type_t<Enum> Adaptor<Enum>::_val = 0;

constexpr bool isIdentChar(char c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           (c == '_');
}

template<typename Enum>
class EnumInfo;

template<typename Enum>
struct ParsedData
{
    constexpr ParsedData(const char* enumName, const Enum* vals, size_t valsCount, const char* rawNames, size_t rawNamesSize)
        : enumName(enumName)
        , values(vals)
        , size(valsCount)
    {
        parseNames(vals, valsCount, rawNames, rawNamesSize);
    }

    using iterator = typename std::map<Enum, std::string_view>::const_iterator;
    friend EnumInfo<Enum>;

private:
    std::string_view enumName;
    std::multimap<Enum, std::string_view> nameByVal;
    std::unordered_map<std::string_view, Enum> valByName;
    const Enum* values;
    const size_t size;

    constexpr void parseNames(const Enum* vals, size_t valsCount, const char* rawNames, size_t rawNamesSize)
    {
        enum states
        {
            state_start, // Before identifier
            state_ident, // In identifier
            state_skip,  // Looking for separator comma
        } state = state_start;

        const char* begin = nullptr;
        const char* end = rawNames;
        size_t value_index = 0;
        int level = 0;
        while (end != rawNames + rawNamesSize)
        {
            assert(*end);
            switch (state)
            {
            case state_start:
                if (IsIdentChar(*end))
                {
                    state = state_ident;
                    begin = end;
                }
                ++end;
                break;
            case state_ident:
                if (!IsIdentChar(*end))
                {
                    state = state_skip;
                    assert(value_index < valsCount);
                    const auto value = vals[value_index];
                    const std::string_view string{begin, size_t(end - begin)};
                    nameByVal.emplace(value, string);
                    valByName.emplace(string, value);
                    ++value_index;
                }
                else
                {
                    ++end;
                }
                break;
            case state_skip:
                if (*end == '(')
                {
                    ++level;
                }
                else if (*end == ')')
                {
                    if (level == 0)
                    {
                        assert(value_index == valsCount);
                        return;
                    }
                    --level;
                }
                else if (level == 0 && *end == ',')
                {
                    state = state_start;
                }
                ++end;
            }
        }
    }
};

// Declare an enumeration inside a class
#define ENUM_DECLARE( enumName, enumType, ... )\
                              ENUM_INFO_DETAIL_MAKE( class, enumName, enumType, __VA_ARGS__ )

// Declare an enumeration inside a namespace
#define ENUM_DECLARE_NS( enumName, enumType, ... )\
                              ENUM_INFO_DETAIL_MAKE( namespace, enumName, enumType, __VA_ARGS__ )

#define ENUM_INFO_DETAIL_SPEC_namespace \
    extern "C"{/* Protection from being used inside a class body */} \
    inline
#define ENUM_INFO_DETAIL_SPEC_class friend
#define ENUM_INFO_DETAIL_STR(x) #x
#define ENUM_INFO_DETAIL_MAKE(spec, enumName, enumType, ...)\
    enum enumName:enumType                              \
    {                                                       \
        __VA_ARGS__                                             \
    };                                                              \
    ENUM_INFO_DETAIL_SPEC_##spec                                        \
    const ParsedData<enumName>& getParsedData(enumName = enumName())        \
    {                                                                           \
        constexpr const char* const enumNameStr = ENUM_INFO_DETAIL_STR(enumName);   \
        const Adaptor<enumName> __VA_ARGS__;                                            \
        const enumName vals[] = { __VA_ARGS__ };                                            \
        constexpr size_t valsCount = sizeof(vals)/sizeof(enumName);                             \
        constexpr const char* const rawNames = ENUM_INFO_DETAIL_STR((__VA_ARGS__));                 \
        constexpr const size_t rawNamesSize = sizeof (ENUM_INFO_DETAIL_STR((__VA_ARGS__))) - 1;         \
        static const ParsedData<enumName> parsedData(enumNameStr, vals, valsCount, rawNames, rawNamesSize); \
        return parsedData;                                                                              \
    }

template<typename Enum>
class EnumInfo
{
public:
    using EnumType = std::underlying_type_t<Enum>;
    // TODO: сделать возможность через параметры шаблона задавать используемую строку.
    static std::string_view      name();
    static size_t                size();
    static std::optional<std::string_view> valueName(Enum value);
    static std::optional<std::string_view> valueName(size_t index);
    static std::optional<Enum>             value(std::string_view name);
    static std::optional<Enum>             value(size_t index);
    static std::optional<size_t>           index(Enum value);
    static std::optional<size_t>           index(std::string_view name);

    struct iterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type  = iterator;
        using difference_type = std::ptrdiff_t;

        iterator() = delete;
        explicit iterator(typename ParsedData<Enum>::iterator it);

        size_t index();
        Enum value();
        std::string_view name();

        value_type      operator*();
        iterator&       operator++();
        iterator        operator++(difference_type);
        iterator&       operator--();
        iterator        operator--(difference_type);
        bool            operator==(iterator);
        bool            operator!=(iterator);

    private:
        typename ParsedData<Enum>::iterator _it;
        size_t _index;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;

    static iterator begin();
    static iterator end();

    static reverse_iterator rbegin();
    static reverse_iterator rend();

private:
    const static ParsedData<Enum> _parsedData;

    template<typename Ret, typename Key, typename Cont>
    static std::optional<Ret> find(Key key, Cont cont);
    static iterator fromIndex(size_t index);
    template<typename Key, typename Cont>
    static std::optional<EnumType> findIndex(Key key, Cont cont);
};

template<typename Enum>
const ParsedData<Enum> EnumInfo<Enum>::_parsedData = getParsedData(Enum());

#endif // ENUMINFO_H

// ---- EnumInfo implementation ----

template<typename Enum>
std::string_view EnumInfo<Enum>::name()
{
    return _parsedData.enumName;
}

template<typename Enum>
std::optional<std::string_view> EnumInfo<Enum>::valueName(Enum value)
{
    return EnumInfo::find<std::string_view>(value, _parsedData.nameByVal);
}

template<typename Enum>
std::optional<std::string_view> EnumInfo<Enum>::valueName(size_t index)
{
    const iterator it = fromIndex(index);
    return it.name();
}

template<typename Enum>
std::optional<Enum> EnumInfo<Enum>::value(std::string_view name)
{
    return EnumInfo::find<Enum>(name, _parsedData.valByName);
}

template<typename Enum>
std::optional<Enum> EnumInfo<Enum>::value(size_t index)
{
    const iterator it = fromIndex(index);
    return it.value();
}

template<typename Enum>
std::optional<size_t> EnumInfo<Enum>::index(Enum value)
{
    return findIndex(value, _parsedData.nameByVal);
}

template<typename Enum>
std::optional<size_t> EnumInfo<Enum>::index(std::string_view name)
{
    return findIndex(name, _parsedData.valByName);
}

template<typename Enum>
size_t EnumInfo<Enum>::size()
{
    return _parsedData.size;
}

template<typename Enum>
typename EnumInfo<Enum>::iterator EnumInfo<Enum>::begin()
{
    return iterator(_parsedData.nameByVal.begin());
}

template<typename Enum>
typename EnumInfo<Enum>::iterator EnumInfo<Enum>::end()
{
    return iterator(_parsedData.nameByVal.end());
}

template<typename Enum>
typename EnumInfo<Enum>::reverse_iterator EnumInfo<Enum>::rbegin()
{
    return iterator(_parsedData.nameByVal.begin());
}

template<typename Enum>
typename EnumInfo<Enum>::reverse_iterator EnumInfo<Enum>::rend()
{
    return iterator(_parsedData.nameByVal.end());
}

template<typename Enum>
typename EnumInfo<Enum>::iterator EnumInfo<Enum>::fromIndex(size_t index)
{
    iterator it(_parsedData.nameByVal.begin());
    for (size_t i = 0; i != index; ++i)
    {
        ++it;
    }
    return it;
}

template<typename Enum>
template<typename Ret, typename Key, typename Cont>
std::optional<Ret> EnumInfo<Enum>::find(Key key, Cont cont)
{
    const auto it = cont.find(key);
    if (it != cont.end())
    {
        return it->second;
    }

    return std::nullopt;
}

template<typename Enum>
template<typename Key, typename Cont>
std::optional<typename EnumInfo<Enum>::EnumType> EnumInfo<Enum>::findIndex(Key key, Cont cont)
{
    const auto it = cont.find(key);
        if (it != cont.end())
        {
            size_t i = 0;
            auto in = cont.begin();
            while (in != cont.end())
            {
                if (in == it)
                    return i;
                ++in;
                ++i;
            }
        }

        return std::nullopt;
}

// ---- EnumInfo::iterator implementation ----

template<typename Enum>
EnumInfo<Enum>::iterator::iterator(typename ParsedData<Enum>::iterator it)
    : _it(it)
    , _index(0)
{
    auto current = EnumInfo<Enum>::_parsedData.nameByVal.begin();
    while (current != _it)
    {
        if (_index == EnumInfo<Enum>::size())
            std::out_of_range("Something gonna wrong!");
        ++current;
        ++_index;
    }
}

template<typename Enum>
size_t EnumInfo<Enum>::iterator::index()
{
    return _index;
}

template<typename Enum>
Enum EnumInfo<Enum>::iterator::value()
{
    return _it->first;
}

template<typename Enum>
std::string_view EnumInfo<Enum>::iterator::name()
{
    return  _it->second;
}

template<typename Enum>
typename EnumInfo<Enum>::iterator::value_type EnumInfo<Enum>::iterator::operator*()
{
    return *this;
}

/// Prefix operator++
template<typename Enum>
typename EnumInfo<Enum>::iterator& EnumInfo<Enum>::iterator::operator++()
{
    ++_it;
    ++_index;
    return *this;
}

/// Postfix operator++
template<typename Enum>
typename EnumInfo<Enum>::iterator EnumInfo<Enum>::iterator::operator++(EnumInfo::iterator::difference_type)
{
    EnumInfo<Enum>::iterator tmp(*this);
    operator++();
    return tmp;
}

/// Prefix operator--
template<typename Enum>
typename EnumInfo<Enum>::iterator& EnumInfo<Enum>::iterator::operator--()
{
    --_it;
    --_index;
    return *this;
}

/// Postfix operator--
template<typename Enum>
typename EnumInfo<Enum>::iterator EnumInfo<Enum>::iterator::operator--(EnumInfo::iterator::difference_type)
{
    EnumInfo<Enum>::iterator tmp(*this);
    operator--();
    return tmp;
}

template<typename Enum>
bool EnumInfo<Enum>::iterator::operator==(EnumInfo::iterator other)
{
    return _it == other._it;
}

template<typename Enum>
bool EnumInfo<Enum>::iterator::operator!=(EnumInfo::iterator other)
{
    return _it != other._it;
}
