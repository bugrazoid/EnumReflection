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


    const auto printOkFail = [](bool isOk)
    {
        return isOk ? "[OK]" : "[FAIL]";
    };

    //--- Tests ---
    std::cout << "Start tests..." << std::endl;

    std::cout << "Enum name test..." << std::endl;
    const bool isTestNameOk = true
                              && testName<CardSuit>("CardSuit")
                              && testName<SomeClass::TasteFlags>("TasteFlags")
                              && testName<SomeNamespace::Ports>("Ports")
                              && testName<ns::Cs::Color>("Color");
    std::cout << "Enum name test is " << printOkFail(isTestNameOk) << std::endl;

    std::cout << "Enum size test..." << std::endl;
    const bool isTestSizeOk = true
                              && testSize<CardSuit>(4)
                              && testSize<SomeClass::TasteFlags>(7)
                              && testSize<SomeNamespace::Ports>(4)
                              && testSize<ns::Cs::Color>(4);
    std::cout << "Enum size test is " << printOkFail(isTestSizeOk) << std::endl;
}
