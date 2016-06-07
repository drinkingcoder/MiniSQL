//
//  API.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/27/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef API_hpp
#define API_hpp

#include <stdio.h>
#include <vector>
#include "SQLCondition.hpp" 
#include "interpreter.hpp"
#include "catalogManager.hpp"
//#include "indexManager.hpp"
#include "bufferManager.hpp"
//#include "recordManager.hpp"
#include "SQLDelegates.hpp"
#include <string>

using namespace std;

class API:public SQLCommandInterpreterDelegate//,public recordDelegate,public indexDelegate
{
private:
    catalogManager* catalog;
    string getInputString();
  /*  void solveSelectFrom(SQLCommandSelectFrom* command);
    void solveInsertInto(SQLCommandInsertInto* command);
    void solveDeleteFrom(SQLCommandDeleteFrom* command);
    void printTuple(SQLTuple* tuple);
    
    //basic delegates*/
    virtual SQLTable* getTableInfo(string* tableName);
    virtual SQLIndex* getIndexInfo(string* indexName);
    virtual string* getRecordFileName(string* tableName);
    virtual string* getIndexFileName(string* indexName);/*
    //record delegates
    virtual int selectedTupleFromRecord(vector<SQLTuple*> tuple);
    virtual int deleteIndex(string* indexName,SQLData* data);
    virtual int insertIndex(string* indexName,SQLData* data,int recordAddress);
    //index delegates
    virtual int selectedRecordAddressFromIndex(string* tableName,vector<int>& recordAddress,vector<SQLCondition*>& condition);
    virtual int deleteRecordAddressFromIndex(string* tableName,vector<int>& recordAddress,vector<SQLCondition*>& condition);*/
public:
    void start();
};

#endif /* API_hpp */
