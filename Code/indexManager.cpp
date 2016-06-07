//
//  indexManager.cpp
//  MiniSQL-final
//
//  Created by drinking on 10/29/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#include "indexManager.hpp"

void printAddress(vector<int>& d)
{
    for( int i=0; i<d.size();i++)
        cout << d[i] << " " ;
    cout << endl;
}

indexDelegate* indexManager::m_delegate;
int indexManager::deleteIndex(string* indexName,SQLData* data)
{
    int res=0;
    string* indexFileName = m_delegate->getIndexFileName(new string(*indexName));
    bufferManager* buffer = new bufferManager(indexFileName);
    SQLIndex* index = m_delegate->getIndexInfo(new string(*indexName));
    SQLTable* table = m_delegate->getTableInfo(new string(*index->tableName));
    for(vector<SQLAttribute*>::iterator itr = table->attributes.begin(); itr!=table->attributes.end();itr++)
    {
        if( *(*itr)->attributeName == *index->attributeName )
        {
            bPlusTree* tree = new bPlusTree(buffer,(*itr)->attributeType,(*itr)->attributeSize);
            res = tree->deleteData(data);
            delete tree;
        }
    }
    delete indexName;
    delete index;
    delete table;
    return res;
}

int indexManager::insertIndex(string* indexName,SQLData* data,int recordAddress)
{
    int res = 0;
    string* indexFileName = m_delegate->getIndexFileName(new string(*indexName));
    bufferManager* buffer = new bufferManager(indexFileName);
    SQLIndex* index = m_delegate->getIndexInfo(new string(*indexName));
    SQLTable* table = m_delegate->getTableInfo(new string(*index->tableName));
    for(vector<SQLAttribute*>::iterator itr = table->attributes.begin(); itr!=table->attributes.end();itr++)
    {
        if( *(*itr)->attributeName == *index->attributeName )
        {
            bPlusTree* tree = new bPlusTree(buffer,(*itr)->attributeType,(*itr)->attributeSize);
            res = tree->insertData(data, recordAddress);
            delete tree;
        }
    }
    delete indexName;
    delete index;
    delete table;
    return res;
}

int indexManager::deleteIndexWithCondition(string* indexName,SQLCondition* condition,vector<SQLCondition*>& otherConditions)
{
    int res = 0;
    string* indexFileName = m_delegate->getIndexFileName(new string(*indexName));
    bufferManager* buffer = new bufferManager(indexFileName);
    SQLIndex* index = m_delegate->getIndexInfo(new string(*indexName));
    SQLTable* table = m_delegate->getTableInfo(new string(*index->tableName));
    for(vector<SQLAttribute*>::iterator itr = table->attributes.begin(); itr!=table->attributes.end();itr++)
    {
        if( *(*itr)->attributeName == *index->attributeName )
        {
            bPlusTree* tree = new bPlusTree(buffer,(*itr)->attributeType,(*itr)->attributeSize);
            vector<int> recordAddress;
            SQLData* data;
            switch (condition->data->type) {
                case INT:
                    data = new SQLDataInt(*condition->data);
                    break;
                case FLOAT:
                    data = new SQLDataFloat(*condition->data);
                    break;
                case CHAR:
                    data = new SQLDataChar(*condition->data);
                    break;
                default:
                    break;
            }
        //    res = m_delegate->deleteRecordAddressFromIndex(recordAddress,otherConditions);
            delete tree;
        }
    }
    delete indexName;
    delete index;
    delete table;
    return res;
}

int indexManager::selectIndexWithCondition(string* indexName,SQLCondition* condition,vector<SQLCondition*>& otherConditions)
{
    if( condition->op->compare("==") != 0 )
    {
        delete indexName;
        delete condition;
        return 0;
    }
    int res = 0;
    string* indexFileName = m_delegate->getIndexFileName(new string(*indexName));
    bufferManager* buffer = new bufferManager(indexFileName);
    SQLIndex* index = m_delegate->getIndexInfo(new string(*indexName));
    SQLTable* table = m_delegate->getTableInfo(new string(*index->tableName));
    for(vector<SQLAttribute*>::iterator itr = table->attributes.begin(); itr!=table->attributes.end();itr++)
    {
        if( *(*itr)->attributeName == *index->attributeName )
        {
            bPlusTree* tree = new bPlusTree(buffer,(*itr)->attributeType,(*itr)->attributeSize);
            vector<int> recordAddress;
            SQLData* data;
            switch (condition->data->type) {
                case INT:
                    data = new SQLDataInt(*condition->data);
                    break;
                case FLOAT:
                    data = new SQLDataFloat(*condition->data);
                    break;
                case CHAR:
                    data = new SQLDataChar(*condition->data);
                    break;
                default:
                    break;
            }
            if( *condition->op == string("==") )
            {
                cout << tree->findEqualTo(data);
                return 0;
            }
            if( *condition->op == string("<") )
            {
                tree->findLessThan(data,recordAddress);
                printAddress(recordAddress);
                return 0;
            }
            if( *condition->op == string(">") )
            {
                tree->findLargerThan(data,recordAddress);
                printAddress(recordAddress);
                return 0;
            }
           /* recordAddress.push_back(tree->findEqualTo(data));
            res = m_delegate->selectedRecordAddressFromIndex(recordAddress,otherConditions);*/
            delete tree;
        }
    }
    delete indexName;
    delete index;
    delete table;
    return res;
}