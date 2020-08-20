#ifndef ENUMINFO_H
#define ENUMINFO_H

#include <string>
#include <array>
#include <map>
#include <unordered_map>
#include <type_traits>

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
struct ParsedData
{
    constexpr ParsedData(const char* enumName, const Enum* vals, size_t valsCount, const char* rawNames, size_t rawNamesSize)
        : enumName(enumName)
    {
        parseNames(vals, valsCount, rawNames, rawNamesSize);
    }

    std::string_view enumName;
    std::map<Enum, std::string_view> nameByVal;
    std::unordered_map<std::string_view, Enum> valByName;

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

//template<typename Enum>
//static auto& getParsedData()
//{
//    constexpr const char* const enumName = "EEE";
//    constexpr const Adaptor<Enum> Value = 0, Other = 10;
//    constexpr const Enum vals[] = {Value = 0, Other = 10};
//    constexpr const size_t valsCount = sizeof(vals)/sizeof(Enum);
//    constexpr const char* const rawNames = "Value = 0, Other = 10";
//    constexpr const size_t rawNamesSize = sizeof ("Value = 0, Other = 10") - 1;
//    static const ParsedData<Enum, valsCount> parsedData(enumName, vals, rawNames, rawNamesSize);
//    return parsedData;
//}

template<typename Enum>
class EnumInfo
{
public:

private:
    const static ParsedData<Enum> _parsedData;
};

template<typename Enum>
const ParsedData<Enum> EnumInfo<Enum>::_parsedData = getParsedData(Enum());


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
#define ENUM_INFO_DETAIL_MAKE(spec, enumName, enumType, ...) \
    enum enumName:enumType                                                                           \
    {                                                                                           \
        __VA_ARGS__                                                                             \
    };                                                                                          \
    ENUM_INFO_DETAIL_SPEC_##spec \
    const ParsedData<enumName>& getParsedData(enumName = enumName())    \
    {   \
        constexpr const char* const enumNameStr = ENUM_INFO_DETAIL_STR(enumName);   \
        const Adaptor<enumName> __VA_ARGS__;    \
        const enumName vals[] = { __VA_ARGS__ };  \
        constexpr size_t valsCount = sizeof(vals)/sizeof(enumName);   \
        constexpr const char* const rawNames = ENUM_INFO_DETAIL_STR((__VA_ARGS__)); \
        constexpr const size_t rawNamesSize = sizeof (ENUM_INFO_DETAIL_STR((__VA_ARGS__))) - 1; \
        static const ParsedData<enumName> parsedData(enumNameStr, vals, valsCount, rawNames, rawNamesSize);    \
        return parsedData;  \
    }

#endif // ENUMINFO_H
