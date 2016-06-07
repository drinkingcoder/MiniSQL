//
//  SQLAttribute.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef SQLAttribute_hpp
#define SQLAttribute_hpp

#include <stdio.h>
#include "SQLType.hpp"
#include <string>

using namespace std;

class SQLAttribute
{
public:
    string* attributeName;
    SQLDataType attributeType;
    int attributeProperty;
    int attributeSize;
    SQLAttribute(string* attributeName,SQLDataType attributeType,int attributeProperty,int attributeSize):attributeName(attributeName),attributeType(attributeType),attributeProperty(attributeProperty),attributeSize(attributeSize)
    {}
    SQLAttribute(SQLAttribute& attribute)
    {
        attributeName = new string(*attribute.attributeName);
        attributeType = attribute.attributeType;
        attributeProperty = attribute.attributeProperty;
        attributeSize = attribute.attributeSize;
    }
    ~SQLAttribute()
    {
        delete attributeName;
    }
};

#endif /* SQLAttribute_hpp */
