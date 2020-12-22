/**
 * @author Trubnikov Sergey <bugrazoid@gmail.com>
 */
#pragma once

#include <string>
#include <array>
#include <map>
#include <unordered_map>
#include <type_traits>
#include <optional>
#include <stdexcept>
#include <assert.h>


template<typename Enum, typename String>
class EnumInfo;

namespace _enum_info_private
{

template<typename Enum>
struct Adaptor
{
    Enum val;
    constexpr operator Enum() const {return val;}

    constexpr Adaptor() : val(static_cast<Enum>([]
    {
        _val = _val.has_value() ? *_val + 1 : 0;
        return *_val;
    }())) {}
    template<typename U>
    constexpr Adaptor(U i) : val(static_cast<Enum>(i)) {_val = i;}

    template<typename U>
    constexpr const Adaptor operator=(const U) const {return *this;}

private:
    static std::optional<std::underlying_type_t<Enum>> _val;
};

template<typename Enum>
std::optional<std::underlying_type_t<Enum>> Adaptor<Enum>::_val = std::nullopt;

/**
 * @author Yakov Litvitskiy <thedsi100@gmail.com> (c) 2016
 * <a href="https://github.com/thedsi/EnumReflection/blob/master/LICENSE.txt">LICENSE.txt</a>
 */
constexpr bool isIdentChar(char c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           (c == '_');
}

template<typename Enum>
struct RawData
{
    constexpr RawData(const char* enumName, size_t enumNameSize,
                      const Enum* vals, size_t valsCount,
                      const char* rawNames, size_t rawNamesSize)
        : _enumName(enumName), _enumNameSize(enumNameSize)
        , _rawNames(rawNames), _rawNamesSize(rawNamesSize)
        , _vals(vals)        , _valsCount(valsCount)
    {}

    const char* _enumName;
    const size_t _enumNameSize;
    const char* _rawNames;
    const size_t _rawNamesSize;
    const Enum* _vals;
    const size_t _valsCount;
};

template<typename Enum, typename String>
struct ParsedData
{
    constexpr ParsedData(const char* enumName, size_t enumNameSize, const Enum* vals, size_t valsCount, const char* rawNames, size_t rawNamesSize)
        : enumName(enumName, enumNameSize)
        , values(vals)
        , size(valsCount)
    {
        names.reserve(size);
        parseNames(vals, valsCount, rawNames, rawNamesSize);
    }
    constexpr ParsedData(RawData<Enum> rawData)
        : ParsedData(rawData._enumName, rawData._enumNameSize,
                     rawData._vals, rawData._valsCount,
                     rawData._rawNames, rawData._rawNamesSize)
    {}

    using iterator = typename std::map<Enum, String>::const_iterator;
    friend EnumInfo<Enum, String>;

private:
    String enumName;
    std::vector<String> names;
    std::multimap<Enum, String> nameByVal;
    std::unordered_map<String, Enum> valByName;
    const Enum* values;
    const size_t size;

