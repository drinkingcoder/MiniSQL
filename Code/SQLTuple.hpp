//
//  SQLTuple.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef SQLTuple_hpp
#define SQLTuple_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "SQLType.hpp"

using namespace std;

class SQLData {
public:
    SQLDataType type;
    int intValue;
    float floatValue;
    string* charValue;
    SQLData(int intValue):intValue(intValue),charValue(NULL) {type = INT;}
    SQLData(float floatValue):floatValue(floatValue),charValue(NULL) { type = FLOAT;}
    SQLData(string* charValue):charValue(charValue) { type = CHAR; }
    SQLData(SQLData& d) {
        type = d.type;
        intValue = d.intValue ;
        floatValue = d.floatValue ;
        if(d.charValue == NULL) charValue = NULL;
        else charValue = new string(*(d.charValue));
    }
    virtual ~SQLData(){}
    virtual int operator>(SQLData& rhs)=0;
    virtual int operator<(SQLData& rhs)=0;
    virtual int operator>=(SQLData& rhs)=0;
    virtual int operator<=(SQLData& rhs)=0;
    virtual int operator==(SQLData& rhs)=0;
    virtual int operator!=(SQLData& rhs)=0;
};

class SQLDataInt:public SQLData {
public:
    SQLDataInt(int intValue):SQLData(intValue){}
    SQLDataInt(SQLData& d):SQLData(d) {}
    ~SQLDataInt(){}
    int operator>(SQLData& rhs)
    {
        return intValue>rhs.intValue;
    }
    int operator<(SQLData& rhs)
    {
        return intValue<rhs.intValue;
    }
    int operator>=(SQLData& rhs)
    {
        return intValue>=rhs.intValue;
    }
    int operator<=(SQLData& rhs)
    {
        return intValue<=rhs.intValue;
    }
    int operator==(SQLData& rhs)
    {
        return intValue==rhs.intValue;
    }
    int operator!=(SQLData& rhs)
    {
        return intValue!=rhs.intValue;
    }
};

class SQLDataFloat:public SQLData {
public:
    SQLDataFloat(float floatValue):SQLData(floatValue){}
    SQLDataFloat(SQLData& d):SQLData(d) {}
    ~SQLDataFloat(){}
    int operator>(SQLData& rhs)
    {
        return floatValue>rhs.floatValue;
    }
    int operator<(SQLData& rhs)
    {
        return floatValue<rhs.floatValue;
    }
    int operator>=(SQLData& rhs)
    {
        return floatValue>=rhs.floatValue;
    }
    int operator<=(SQLData& rhs)
    {
        return floatValue<=rhs.floatValue;
    }
    int operator==(SQLData& rhs)
    {
        return floatValue==rhs.floatValue;
    }
    int operator!=(SQLData& rhs)
    {
        return floatValue!=rhs.floatValue;
    }
};

class SQLDataChar:public SQLData {
public:
    SQLDataChar(string* charValue):SQLData(charValue){}
    SQLDataChar(SQLData& d):SQLData(d) {}
    ~SQLDataChar(){ if(charValue!=NULL)delete charValue;}
    int operator>(SQLData& rhs)
    {
        return *charValue>*(rhs.charValue);
    }
    int operator<(SQLData& rhs)
    {
        return *charValue<*(rhs.charValue);
    }
    int operator>=(SQLData& rhs)
    {
        return *charValue>=*(rhs.charValue);
    }
    int operator<=(SQLData& rhs)
    {
        return *charValue<=*(rhs.charValue);
    }
    int operator==(SQLData& rhs)
    {
        return *charValue==*(rhs.charValue);
    }
    int operator!=(SQLData& rhs)
    {
        return *charValue!=*(rhs.charValue);
    }
};

class SQLTuple
{
public:
    vector<SQLData*> data;
    int length;
    SQLTuple()
    {
        length = 0;
    }
    SQLTuple(SQLTuple& tuple)
    {
        for( int i=0; i<tuple.data.size();i++)
            switch (tuple.data[i]->type ) {
                case INT:
                    data.push_back(new SQLDataInt(*tuple.data[i]));
                    break;
                case FLOAT:
                    data.push_back(new SQLDataFloat(*tuple.data[i]));
                    break;
                case CHAR:
                    data.push_back(new SQLDataChar(*tuple.data[i]));
                    break;
                default:
                    break;
            }
        length = tuple.length;
    }
    void appendValue(void* value,SQLDataType type)
    {
        SQLData* data;
        switch (type) {
            case INT:
                data = new SQLDataInt(*(int*)value);
                delete (int*)value;
                break;
            case FLOAT:
                data = new SQLDataFloat(*(float*)value);
                delete (float*)value;
                break;
            case CHAR:
                data = new SQLDataChar((string*)value);
                break;
            default:
                break;
        }
        length++;
        
    }
    ~SQLTuple()
    {
        vector<SQLData*>::iterator itr;
        itr = data.begin();
        for(;itr!=data.end();itr++)
            delete *itr;
    }
};

#endif /* SQLTuple_hpp */
