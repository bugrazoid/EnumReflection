// (c) 2016 Yakov Litvitskiy <thedsi100@gmail.com>
// See LICENSE.txt
#include "EnumReflection.h"


EnumReflector::~EnumReflector()
{
    if(_data)
        delete _data;
}

int EnumReflector::Count() const
{
    return (int)_data->values.size();
}

EnumReflector::Enumerator EnumReflector::Find(const std::string& name) const
{
    for (int i = 0; i < (int)_data->values.size(); ++i)
    {
        if (_data->values[i].name == name)
            return At(i);
    }
    return end();
}

EnumReflector::Enumerator EnumReflector::Find(int value) const
{
    for (int i = 0; i < (int)_data->values.size(); ++i)
    {
        if (_data->values[i].value == value)
            return At(i);
    }
    return end();
}

const std::string& EnumReflector::EnumName() const
{
    return _data->enumName;
}

const std::string& EnumReflector::Enumerator::Name() const
{
    return _er._data->values[_index].name;
}

int EnumReflector::Enumerator::Value() const
{
    return _er._data->values[_index].value;
}
