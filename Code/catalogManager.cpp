//
//  catalogManager.cpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#include "catalogManager.hpp"

SQLAttribute* catalogManager::parseAttribute()
{
    string* attributeName = new string();
    SQLDataType type;
    int property,size;
    while(*pointer!='\0')
    {
        *attributeName += *pointer;
        pointer++;
    }
    pointer++;
    type = *(SQLDataType*)pointer;
    pointer+=sizeof(SQLDataType);
    property = *(int*)pointer;
    pointer+=sizeof(int);
    size=*(int*)pointer;
    pointer+=sizeof(int);
    return new SQLAttribute(attributeName,type,property,size);
}

void catalogManager::writeAttribute(SQLAttribute* attribute)
{
    for (int i=0; i<attribute->attributeName->length(); i++) {
        *(char*)pointer = (*(attribute->attributeName))[i];
        pointer++;
    }
    *(char*)pointer = '\0';
    pointer++;
    *(SQLDataType*)pointer = attribute->attributeType;
    pointer+=sizeof(SQLDataType);
    *(int*)pointer = attribute->attributeProperty;
    pointer+=sizeof(int);
    *(int*)pointer = attribute->attributeSize;
    pointer+=sizeof(int);
}

SQLTable* catalogManager::parseTable()
{
    string* tableName = new string();
    while(*pointer != '\0')
    {
        *tableName += *pointer;
        pointer++;
    }
    pointer++;
    SQLTable* table = new SQLTable(tableName);
    int attributeNumber = *((int*)pointer);
    pointer+=sizeof(int);
    for(int tmp=0 ;tmp<attributeNumber; tmp++)
        table->appendAttribute(parseAttribute());
    return table;
}

void catalogManager::writeTable(SQLTable* table)
{
    for (int i=0; i<table->tableName->length(); i++) {
        *(char*)pointer = (*(table->tableName))[i];
        pointer++;
    }
    *(char*)pointer = '\0';
    pointer++;
    *(int*)pointer = table->length;
    pointer+=sizeof(int);
    for(vector<SQLAttribute*>::iterator itr = table->attributes.begin() ; itr!= table->attributes.end() ; itr++)
        writeAttribute(*itr);
}

SQLIndex* catalogManager::parseIndex()
{
    string* indexName = new string();
    string* tableName = new string();
    string* attributeName = new string();
    while(*pointer != '\0')
    {
        *indexName += *pointer;
        pointer++;
    }
    pointer++;
    while(*pointer != '\0')
    {
        *tableName += *pointer;
        pointer++;
    }
    pointer++;
    while(*pointer != '\0')
    {
        *attributeName += *pointer;
        pointer++;
    }
    pointer++;
    return new SQLIndex(indexName,tableName,attributeName);
}

void catalogManager::writeIndex(SQLIndex* index)
{
    for (int i=0; i<index->indexName->length(); i++) {
        *(char*)pointer = (*(index->indexName))[i];
        pointer++;
    }
    *(char*)pointer = '\0';
    pointer++;
    for (int i=0; i<index->tableName->length(); i++) {
        *(char*)pointer = (*(index->tableName))[i];
        pointer++;
    }
    *(char*)pointer = '\0';
    pointer++;
    for (int i=0; i<index->attributeName->length(); i++) {
        *(char*)pointer = (*(index->attributeName))[i];
        pointer++;
    }
    *(char*)pointer = '\0';
    pointer++;
}

catalogManager::catalogManager(string* fileName)
{
    buffer = new bufferManager(fileName);
    if( buffer->pageNumber == 0 ) buffer->getFreshPage();
    pointer = (unsigned char*)(buffer->startUsePage(0));
    int tableNumber = *((int*)pointer);
    pointer+=sizeof(int);
    for(int i=0; i<tableNumber ; i++)
        tables.push_back(parseTable());
    int indexNumber = *(int*)pointer;
    pointer+=sizeof(int);
    for(int i=0; i<indexNumber ; i++)
        indexes.push_back(parseIndex());
    buffer->finishUsePage(0);
   // delete fileName;
}

catalogManager::~catalogManager()
{
    pointer = (unsigned char*)(buffer->startUsePage(0));
    *(int*)pointer = int(tables.size());
    pointer+=sizeof(int);
    for(vector<SQLTable*>::iterator itr = tables.begin(); itr!=tables.end() ; itr++)
    {
        writeTable(*itr);
        delete *itr;
    }
    *(int*)pointer = int(indexes.size());
    pointer+=sizeof(int);
    for (vector<SQLIndex*>::iterator itr = indexes.begin(); itr!=indexes.end() ; itr++)
    {
        writeIndex(*itr);
        delete *itr;
    }
    delete buffer;
}

