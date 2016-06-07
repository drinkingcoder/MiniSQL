//
//  SQLIndex.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef SQLIndex_hpp
#define SQLIndex_hpp

#include <stdio.h>
#include "SQLType.hpp"
#include <string>

using namespace std;

class SQLIndex {
public:
    string* indexName;
    string* tableName;
    string* attributeName;
    SQLIndex(string* indexName,string* tableName,string* attributeName):indexName(indexName),tableName(tableName),attributeName(attributeName)
    {}
    SQLIndex(SQLIndex& index)
    {
        indexName = new string(*index.indexName);
        tableName = new string(*index.tableName);
        attributeName = new string(*index.attributeName);
    }
    ~SQLIndex()
    {
        delete indexName;
        delete tableName;
        delete attributeName;
    }
};

#endif /* SQLIndex_hpp */
