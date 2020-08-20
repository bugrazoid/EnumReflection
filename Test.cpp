#include "EnumReflection.h"
#include "EnumInfo.h"

#include <iostream>
using std::cout;
using std::endl;



//Z_ENUM_NS( CardSuit, int,
//    Spades,
//    Hearts,
//    Diamonds,
//    Clubs
//)

class SomeClass
{
public:
    static const int Constant = 100;
//    Z_ENUM( TasteFlags, int,
//        None      = 0,
//        Salted    = 1 << 0,
//        Sour      = 1 << 1,
//        Sweet     = 1 << 2,
//        SourSweet = (Sour | Sweet),
//        Other     = Constant,
//        Last
//    )
};

namespace SomeNamespace
{
//    Z_ENUM_NS( Ports, int,
//        HTTP  = 80,
//        HTTPS = 443,
//        SSH   = 22
//    )

}

namespace AnotherNamespace
{
//    static int GetPort(const std::string& name)
//    {
//        const auto& port = EnumReflector::For<::SomeNamespace::Ports>().Find(name);
//        return port.IsValid() ? port.Value() : 0;
//    }
}

namespace ClassEnum
{

//Z_ENUM_NS(Fruit, int8_t,
//          Apple = 0,
//          Orange = 100
//)

}


class CL1
{
public:
    ENUM_DECLARE(ABC, int,
                 A = 10, B, C)
};

ENUM_DECLARE_NS(EEE, int,
    Value = 0,
    Other = 10
);

namespace NS1
{
    ENUM_DECLARE_NS(NNN, int,
                    A = 0,
                    B,
                    C)
}


int main()
{
//    {
//        auto& reflector = EnumReflector::For<SomeClass::TasteFlags>();

//        cout << "Enum " << reflector.EnumName() << endl;
//        for (auto& val : reflector)
//        {
//            cout << "Value " << val.Name() << " = " << val.Value() << endl;
//        }
//        cout << endl;
//    }

//    using AnotherNamespace::GetPort;

//    cout << "HTTPS port: " << GetPort("HTTPS") << endl;
//    cout << endl;

//    constexpr bool b = std::is_enum<SomeNamespace::Ports>::value;

//    {
//        auto& reflector = EnumReflector::For<ClassEnum::Fruit>();
//        std::cout << std::boolalpha;
//        cout << "Enum " << reflector.EnumName()
//             << " with underlying type is int8_t: "
//             << std::is_same_v<int8_t, std::underlying_type_t<ClassEnum::Fruit>>
//             << endl;
//        for (auto& val : reflector)
//        {
//            cout << "Value " << val.Name() << " = " << val.Value() << endl;
//        }
//        cout << endl;
//    }

    EnumInfo<CL1::ABC> abc;
    EnumInfo<CL1::ABC> abc1;
    EnumInfo<CL1::ABC> abc2;

    const EEE aa[] = {Value, Other};
    const auto ba = getParsedData(CL1::ABC());
    std::cout << ba.enumName << std::endl;
    for (const auto& b: ba.nameByVal)
    {
        std::cout << b.first << " " << b.second << std::endl;
    }
    std::cout << std::endl;
}
