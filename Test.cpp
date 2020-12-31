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

    std::cout << "\"" << testName << "\" test ... ";
    const bool isOk = testFunc();
    std::cout << "\b\b\b\bdone with " << printOkFail(isOk) << std::endl;
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

template<typename Enum>
bool testValueByValueName(Enum value, std::string_view valueName)
{
    const auto opt = EnumInfo<Enum>::value(valueName);
    if (!opt.has_value())
    {
        std::cerr << "Value not found for value name\"" << valueName << "\"" << std::endl;
        return false;
    }
    const bool isSame = *opt == value;
    if (!isSame)
    {
        std::cerr << "Enum value \"" << static_cast<std::underlying_type_t<Enum>>(*opt)
                  << "\" not equal to \"" << static_cast<std::underlying_type_t<Enum>>(value) << "\"" << std::endl;
    }

    return isSame;
}

template<typename Enum>
bool testValueByIndex(size_t index, Enum value)
{
    const auto opt = EnumInfo<Enum>::value(index);
    if (!opt.has_value())
    {
        std::cerr << "Value not found for \"" << static_cast<std::underlying_type_t<Enum>>(value) << "\" and index == "
                  << index << std::endl;
        return false;
    }
    const bool isSame = *opt == value;
    if (!isSame)
    {
        std::cerr << "Enum value \"" << static_cast<std::underlying_type_t<Enum>>(*opt)
                  << "\" not equal to \"" << static_cast<std::underlying_type_t<Enum>>(value) << "\" for index == "
                  << index << std::endl;
    }

    return isSame;
}

template<typename Enum>
bool testIndexByValue(size_t index, Enum value)
{
    const auto valueName = EnumInfo<Enum>::valueName(value).value();
    const auto opt = EnumInfo<Enum>::index(value);
    if (!opt.has_value())
    {
        std::cerr << "Index not found for \"" <<  valueName << std::endl;
        return false;
    }
    const bool isSame = *opt == index;
    if (!isSame)
    {
        std::cerr << "Enum index \"" << static_cast<std::underlying_type_t<Enum>>(*opt)
                  << "\" not equal to \"" << index << "\" for value "
                  <<  valueName << std::endl;
    }

    return isSame;
}

template<typename Enum>
bool testIndexByValueName(size_t index, std::string_view valueName)
{
    const auto opt = EnumInfo<Enum>::index(valueName);
    if (!opt.has_value())
    {
        std::cerr << "Index not found for \"" << valueName << std::endl;
        return false;
    }
    const bool isSame = *opt == index;
    if (!isSame)
    {
        std::cerr << "Enum index \"" << *opt
                  << "\" not equal to \"" << index << "\" for "
                  << valueName << std::endl;
    }

    return isSame;
}

