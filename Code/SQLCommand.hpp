//
//  SQLCommand.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef SQLCommand_hpp
#define SQLCommand_hpp

#include <stdio.h>
#include <stdlib.h>
#include "SQLType.hpp"
#include "SQLCondition.hpp"
#include "SQLTable.hpp"
#include "SQLTuple.hpp"
#include "SQLIndex.hpp"

using namespace std;


class SQLCommand{
public:
    SQLCommandType commandType;
    virtual ~SQLCommand(){}
};

class SQLCommandCreateTable:public SQLCommand
{
public:
    SQLTable* table;
    void init(SQLTable* table)
    {
        this->table = table;
    }
    ~SQLCommandCreateTable()
    {
        delete table;
    }
};

class SQLCommandDropTable:public SQLCommand
{
public:
    string* tableName;
    void init(string* tableName)
    {
        this->tableName = tableName;
    }
    ~SQLCommandDropTable()
    {
        delete tableName;
    }
};

class SQLCommandCreateIndex:public SQLCommand
{
public:
    SQLIndex* index;
    void init(SQLIndex* index)
    {
        this->index = index;
    }
    ~SQLCommandCreateIndex()
    {
        delete index;
    }
};

class SQLCommandDropIndex:public SQLCommand
{
public:
    string* indexName;
    void init(string* indexName)
    {
        this->indexName = indexName;
    }
    ~SQLCommandDropIndex()
    {
        delete indexName;
    }
};

class SQLCommandSelectFrom:public SQLCommand
{
public:
    string* attributeName;
    string* tableName;
    SQLCondition* condition;
    void init(string* attributeName,string* tableName,SQLCondition* condition)
    {
        this->attributeName = attributeName;
        this->tableName = tableName;
        this->condition = condition;
    }
    ~SQLCommandSelectFrom()
    {
        delete attributeName;
        delete tableName;
        delete condition;
    }
};

class SQLCommandInsertInto:public SQLCommand
{
public:
    string* tableName;
    SQLTuple* tuple;
    void init(string* tableName,SQLTuple* tuple)
    {
        this->tableName = tableName;
        this->tuple = tuple;
    }
    ~SQLCommandInsertInto()
    {
        delete tableName;
        delete tuple;						//Note!    delete list<void*> means we delete void* default?
    }
};

class SQLCommandDeleteFrom:public SQLCommand
{
public:
    string* tableName;
    SQLCondition* condition;
    void init(string* tableName,SQLCondition* condition)
    {
        this->tableName = tableName;
        this->condition = condition;
    }
    ~SQLCommandDeleteFrom()
    {
        delete tableName;
        delete condition;
    }
};

class SQLCommandQuit:public SQLCommand
{
};

class SQLCommandExec:public SQLCommand
{
    string* fileName;
    void init(string* fileName)
    {
        this->fileName = fileName;
    }
    ~SQLCommandExec()
    {
        delete fileName;
    }
};

#endif /* SQLCommand_hpp */