    /**
     * @author Yakov Litvitskiy <thedsi100@gmail.com> (c) 2016
     * <a href="https://github.com/thedsi/EnumReflection/blob/master/LICENSE.txt">LICENSE.txt</a>
     */
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
                if (isIdentChar(*end))
                {
                    state = state_ident;
                    begin = end;
                }
                ++end;
                break;
            case state_ident:
                if (!isIdentChar(*end))
                {
                    state = state_skip;
                    assert(value_index < valsCount);
                    const auto value = vals[value_index];
                    const String string(begin, size_t(end - begin));
                    names.push_back(string);
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

} // namespace _enum_info_private

/**
 * @brief Declare an enumeration inside a class
 * @author Yakov Litvitskiy <thedsi100@gmail.com> (c) 2016
 * <a href="https://github.com/thedsi/EnumReflection/blob/master/LICENSE.txt">LICENSE.txt</a>
 */
#define ENUM_DECLARE( enumName, enumType, ... )\
                              ENUM_INFO_DETAIL_MAKE( class, enumName, enumType, __VA_ARGS__ )

/**
 * @brief Declare an enumeration inside a namespace
 * @author Yakov Litvitskiy <thedsi100@gmail.com> (c) 2016
 * <a href="https://github.com/thedsi/EnumReflection/blob/master/LICENSE.txt">LICENSE.txt</a>
 */
#define ENUM_DECLARE_NS( enumName, enumType, ... )\
                              ENUM_INFO_DETAIL_MAKE( namespace, enumName, enumType, __VA_ARGS__ )

/**
 * @author Yakov Litvitskiy <thedsi100@gmail.com> (c) 2016
 * <a href="https://github.com/thedsi/EnumReflection/blob/master/LICENSE.txt">LICENSE.txt</a>
 */
#define ENUM_INFO_DETAIL_SPEC_namespace \
    extern "C"{/* Protection from being used inside a class body */} \
    inline

/**
 * @author Yakov Litvitskiy <thedsi100@gmail.com> (c) 2016
 * <a href="https://github.com/thedsi/EnumReflection/blob/master/LICENSE.txt">LICENSE.txt</a>
 */
#define ENUM_INFO_DETAIL_SPEC_class friend

/**
 * @author Yakov Litvitskiy <thedsi100@gmail.com> (c) 2016
 * <a href="https://github.com/thedsi/EnumReflection/blob/master/LICENSE.txt">LICENSE.txt</a>
 */
#define ENUM_INFO_DETAIL_STR(x) #x

/**
 * @author Yakov Litvitskiy <thedsi100@gmail.com> (c) 2016
 * <a href="https://github.com/thedsi/EnumReflection/blob/master/LICENSE.txt">LICENSE.txt</a>
 */
#define ENUM_INFO_DETAIL_MAKE(spec, enumName, enumType, ...)                                    \
    enum enumName:enumType                                                                      \
    {                                                                                           \
        __VA_ARGS__                                                                             \
    };                                                                                          \
    ENUM_INFO_DETAIL_SPEC_##spec                                                                \
    const _enum_info_private::RawData<enumName>& getRawData(enumName = enumName())              \
    {                                                                                           \
        constexpr const char* const enumNameStr = ENUM_INFO_DETAIL_STR(enumName);               \
        constexpr const size_t enumNameStrSize = sizeof(ENUM_INFO_DETAIL_STR(enumName)) - 1;    \
        static const _enum_info_private::Adaptor<enumName> __VA_ARGS__;                         \
        static const enumName vals[] = { __VA_ARGS__ };                                         \
        constexpr size_t valsCount = sizeof(vals)/sizeof(enumName);                             \
        constexpr const char* const rawNames = ENUM_INFO_DETAIL_STR((__VA_ARGS__));             \
        constexpr const size_t rawNamesSize = sizeof (ENUM_INFO_DETAIL_STR((__VA_ARGS__))) - 1; \
        static const _enum_info_private::RawData<enumName> rawData(                             \
            enumNameStr, enumNameStrSize, vals, valsCount, rawNames, rawNamesSize);             \
        return rawData;                                                                         \
    }

template<typename Enum, typename String = std::string_view>
class EnumInfo
{
public:
    using EnumType = std::underlying_type_t<Enum>;

    static String   name();
    static size_t   size();

    static std::optional<String>    valueName(Enum value);
    static std::optional<String>    valueName(size_t index);
    // TODO: valueNames & aliases
    static std::optional<Enum>      value(String name);
    static std::optional<Enum>      value(size_t index);
    static std::optional<size_t>    index(Enum value);
    static std::optional<size_t>    index(String name);

    struct iterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type  = iterator;
        using difference_type = std::ptrdiff_t;

        iterator() = delete;
        explicit iterator(typename _enum_info_private::ParsedData<Enum, String>::iterator it);

        size_t index();
        Enum value();
        String name() const;

        value_type      operator*();
        iterator&       operator++();
        iterator        operator++(difference_type);
        iterator&       operator--();
        iterator        operator--(difference_type);
        bool            operator==(iterator) const;
        bool            operator!=(iterator) const;

    private:
        typename _enum_info_private::ParsedData<Enum, String>::iterator _it;
        size_t _index;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;

    static iterator begin();
    static iterator end();

    static reverse_iterator rbegin();
    static reverse_iterator rend();

private:
    const static _enum_info_private::ParsedData<Enum, String> _parsedData;