template<typename Enum>
bool testIterator(std::vector<std::pair<Enum, std::string>> etalon)
{
    if (EnumInfo<Enum>::size() != etalon.size())
    {
        std::cerr << "Size mismatch! Enum size: " << EnumInfo<Enum>::size()
                  << ", etalon size: " << etalon.size();
        return false;
    }

    bool isOK = true;
    size_t i = 0;
    for (auto it: EnumInfo<Enum>())
    {
        isOK = isOK
               && it.index() == i
               && it.value() == etalon[i].first
               && it.name()  == etalon[i].second;

        if (!isOK)
        {
            if (it.index() != i)
                std::cerr << "Index mismatch! Current index: " << i
                          << ", iterator index: " << it.index() << std::endl;
            if (it.value() != etalon[i].first)
                std::cerr << "Value mismatch! Etalon value: " << static_cast<std::underlying_type_t<Enum>>(etalon[i].first)
                          << ", iterator name: " << static_cast<std::underlying_type_t<Enum>>(it.value()) << std::endl;
            if (it.name() != etalon[i].second)
                            std::cerr << "Name mismatch! Etalon name: " << etalon[i].second
                                      << ", iterator name: " << it.name() << std::endl;
            return false;
        }
        ++i;
    }
    return isOK;
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

    test("Enum value name by index", []
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

    test("Enum value by value name", []
    {
        return true
                && testValueByValueName(CardSuit::Spades  , "Spades")
                && testValueByValueName(CardSuit::Hearts  , "Hearts")
                && testValueByValueName(CardSuit::Diamonds, "Diamonds")
                && testValueByValueName(CardSuit::Clubs   , "Clubs")
                && testValueByValueName(SomeClass::TasteFlags::None     , "None")
                && testValueByValueName(SomeClass::TasteFlags::Salted   , "Salted")
                && testValueByValueName(SomeClass::TasteFlags::Sour     , "Sour")
                && testValueByValueName(SomeClass::TasteFlags::Sweet    , "Sweet")
                && testValueByValueName(SomeClass::TasteFlags::SourSweet, "SourSweet")
                && testValueByValueName(SomeClass::TasteFlags::Other    , "Other")
                && testValueByValueName(SomeClass::TasteFlags::Last     , "Last")
                && testValueByValueName(SomeNamespace::Ports::HTTP       , "HTTP")
                && testValueByValueName(SomeNamespace::Ports::HTTPS      , "HTTPS")
                && testValueByValueName(SomeNamespace::Ports::SecureShell, "SecureShell")
                && testValueByValueName(SomeNamespace::Ports::SSH        , "SSH")
                && testValueByValueName(ns::Cs::Color::Transparent, "Transparent")
                && testValueByValueName(ns::Cs::Color::Red        , "Red")
                && testValueByValueName(ns::Cs::Color::Green      , "Green")
                && testValueByValueName(ns::Cs::Color::Blue       , "Blue")
                ;
    });

    test("Enum value by index", []
    {
        return true
                && testValueByIndex(0, CardSuit::Spades)
                && testValueByIndex(1, CardSuit::Hearts)
                && testValueByIndex(2, CardSuit::Diamonds)
                && testValueByIndex(3, CardSuit::Clubs)
                && testValueByIndex(0, SomeClass::TasteFlags::None)
                && testValueByIndex(1, SomeClass::TasteFlags::Salted)
                && testValueByIndex(2, SomeClass::TasteFlags::Sour)
                && testValueByIndex(3, SomeClass::TasteFlags::Sweet)
                && testValueByIndex(4, SomeClass::TasteFlags::SourSweet)
                && testValueByIndex(5, SomeClass::TasteFlags::Other)
                && testValueByIndex(6, SomeClass::TasteFlags::Last)
                && testValueByIndex(0, SomeNamespace::Ports::HTTP)
                && testValueByIndex(1, SomeNamespace::Ports::HTTPS)
                && testValueByIndex(2, SomeNamespace::Ports::SecureShell)
                && testValueByIndex(3, SomeNamespace::Ports::SSH)
                && testValueByIndex(0, ns::Cs::Color::Transparent)
                && testValueByIndex(1, ns::Cs::Color::Red)
                && testValueByIndex(2, ns::Cs::Color::Green)
                && testValueByIndex(3, ns::Cs::Color::Blue)
                ;
    });

    test("Enum index by value", []
    {
        return true
                && testIndexByValue(0, CardSuit::Spades)
                && testIndexByValue(1, CardSuit::Hearts)
                && testIndexByValue(2, CardSuit::Diamonds)
                && testIndexByValue(3, CardSuit::Clubs)
                && testIndexByValue(0, SomeClass::TasteFlags::None)
                && testIndexByValue(1, SomeClass::TasteFlags::Salted)
                && testIndexByValue(2, SomeClass::TasteFlags::Sour)
                && testIndexByValue(3, SomeClass::TasteFlags::Sweet)
                && testIndexByValue(4, SomeClass::TasteFlags::SourSweet)
                && testIndexByValue(5, SomeClass::TasteFlags::Other)
                && testIndexByValue(6, SomeClass::TasteFlags::Last)
                && testIndexByValue(0, SomeNamespace::Ports::HTTP)
                && testIndexByValue(1, SomeNamespace::Ports::HTTPS)
                && testIndexByValue(2, SomeNamespace::Ports::SecureShell)
                // Because "SecureShell" and "SSH" has same value, than result is first matched index
                && testIndexByValue(2, SomeNamespace::Ports::SSH)
                && testIndexByValue(0, ns::Cs::Color::Transparent)
                && testIndexByValue(1, ns::Cs::Color::Red)
                && testIndexByValue(2, ns::Cs::Color::Green)
                && testIndexByValue(3, ns::Cs::Color::Blue)
                ;
    });

    test("Enum index by value name", []
    {
        return true
                && testIndexByValueName<CardSuit>(0, "Spades")
                && testIndexByValueName<CardSuit>(1, "Hearts")
                && testIndexByValueName<CardSuit>(2, "Diamonds")
                && testIndexByValueName<CardSuit>(3, "Clubs")
                && testIndexByValueName<SomeClass::TasteFlags>(0, "None")
                && testIndexByValueName<SomeClass::TasteFlags>(1, "Salted")
                && testIndexByValueName<SomeClass::TasteFlags>(2, "Sour")
                && testIndexByValueName<SomeClass::TasteFlags>(3, "Sweet")
                && testIndexByValueName<SomeClass::TasteFlags>(4, "SourSweet")
                && testIndexByValueName<SomeClass::TasteFlags>(5, "Other")
                && testIndexByValueName<SomeClass::TasteFlags>(6, "Last")
                && testIndexByValueName<SomeNamespace::Ports>(0, "HTTP")
                && testIndexByValueName<SomeNamespace::Ports>(1, "HTTPS")
                && testIndexByValueName<SomeNamespace::Ports>(2, "SecureShell")
                && testIndexByValueName<SomeNamespace::Ports>(3, "SSH")
                && testIndexByValueName<ns::Cs::Color>(0, "Transparent")
                && testIndexByValueName<ns::Cs::Color>(1, "Red")
                && testIndexByValueName<ns::Cs::Color>(2, "Green")
                && testIndexByValueName<ns::Cs::Color>(3, "Blue")
                ;
    });

    test("Enum iterator", []
    {
        return true
                && testIterator<CardSuit>({
                                              {CardSuit::Spades  , "Spades"  },
                                              {CardSuit::Hearts  , "Hearts"  },
                                              {CardSuit::Diamonds, "Diamonds"},
                                              {CardSuit::Clubs   , "Clubs"   }
                                          })
                && testIterator<SomeClass::TasteFlags>({
                                                           {SomeClass::TasteFlags::None     , "None"     },
                                                           {SomeClass::TasteFlags::Salted   , "Salted"   },
                                                           {SomeClass::TasteFlags::Sour     , "Sour"     },
                                                           {SomeClass::TasteFlags::Sweet    , "Sweet"    },
                                                           {SomeClass::TasteFlags::SourSweet, "SourSweet"},
                                                           {SomeClass::TasteFlags::Other    , "Other"    },
                                                           {SomeClass::TasteFlags::Last     , "Last"     }
                                                       })
                && testIterator<SomeNamespace::Ports>({
                                                          {SomeNamespace::Ports::HTTP       , "HTTP"       },
                                                          {SomeNamespace::Ports::HTTPS      , "HTTPS"      },
                                                          {SomeNamespace::Ports::SecureShell, "SecureShell"},
                                                          {SomeNamespace::Ports::SSH        , "SSH"        }
                                                      })
                && testIterator<ns::Cs::Color>({

                                                   {ns::Cs::Color::Transparent, "Transparent"},
                                                   {ns::Cs::Color::Red        , "Red"        },
                                                   {ns::Cs::Color::Green      , "Green"      },
                                                   {ns::Cs::Color::Blue       , "Blue"       }
                                               })
                ;
    });
}
