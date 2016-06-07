//
//  API.cpp
//  MiniSQL-final
//
//  Created by drinking on 10/27/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#include "API.hpp"
#include<iostream>

using namespace std;

//basic delegates
SQLTable* API::getTableInfo(string* tableName)
{
    return catalog->getTableInfo(tableName);
}
SQLIndex* API::getIndexInfo(string* indexName)
{
    return catalog->getIndexInfo(indexName);
}
string* API::getRecordFileName(string* tableName)
{
    return catalog->getRecordFileName(tableName);
}
string* API::getIndexFileName(string* indexName)
{
    return catalog->getIndexFileName(indexName);
}
/*
//record delegates
int API::selectedTupleFromRecord(vector<SQLTuple*> tuple)
{
    for(vector<SQLTuple*>::iterator itr = tuple.begin(); itr!=tuple.end() ;)
    {
        printTuple(*itr);
        delete *itr;
        tuple.erase(itr);
    }
    return 1;
}

int API::deleteIndex(string* indexName,SQLData* data)
{
    indexManager::deleteIndex(indexName, data);
    return 1;
}

int API::insertIndex(string* indexName,SQLData* data,int recordAddress)
{
    indexManager::insertIndex(indexName, data, recordAddress);
    return 1;
}

//index delegates
int API::selectedRecordAddressFromIndex(string* tableName,vector<int>& recordAddress,vector<SQLCondition*>& condition)
{
    recordManager::selectTuplesFromAddress(tableName,recordAddress, condition);
    return 1;
}

int API::deleteRecordAddressFromIndex(string* tableName,vector<int>& recordAddress,vector<SQLCondition*>& condition)
{
    recordManager::deleteTuplesFromAddress(tableName, recordAddress, condition);
    return 1;
}
void API::printTuple(SQLTuple* tuple)
{
    for (vector<SQLData*>::iterator itr = tuple->data.begin(); itr!= tuple->data.end() ; itr++) {
        switch ((*itr)->type) {
            case INT:
                cout << (*itr)->intValue << endl;
                break;
            case FLOAT:
                cout << (*itr)->floatValue << endl;
                break;
            case CHAR:
                cout << *(*itr)->charValue << endl;
                break;
            default:
                break;
        }
    }
    delete tuple;
}
*//*
void API::solveSelectFrom(SQLCommandSelectFrom* command)
{
    string* tableName = new string(*command->tableName);
    SQLTable* table = catalog->getTableInfo(new string(*tableName));
    string* attributeName = new string(*command->attributeName);
    for( vector<SQLAttribute*>::iterator itr = table->attributes.begin(); itr!=table->attributes.end();itr++)
    {
        if(*(*itr)->attributeName == *attributeName)
        {
            if (((*itr)->attributeProperty & (INDEX|PRIMARY)) != 0) {
                SQLCondition* condition = new SQLCondition(*command->condition);
                vector<SQLCondition*> otherConditions;
                indexManager::selectIndexWithCondition(new string(*tableName),condition,otherConditions);
                delete command;
            } else
            {
                vector<SQLCondition*> condition;
                condition.push_back(new SQLCondition(*command->condition));
                recordManager::selectTuples(new string(*tableName),condition);
            }
            break;
        }
    }
    delete table;
    delete attributeName;
    delete tableName;
}

void API::solveInsertInto(SQLCommandInsertInto* command)
{
    string* tableName = new string(*command->tableName);
    SQLTuple* tuple = new SQLTuple(*command->tuple);
    recordManager::insertTuple(tableName, tuple);
    delete command;
}*/
/*
void API::solveDeleteFrom(SQLCommandDeleteFrom* command)
{
    string* tableName = new string(*command->tableName);
    vector<SQLCondition*> condition;
    condition.push_back(new SQLCondition(*command->condition));
    recordManager::deleteTuple(tableName, condition);
    delete command;
}
*/
string API::getInputString()
{
    string cmd;
    char chrcmd[100];
    int index;
    index = 0;
    cin.get(chrcmd[index]);
    while (chrcmd[index]!=';') {
    index++;
    cin.get(chrcmd[index]);
    }
    index++;
    chrcmd[index] = '\0';

    cmd = string(chrcmd);
    cout << chrcmd << endl;
    cout << cmd << endl;
    return cmd;
}

void API::start()
{
    SQLCommand* command;
    bufferManager::initialMemory();
    catalog = new catalogManager(new string("/Users/drinkingcoder/Documents/university/Database Design/MiniSQL-final/lib/catalog"));
    SQLCommandInterpreter::setDelegate(this);
    catalog->printTables();
    catalog->printIndexes();
  //  indexManager::setDelegate(this);
   // recordManager::setDelegate(this);
    while(1)
    {
        command = SQLCommandInterpreter::transferStringToCommand(getInputString());
        if(command == NULL)
            cout << endl << "Failed!" << endl;
        else {
            switch(command->commandType)
            {
                case QUIT:
                    delete catalog;
                    bufferManager::quitBufferManager();
                    delete command;
                    return;
                case CREATE_TABLE:
                    catalog->createTable(new SQLTable(*((SQLCommandCreateTable*)command)->table));
                    delete command;
                    break;
                case DROP_TABLE:
                    catalog->dropTable(new string(*((SQLCommandDropTable*)command)->tableName));
                    delete command;
                    break;
                case CREATE_INDEX:
                    catalog->createIndex(new SQLIndex(*((SQLCommandCreateIndex*)command)->index));
            //        recordManager::createIndex(new SQLIndex(*((SQLCommandCreateIndex*)command)->index));
                    delete command;
                    break;
                case DROP_INDEX:
                    catalog->dropIndex(new string(*((SQLCommandDropIndex*)command)->indexName));
                    delete command;
                    break;
           /*     case SELECT_FROM:
                    solveSelectFrom((SQLCommandSelectFrom*)command);
                    break;
                case INSERT_INTO:
                    solveInsertInto((SQLCommandInsertInto*)command);*/
                    //break;
           /*     case DELETE_FROM:
                    solveDeleteFrom((SQLCommandDeleteFrom*)command);
                    break;*/
                default:
                    break;
            }
            catalog->printTables();
            catalog->printIndexes();
        }
    }
    delete catalog;
    bufferManager::quitBufferManager();
}