//
//  SQLCondition.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef SQLCondition_hpp
#define SQLCondition_hpp

#include <stdio.h>
#include <string>
#include "SQLType.hpp"
#include "SQLTuple.hpp"

using namespace std;

class SQLCondition
{
public:
    string* attributeName;
    string* op;
    SQLData* data;
    int isSatisfied(SQLData* data)
    {
        if(data->type != this->data->type) return 0;
        if( op->compare("==") ) return (*data == *this->data);
        if( op->compare("!=") ) return (*data != *this->data);
        if( op->compare(">=") ) return (*data >= *this->data);
        if( op->compare("<=") ) return (*data <= *this->data);
        if( op->compare(">") ) return (*data > *this->data);
        if( op->compare("<") ) return (*data < *this->data);
        return 0;
    }
    SQLCondition(string* attributeName,SQLDataType attributeType,string* op,void* value)
    {
        this->attributeName = attributeName;
        this->op = op;
        int intValue;
        float floatValue;
        string* charValue;
        switch(attributeType)
        {
            case INT:intValue = *((int*)value);data = new SQLDataInt(intValue); break;
            case FLOAT:floatValue = *((float*)value);data = new SQLDataFloat(floatValue);break;
            case CHAR:charValue = (string*)value;data = new SQLDataChar(charValue); break;
            default:break;
        }
    }
    ~SQLCondition()
    {
        delete attributeName;
        delete op;
        delete data;
    }
};


#endif /* SQLCondition_hpp */
