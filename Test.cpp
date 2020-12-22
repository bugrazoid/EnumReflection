#include "EnumInfo.h"

#include <iostream>
#include <limits>


ENUM_DECLARE_NS( CardSuit, int8_t,
    Spades,
    Hearts,
    Diamonds,
    Clubs
)

class SomeClass
{
public:
    static const int Constant = 100;
    ENUM_DECLARE( TasteFlags, uint64_t,
        None      = 0,
        Salted    = 1 << 0,
        Sour      = 1 << 1,
        Sweet     = 1 << 2,
        SourSweet = (Sour | Sweet),
        Other     = Constant,
        Last      = std::numeric_limits<uint64_t>::max()
    )
};

namespace SomeNamespace
{
    ENUM_DECLARE_NS( Ports, int,
        HTTP  = 80,
        HTTPS = 443,
        SecureShell = 22,
        SSH   = 22
    )

}

namespace AnotherNamespace
{
    static int GetPort(const std::string& name)
    {
        const auto& port = EnumInfo<::SomeNamespace::Ports>::value(name);
        return port.has_value() ? port.value() : 0;
    }
}

namespace ns
{
struct Cs
{
    ENUM_DECLARE(Color, int16_t,
                 Transparent = -1,
                 Red = 1,
                 Green,
                 Blue)
};
}

template<typename Enum>
void printEnum()
{
    std::cout << "Enum " << EnumInfo<Enum>::name()
              << " with underlying type " << typeid (typename EnumInfo<Enum>::EnumType).name() << std::endl;
    for (auto it: EnumInfo<Enum>())
    {
        std::cout
                << it.index() << ") "
                << std::to_string(it.value()) << " - "
                << it.name() << std::endl;
    }
    std::cout << "Enum size is " << EnumInfo<Enum>::size() << std::endl << std::endl;
}

template<typename F>
bool test(std::string_view testName, F testFunc)
{
    const auto printOkFail = [](bool isOk)
    {
        return isOk ? "[OK]" : "[FAIL]";
    };

    std::cout << "\"" << testName << "\" test ...";
    const bool isOk = testFunc();
    std::cout << "\b\b\bdone with " << printOkFail(isOk) << std::endl;
    return isOk;
}

template<typename Enum>
bool testName(const std::string_view name)
{
    const bool sameName = name == EnumInfo<Enum>::name();
    if (!sameName)
    {
        std::cerr << "Enum name \"" << EnumInfo<Enum>::name()
                  << "\" not equal to \"" << name << "\"" << std::endl;
    }

    return sameName;
}

template<typename Enum>
bool testSize(size_t size)
{
    const bool sameSize = size == EnumInfo<Enum>::size();
    if (!sameSize)
    {
        std::cerr << "Enum size \"" << EnumInfo<Enum>::name()
                  << "\" not equal to \"" << size << "\"" << std::endl;
    }

    return sameSize;
}

template<typename Enum>
bool testValueNameByValue(Enum value, std::string_view valueName)
{
    const auto name = EnumInfo<Enum>::valueName(value);
    if (!name.has_value())
    {
        std::cerr << "Value name not found for \"" << valueName << "\"" << std::endl;
        return false;
    }
    const bool isSame = *name == valueName;
    if (!isSame)
    {
        std::cerr << "Enum value name \"" << *name
                  << "\" not equal to \"" << valueName << "\"" << std::endl;
    }

    return isSame;
}

template<typename Enum>
bool testValueNameByIndex(size_t index, std::string_view valueName)
{
    const auto name = EnumInfo<Enum>::valueName(index);
    if (!name.has_value())
    {
        std::cerr << "Value name not found for \"" << valueName << "\" and index == "
                  << index << std::endl;
        return false;
    }
    const bool isSame = *name == valueName;
    if (!isSame)
    {
        std::cerr << "Enum value name \"" << *name
                  << "\" not equal to \"" << valueName << "\" for index == "
                  << index << std::endl;
    }

    return isSame;
}


