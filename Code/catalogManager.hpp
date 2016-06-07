//
//  catalogManager.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef catalogManager_hpp
#define catalogManager_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include "SQLTable.hpp"
#include "SQLIndex.hpp"
#include "bufferManager.hpp"

using namespace std;

class catalogManager
{
private:
    vector<SQLTable*> tables;
    vector<SQLIndex*> indexes; //用来存储表单和索引的信息
    
    bufferManager* buffer;  //catalog也将通过buffer来处理缓存和文件内容
    unsigned char* pointer; //辅助catalog处理的变量
    
    SQLTable* parseTable();
    SQLAttribute* parseAttribute();
    SQLIndex* parseIndex();
    //由于要从文件中读取相关的catalog信息，这三个函数分别是从缓存区指定位置（pointer）中解析出相关的数据结构SQLTable、SQLAttribute、SQLIndex来。
    
    void writeTable(SQLTable* table);
    void writeIndex(SQLIndex* index);
    void writeAttribute(SQLAttribute* attribute);
    //要将catalog的有效信息写入到缓冲区并最终写入磁盘中，这三个函数分别是将三种数据结构的内容以相应格式写入指定的缓冲区位置（pointer）中。
    
public:
    catalogManager(string* fileName);
    ~catalogManager();
    void createTable(SQLTable* table);
    void createIndex(SQLIndex* index);
    void dropTable(string* tableName);
    void dropIndex(string* indexName);
    //提供四个接口给API调用，分别完成函数名对应的逻辑功能
    
    string* getRecordFileName(string* tableName);
    string* getIndexFileName(string* indexName);
    SQLTable* getTableInfo(string* tableName);
    SQLIndex* getIndexInfo(string* indexName);
    //提供四个接口给API调用，使API能够获得catalog内的消息，并能够将消息分发给其他模块，完成来自其他模块的委托。
    
    void printTableInfo(string* tableName);
    void printIndexInfo(string* indexName);
    void printTables();
    void printIndexes(); //测试时使用的打印catalog内部消息的相关函数，不必了解
};


#endif /* catalogManager_hpp */
