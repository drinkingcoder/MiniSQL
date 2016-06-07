//
//  SQLDelegates.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/29/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef SQLDelegates_hpp
#define SQLDelegates_hpp

#include <stdio.h>
#include "SQLTable.hpp"
#include "SQLTuple.hpp"
#include "SQLCondition.hpp"
#include "SQLIndex.hpp"

class basicDelegate {
public:
    virtual SQLTable* getTableInfo(string* tableName)=0;
    virtual SQLIndex* getIndexInfo(string* indexName)=0;
    virtual string* getRecordFileName(string* tableName)=0;
    virtual string* getIndexFileName(string* indexName)=0;
};

class recordDelegate:public basicDelegate {
public:
    virtual int selectedTupleFromRecord(vector<SQLTuple*> tuple)=0;
    virtual int deleteIndex(string* indexName,SQLData* data)=0;
    virtual int insertIndex(string* indexName,SQLData* data,int recordAddress)=0;
};

class indexDelegate:public basicDelegate {
public:
    virtual int selectedRecordAddressFromIndex(vector<int>& recordAddress,vector<SQLCondition*>& condition)=0;
    virtual int deleteRecordAddressFromIndex(vector<int>& recordAddress,vector<SQLCondition*>& condition)=0;
};

class SQLCommandInterpreterDelegate:public basicDelegate
{
};

#endif /* SQLDelegates_hpp */