int main()
{
    //--- Examples ---
    printEnum<SomeClass::TasteFlags>();

    std::cout << "Index for " << EnumInfo<SomeClass::TasteFlags>::valueName(SomeClass::TasteFlags::Sour).value_or("ERROR")
              << " is " << EnumInfo<SomeClass::TasteFlags>::index(SomeClass::TasteFlags::Sour).value_or(0) << std::endl;
    std::cout << std::endl;

    printEnum<SomeNamespace::Ports>();

    using AnotherNamespace::GetPort;

    std::cout << "HTTPS port: " << GetPort("HTTPS") << std::endl;
    std::cout << std::to_string(SomeNamespace::Ports::SSH) << " is port for "
              << EnumInfo<SomeNamespace::Ports>::valueName(SomeNamespace::Ports::SSH).value_or("ERROR") << std::endl;
    std::cout << std::endl;

    printEnum<CardSuit>();
    std::cout << std::endl;
    printEnum<ns::Cs::Color>();



    //--- Tests ---
    std::cout << "Start tests..." << std::endl;

    test("Enum name", []
    {
        return true
                && testName<CardSuit>("CardSuit")
                && testName<SomeClass::TasteFlags>("TasteFlags")
                && testName<SomeNamespace::Ports>("Ports")
                && testName<ns::Cs::Color>("Color");
    });

    test("Enum size", []
    {
        return true
                && testSize<CardSuit>(4)
                && testSize<SomeClass::TasteFlags>(7)
                && testSize<SomeNamespace::Ports>(4)
                && testSize<ns::Cs::Color>(4);
    });

    test("Enum value name by value", []
    {
        return true
                && testValueNameByValue(CardSuit::Spades  , "Spades")
                && testValueNameByValue(CardSuit::Hearts  , "Hearts")
                && testValueNameByValue(CardSuit::Diamonds, "Diamonds")
                && testValueNameByValue(CardSuit::Clubs   , "Clubs")
                && testValueNameByValue(SomeClass::TasteFlags::None     , "None")
                && testValueNameByValue(SomeClass::TasteFlags::Salted   , "Salted")
                && testValueNameByValue(SomeClass::TasteFlags::Sour     , "Sour")
                && testValueNameByValue(SomeClass::TasteFlags::Sweet    , "Sweet")
                && testValueNameByValue(SomeClass::TasteFlags::SourSweet, "SourSweet")
                && testValueNameByValue(SomeClass::TasteFlags::Other    , "Other")
                && testValueNameByValue(SomeClass::TasteFlags::Last     , "Last")
                && testValueNameByValue(SomeNamespace::Ports::HTTP       , "HTTP")
                && testValueNameByValue(SomeNamespace::Ports::HTTPS      , "HTTPS")
                && testValueNameByValue(SomeNamespace::Ports::SecureShell, "SecureShell")
                // Because "SecureShell" comes first and it is same as SSH here 'SSH' value returns "SecureShell" name
                && testValueNameByValue(SomeNamespace::Ports::SSH        , "SecureShell")
                && testValueNameByValue(ns::Cs::Color::Transparent, "Transparent")
                && testValueNameByValue(ns::Cs::Color::Red        , "Red")
                && testValueNameByValue(ns::Cs::Color::Green      , "Green")
                && testValueNameByValue(ns::Cs::Color::Blue       , "Blue")
                ;
    });

    test("Enum value by index", []
    {
        return true
                && testValueNameByIndex<CardSuit>(0, "Spades")
                && testValueNameByIndex<CardSuit>(1, "Hearts")
                && testValueNameByIndex<CardSuit>(2, "Diamonds")
                && testValueNameByIndex<CardSuit>(3, "Clubs")
                && testValueNameByIndex<SomeClass::TasteFlags>(0, "None")
                && testValueNameByIndex<SomeClass::TasteFlags>(1, "Salted")
                && testValueNameByIndex<SomeClass::TasteFlags>(2, "Sour")
                && testValueNameByIndex<SomeClass::TasteFlags>(3, "Sweet")
                && testValueNameByIndex<SomeClass::TasteFlags>(4, "SourSweet")
                && testValueNameByIndex<SomeClass::TasteFlags>(5, "Other")
                && testValueNameByIndex<SomeClass::TasteFlags>(6, "Last")
                && testValueNameByIndex<SomeNamespace::Ports>(0, "HTTP")
                && testValueNameByIndex<SomeNamespace::Ports>(1, "HTTPS")
                && testValueNameByIndex<SomeNamespace::Ports>(2, "SecureShell")
                && testValueNameByIndex<SomeNamespace::Ports>(3, "SSH")
                && testValueNameByIndex<ns::Cs::Color>(0, "Transparent")
                && testValueNameByIndex<ns::Cs::Color>(1, "Red")
                && testValueNameByIndex<ns::Cs::Color>(2, "Green")
                && testValueNameByIndex<ns::Cs::Color>(3, "Blue")
                ;
    });
}
