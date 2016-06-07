//
//  SQLTable.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef SQLTable_hpp
#define SQLTable_hpp

#include <stdio.h>
#include <vector>
#include "SQLAttribute.hpp"

using namespace std;
class SQLTable {
public:
    vector<SQLAttribute*> attributes;
    string* tableName;
    int length;
    SQLTable(string* tableName):length(0),tableName(tableName)
    {
    }
    SQLTable(SQLTable& table)
    {
        for(vector<SQLAttribute*>::iterator itr = table.attributes.begin(); itr!=table.attributes.end() ; itr++)
            attributes.push_back(new SQLAttribute(**itr));
        tableName = new string(*table.tableName);
        length = table.length;
    }
    void appendAttribute(SQLAttribute* attribute)
    {
        attributes.push_back(attribute);
        length++;
    }
    void changeProperty(string* attributeName,int property)
    {
        for(vector<SQLAttribute*>::iterator it = attributes.begin() ; it != attributes.end() ; it++)
            if( *((*it)->attributeName) == *attributeName)
            {
                (*it)->attributeProperty ^= property;
                break;
            }
        delete attributeName;
    }
    ~SQLTable()
    {
        for(vector<SQLAttribute*>::iterator itr = attributes.begin(); itr!=attributes.end() ; itr++)
            delete (*itr);
        delete tableName;
    }
};

#endif /* SQLTable_hpp */
