#ifndef ENUMINFO_H
#define ENUMINFO_H

#include <string>
#include <array>
#include <map>
#include <unordered_map>

template<typename Enum>
struct Adaptor
{
    Enum val;
    constexpr operator Enum() const {return val;}

    template<typename U>
    constexpr Adaptor(U i) : val(static_cast<Enum>(i)) {}

    template<typename U>
    constexpr const Adaptor operator=(const U) const {return *this;}
};

constexpr bool isIdentChar(char c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           (c == '_');
}

template<typename Enum, size_t Size>
struct ParsedData
{
    constexpr ParsedData(const char* enumName, const Enum* vals, const char* rawNames, size_t rawNamesSize)
        : enumName(enumName)
    {
        parseNames(vals, rawNames, rawNamesSize);
    }

    std::string_view enumName;
    std::map<Enum, std::string_view> nameByVal;
    std::unordered_map<std::string_view, Enum> valByName;

    constexpr void parseNames(const Enum* vals, const char* rawNames, size_t rawNamesSize)
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
                    assert(value_index < Size);
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
                        assert(value_index == Size);
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

template<typename Enum>
static auto& getParsedData()
{
    constexpr const char* const enumName = "EEE";
    constexpr const Adaptor<Enum> Value = 0, Other = 10;
    constexpr const Enum vals[] = {Value = 0, Other = 10};
    constexpr const size_t valsCount = sizeof(vals)/sizeof(Enum);
    constexpr const char* const rawNames = "Value = 0, Other = 10";
    constexpr const size_t rawNamesSize = sizeof ("Value = 0, Other = 10") - 1;
    static const ParsedData<Enum, valsCount> parsedData(enumName, vals, rawNames, rawNamesSize);
    return parsedData;
}

template<typename Enum>
class EnumInfo
{
public:

};

#endif // ENUMINFO_H