    template<typename Ret, typename Key, typename Cont>
    static std::optional<Ret> find(Key key, Cont cont);
    static iterator fromIndex(size_t index);
};

template<typename Enum, typename String>
const _enum_info_private::ParsedData<Enum, String> EnumInfo<Enum, String>::_parsedData =
        _enum_info_private::ParsedData<Enum, String>(getRawData(Enum()));

// ---- EnumInfo implementation ----

template<typename Enum, typename String>
String EnumInfo<Enum, String>::name()
{
    return _parsedData.enumName;
}

template<typename Enum, typename String>
std::optional<String> EnumInfo<Enum, String>::valueName(Enum value)
{
    return EnumInfo::find<String>(value, _parsedData.nameByVal);
}

template<typename Enum, typename String>
std::optional<String> EnumInfo<Enum, String>::valueName(size_t index)
{
    if (index >= _parsedData.names.size())
        return std::nullopt;

    return _parsedData.names[index];
}

template<typename Enum, typename String>
std::optional<Enum> EnumInfo<Enum, String>::value(String name)
{
    return EnumInfo::find<Enum>(name, _parsedData.valByName);
}

template<typename Enum, typename String>
std::optional<Enum> EnumInfo<Enum, String>::value(size_t index)
{
    if (index >= _parsedData.size)
        return std::nullopt;
    return _parsedData.values[index];
}

template<typename Enum, typename String>
std::optional<size_t> EnumInfo<Enum, String>::index(Enum value)
{
    for (size_t i = 0; i < _parsedData.size; ++i)
    {
        if (_parsedData.values[i] == value)
            return i;
    }
    return std::nullopt;
}

template<typename Enum, typename String>
std::optional<size_t> EnumInfo<Enum, String>::index(String name)
{
    const auto it = std::find(_parsedData.names.begin(), _parsedData.names.end(), name);
    if (it == _parsedData.names.end())
        return std::nullopt;
    return std::distance(_parsedData.names.begin(), it);
}

template<typename Enum, typename String>
size_t EnumInfo<Enum, String>::size()
{
    return _parsedData.size;
}

template<typename Enum, typename String>
typename EnumInfo<Enum, String>::iterator EnumInfo<Enum, String>::begin()
{
    return iterator(_parsedData.nameByVal.begin());
}

template<typename Enum, typename String>
typename EnumInfo<Enum, String>::iterator EnumInfo<Enum, String>::end()
{
    return iterator(_parsedData.nameByVal.end());
}

template<typename Enum, typename String>
typename EnumInfo<Enum, String>::reverse_iterator EnumInfo<Enum, String>::rbegin()
{
    return iterator(_parsedData.nameByVal.begin());
}

template<typename Enum, typename String>
typename EnumInfo<Enum, String>::reverse_iterator EnumInfo<Enum, String>::rend()
{
    return iterator(_parsedData.nameByVal.end());
}

template<typename Enum, typename String>
typename EnumInfo<Enum, String>::iterator EnumInfo<Enum, String>::fromIndex(size_t index)
{
    iterator it(_parsedData.nameByVal.begin());
    for (size_t i = 0; i != index; ++i)
    {
        ++it;
    }
    return it;
}

template<typename Enum, typename String>
template<typename Ret, typename Key, typename Cont>
std::optional<Ret> EnumInfo<Enum, String>::find(Key key, Cont cont)
{
    const auto it = cont.find(key);
    if (it != cont.end())
    {
        return it->second;
    }

    return std::nullopt;
}

// ---- EnumInfo::iterator implementation ----

template<typename Enum, typename String>
EnumInfo<Enum, String>::iterator::iterator(typename _enum_info_private::ParsedData<Enum, String>::iterator it)
    : _it(it)
    , _index(0)
{
    auto current = EnumInfo<Enum, String>::_parsedData.nameByVal.begin();
    while (current != _it)
    {
        if (_index == EnumInfo<Enum, String>::size())
            std::out_of_range("Something gonna wrong!");
        ++current;
        ++_index;
    }
}

template<typename Enum, typename String>
size_t EnumInfo<Enum, String>::iterator::index()
{
    return _index;
}

template<typename Enum, typename String>
Enum EnumInfo<Enum, String>::iterator::value()
{
    return _it->first;
}

template<typename Enum, typename String>
String EnumInfo<Enum, String>::iterator::name() const
{
    return  _it->second;
}

template<typename Enum, typename String>
typename EnumInfo<Enum, String>::iterator::value_type EnumInfo<Enum, String>::iterator::operator*()
{
    return *this;
}

/// Prefix operator++
template<typename Enum, typename String>
typename EnumInfo<Enum, String>::iterator& EnumInfo<Enum, String>::iterator::operator++()
{
    ++_it;
    ++_index;
    return *this;
}

/// Postfix operator++
template<typename Enum, typename String>
typename EnumInfo<Enum, String>::iterator EnumInfo<Enum, String>::iterator::operator++(EnumInfo::iterator::difference_type)
{
    EnumInfo<Enum, String>::iterator tmp(*this);
    operator++();
    return tmp;
}

/// Prefix operator--
template<typename Enum, typename String>
typename EnumInfo<Enum, String>::iterator& EnumInfo<Enum, String>::iterator::operator--()
{
    --_it;
    --_index;
    return *this;
}

/// Postfix operator--
template<typename Enum, typename String>
typename EnumInfo<Enum, String>::iterator EnumInfo<Enum, String>::iterator::operator--(EnumInfo::iterator::difference_type)
{
    EnumInfo<Enum, String>::iterator tmp(*this);
    operator--();
    return tmp;
}

template<typename Enum, typename String>
bool EnumInfo<Enum, String>::iterator::operator==(EnumInfo::iterator other) const
{
    return _it == other._it;
}

template<typename Enum, typename String>
bool EnumInfo<Enum, String>::iterator::operator!=(EnumInfo::iterator other) const
{
    return _it != other._it;
}