void catalogManager::createTable(SQLTable* table)
{
    tables.push_back(table);
    for(vector<SQLAttribute*>::iterator itr = table->attributes.begin(); itr != table->attributes.end() ; itr++)
        if(((*itr)->attributeProperty & PRIMARY) != 0)
        {
            string* indexName = new string(*(*itr)->attributeName + string("+index"));
            string* tableName = new string(*(table->tableName));
            string* attributeName = new string(*((*itr)->attributeName));
            SQLIndex* index = new SQLIndex(indexName,tableName,attributeName);
            createIndex(index);
        }
}

void catalogManager::dropTable(string* tableName)
{
    for(vector<SQLTable*>::iterator itr = tables.begin(); itr!=tables.end() ; )
        if( *((*itr)->tableName) == *tableName)
        {
            for(vector<SQLIndex*>::iterator itr = indexes.begin(); itr != indexes.end() ; )
                if( *(*itr)->tableName == * tableName) dropIndex(new string(*(*itr)->indexName));
                    else itr++;
            delete *itr;
            tables.erase(itr);
            break;
        } else itr++;
    delete tableName;
}

void catalogManager::createIndex(SQLIndex* index)
{
    indexes.push_back(index);
    for(vector<SQLTable*>::iterator itr = tables.begin(); itr!=tables.end() ; itr++ )
        if( *((*itr)->tableName) == *(index->tableName))
        {
            (*itr)->changeProperty(new string(*index->attributeName), INDEX);
            break;
        }
}

void catalogManager::dropIndex(string* indexName)
{
    SQLIndex* index = NULL;
    for(vector<SQLIndex*>::iterator itr = indexes.begin(); itr!=indexes.end() ; itr++)
        if(*((*itr)->indexName) == (*indexName))
        {
            index = *itr;
            indexes.erase(itr);
            break;
        }
    if( index == NULL ) return;
    for(vector<SQLTable*>::iterator itr = tables.begin(); itr!=tables.end() ; itr++ )
        if( *((*itr)->tableName) == *(index->tableName))
        {
            (*itr)->changeProperty(new string(*index->attributeName), INDEX);
            break;
        }
    delete index;
}

SQLTable* catalogManager::getTableInfo(string* tableName)
{
    SQLTable* res = NULL;
    for(vector<SQLTable*>::iterator itr = tables.begin(); itr!=tables.end() ; itr++ )
        if( *((*itr)->tableName) == *tableName)
        {
            res = new SQLTable(**itr);
            break;
        }
    if( res == NULL ) cout << "getTableInfo == NULL" << endl;
    return res;
}

SQLIndex* catalogManager::getIndexInfo(string* indexName)
{
    SQLIndex* res = NULL;
    for(vector<SQLIndex*>::iterator itr = indexes.begin(); itr!=indexes.end() ; itr++ )
        if( *((*itr)->indexName) == *indexName)
        {
            res = new SQLIndex(**itr);
            break;
        }
    if( res == NULL ) cout << "getIndexInfo == NULL" << endl;
    return res;
}

void catalogManager::printTableInfo(string* tableName)
{
    for(vector<SQLTable*>::iterator itr = tables.begin(); itr!=tables.end() ; itr++ )
        if( *((*itr)->tableName) == *tableName)
        {
            SQLTable* table = *itr;
            cout << "tableName = " << *tableName << endl;
            for(vector<SQLAttribute*>::iterator itr = table->attributes.begin() ; itr != table->attributes.end() ; itr++)
            {
                cout << "\t" << *(*itr)->attributeName;
                switch ((*itr)->attributeType) {
                    case INT:
                        cout << "\tINT";
                        break;
                    case FLOAT:
                        cout << "\tFLOAT";
                        break;
                    case CHAR:
                        cout << "\tCHAR( " << (*itr)->attributeSize << " )";
                        break;
                    default:
                        break;
                }
                if( ((*itr)->attributeProperty & PRIMARY) != 0 ) cout <<"\tPRIMARY";
                if( ((*itr)->attributeProperty & INDEX) != 0 ) cout <<"\tINDEX";
                cout << endl;
            }
        }
    delete tableName;
}

void catalogManager::printTables()
{
    for(int i=0; i<tables.size() ; i++)
        printTableInfo(new string(*tables[i]->tableName));
}

void catalogManager::printIndexes()
{
    for(int i=0; i<indexes.size() ; i++)
        printIndexInfo(new string(*indexes[i]->indexName));
}

void catalogManager::printIndexInfo(string* indexName)
{
    for(vector<SQLIndex*>::iterator itr = indexes.begin(); itr!=indexes.end() ; itr++ )
        if( *((*itr)->indexName) == *indexName)
        {
            cout << "indexName = " << *indexName << "-" << *(*itr)->tableName << " on( ";
            cout << *(*itr)->attributeName << " )" << endl;
        }
    delete indexName;
}

string* catalogManager::getRecordFileName(string* tableName)
{
    *tableName+= string(".table");
    return tableName;
}

string* catalogManager::getIndexFileName(string* indexName)
{
    *indexName+= string(".index");
    return indexName;
}