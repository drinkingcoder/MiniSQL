//
//  interpreter.cpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#include "interpreter.hpp"
#include <iostream>

using namespace std;

int SQLCommandInterpreter::index = 0;
int SQLCommandInterpreter::tmp = 0;
string SQLCommandInterpreter::inputString;
string SQLCommandInterpreter::command;
int SQLCommandInterpreter::intValue;
float SQLCommandInterpreter::floatValue;
int SQLCommandInterpreter::isString;
SQLCommandInterpreterDelegate* SQLCommandInterpreter::m_delegate;

int SQLCommandInterpreter::attributeNumber;
string* SQLCommandInterpreter::attributeName;
SQLDataType SQLCommandInterpreter::attributeType;
string* SQLCommandInterpreter::tableName;
string* SQLCommandInterpreter::indexName;
int SQLCommandInterpreter::attributeProperty;
int SQLCommandInterpreter::attributeSize;
string* SQLCommandInterpreter::op;
SQLCondition* SQLCommandInterpreter::condition;
SQLTuple* SQLCommandInterpreter::tuple;
SQLTable* SQLCommandInterpreter::table;
SQLAttribute* SQLCommandInterpreter::attribute;
SQLIndex* SQLCommandInterpreter::sqlindex;

void SQLCommandInterpreter::resetPointers()
{

    attributeName = NULL;
    tableName = NULL;
    indexName = NULL;
    op = NULL;
    condition = NULL;
    table = NULL;
    attribute = NULL;
}

SQLCommand* SQLCommandInterpreter::transferError(string error)
{
    cout << "Transfer Faild:" << endl;
    cout << '\t' << error << endl;
    if(attributeName!=NULL) delete attributeName;
    if(tableName!=NULL) delete tableName;
    if(indexName!=NULL) delete indexName;
    if(op!=NULL) delete op;
    if(condition!=NULL) delete condition;
    if(table!=NULL) delete table;
    if(attribute!=NULL) delete attribute;
    resetPointers();
    return NULL;
}


int SQLCommandInterpreter::transferStringToInt()
{
    cout << "transferStringToInt" << endl;
    intValue = 0;
    for (string::iterator itr = command.begin(); itr!=command.end(); itr++) {
        if(*itr < '0' || *itr > '9') return 0;
        intValue *= 10;
        intValue += *itr - '0';
    }
    return 1;
}

int SQLCommandInterpreter::transferStringToFloat()
{
    cout << "transferStringToFloat" << endl;
    floatValue = 0;
    int decimalFlag = 0;
    float times = 1;
    for (string::iterator itr = command.begin(); itr != command.end(); itr++) {
        if(*itr == '.')
        {
            if (decimalFlag == 1) return 0;
            decimalFlag = 1;
        } else if( *itr <= '9' && *itr >= '0')
        {
            if (decimalFlag) times *= 0.1;
            floatValue *= 10;
            floatValue += *itr - '0';
        } else return 0;
    }
    floatValue *= times;
    return 1;
}

SQLCondition* SQLCommandInterpreter::transferStringToCondition()
{
    cout << "transferStringToCondition" << endl;
    SQLCondition* condition;
    table = m_delegate->getTableInfo(tableName);
    if(table == NULL)
    {
        transferError("no table has this name");
        return NULL;
    }
    tableName = NULL;
    attributeName = new string(command);
    nextStep();
    op = new string(command);
    for(tmp = 0;tmp<table->length;tmp++)
        if(*(table->attributes[tmp]->attributeName) == *attributeName)
        {
            attributeType = table->attributes[tmp]->attributeType;
            break;
        }
    if(tmp == table->length)
    {
        transferError("attributeName not exist in such table");
        return NULL;
    }
    nextStep();
    switch(attributeType)
    {
        case INT:
            if(transferStringToInt()==0) { transferError("value is not int"); return NULL;}
            condition = new SQLCondition(attributeName,attributeType,op,new int(intValue));
            break;
        case FLOAT:
            if(transferStringToFloat()==0) { transferError("value is not float"); return NULL;}
            condition = new SQLCondition(attributeName,attributeType,op,new float(intValue));
            break;
        case CHAR:
            if(!isString) { transferError("value is not float"); return NULL;}
            condition = new SQLCondition(attributeName,attributeType,op,new string(command));
            break;
        default:
            transferError("peculiar value type");
            return NULL;
            
    }
    return condition;
}

int isSeperator(char c)
{
    if( c == ' ' ) return 1;
    if( c == ',' ) return 1;
    if( c == ';' ) return 1;
    if( c == '\n') return 1;
    if( c == '\0') return 1;
    return 0;
}

void SQLCommandInterpreter::nextStep()
{
    cout << "SQLCommandInterpreter::nextStep" << endl;
    int charFlag = 0;
    isString = 0;
    while(isSeperator(inputString[index]))
    {
        //if(inputString[index] == '\0') break;
        index++;
    }
    if( inputString[index] == '(' )
    {
        index++;
        command = "(";
    } else if( inputString[index] == ')' )
    {
        index++;
        command = ")";
    } else {
        tmp = index;
        while(!isSeperator(inputString[tmp])||charFlag)
        {
            if(inputString[tmp] == '\0') break;
            if(inputString[tmp] == '(') break;
            if(inputString[tmp] == ')') break;
            if(inputString[tmp] == '\'')
            {
                charFlag = !charFlag;
                if(charFlag) index++;
                isString = 1;
            }
            tmp++;
        }
        command = inputString.substr(index,tmp-index-isString);
        index = tmp;
    }
    cout << "\tgetCommandString = " << command << endl;
}

SQLCommand* SQLCommandInterpreter::SQLSelectCommandTransfer()
{
    cout << "SQLCommandInterpreter::SQLSelectCommandTransfer" << endl;
    SQLCommandSelectFrom* res = new SQLCommandSelectFrom();
    res->commandType = SELECT_FROM;
    nextStep();
    if(command.compare("*")==0)
    {
        attributeName = new string("*");
    } else return transferError("select attribute isn\'t \'*\'");
    nextStep();
    if(command.compare("from")!=0) return transferError("select \'from\' lost");
    nextStep();
    tableName = new string(command);
    nextStep();
    if(command.compare("where")!=0) return transferError("select \'where\' lost");
    nextStep();
    condition = transferStringToCondition();
    if(condition == NULL) return NULL;
    res->init(attributeName,tableName,condition);
    resetPointers();
    return res;
}

SQLCommand* SQLCommandInterpreter::SQLDeleteCommandTransfer()
{
    cout << "SQLCommandInterpreter::SQLDeleteCommandTransfer" << endl;
    SQLCommandDeleteFrom* res = new SQLCommandDeleteFrom();
    res->commandType = DELETE_FROM;
    nextStep();
    if(command.compare("from")!=0) return transferError("delete \'from\' lost");
    nextStep();
    tableName = new string(command);
    nextStep();
    if(command.compare("where")!=0) return transferError("delete \'where\' lost");
    nextStep();
    condition = transferStringToCondition();
    if(condition == NULL) return NULL;
    res->init(tableName,condition);
    resetPointers();
    return res;
}

SQLCommand* SQLCommandInterpreter::SQLCreateTableCommandTransfer()
{
    cout << "SQLCommandInterpreter::SQLCreateTableCommandTransfer" << endl;
    SQLCommandCreateTable* res = new SQLCommandCreateTable();
    res->commandType = CREATE_TABLE;
    nextStep();
    tableName = new string(command);
    table = new SQLTable(tableName);
    tableName = NULL;
    nextStep();
    if(command.compare("(")!=0) return transferError("\'(\' after table lost");
    
    nextStep();
    
    while (command.compare(")")!=0) {
        if (command.compare("primary") == 0) {
            nextStep();
            if(command.compare("key")!=0) return transferError("\'key\' after primary lost");
            nextStep();
            if (command.compare("(")!=0) return  transferError("\'(\' after key lost");
            
            nextStep();
            for (tmp=0; tmp<table->length; tmp++)
            {
                cout << *(table->attributes[tmp]->attributeName);
                if(*(table->attributes[tmp]->attributeName) == command)
                {
                    (table->attributes[tmp]->attributeProperty) |= PRIMARY|INDEX;
                    break;
                }
            }
            if (tmp == table->length) return  transferError("no such attribute name can be made into primary key");
            nextStep();
            if (command.compare(")")!=0) return  transferError("\')\' after primary key lost");
            nextStep();
            continue;
        }
        attributeName = new string(command);
        nextStep();
        if (command.compare("int") == 0) {
            attributeType = INT;
            attributeSize = 4;
        } else if (command.compare("float") == 0)
        {
            attributeType = FLOAT;
            attributeSize = 4;
        } else if (command.compare("char") == 0)
        {
            nextStep();
            nextStep();
            if(transferStringToInt() == 0) return transferError("it can\'t be transferred to an int");
            attributeSize = intValue;
            attributeType = CHAR;
            nextStep();
        } else return transferError("no such value type");
        nextStep();
        attributeProperty = 0;
        while (1) {
            if (command.compare("unique") == 0)
                attributeProperty |= UNIQUE;
            else if(command.compare("not")==0)
            {
                nextStep();
                attributeProperty |= NOTNULL;
            } else break;
            nextStep();
        }
        attribute = new SQLAttribute(attributeName,attributeType,attributeProperty,attributeSize);
        table->appendAttribute(attribute);
        attribute = NULL;
    }
    cout << "SQLCommandInterpreter::SQLCreateTableCommandTransfer Finished" << endl;
    res->init(table);
    table = NULL;
    resetPointers();
    return res;
}

SQLCommand* SQLCommandInterpreter::SQLCreateIndexCommandTransfer()
{
    cout << "SQLCommandInterpreter::SQLCreateIndexCommandTransfer" << endl;
    SQLCommandCreateIndex* res = new SQLCommandCreateIndex();
    res->commandType = CREATE_INDEX;
    nextStep();
    indexName = new string(command);
    if(command.compare("on") == 0) return transferError("indexName between index and on lost");
    nextStep();
    if(command.compare("on")!=0) return transferError("\'on\' after create index lost");
    nextStep();
    tableName = new string(command);
    nextStep();
    if(command.compare("(")!=0) return transferError("\'(\' after tableName lost");
    nextStep();
    attributeName = new string(command);
    if(command.compare("")==0) return transferError("no attribute in ()");
    nextStep();
    if(command.compare(")")!=0) return transferError("\')\' after index attribute lost");
    sqlindex = new SQLIndex(indexName,tableName,attributeName);
    res->init(sqlindex);
    sqlindex = NULL;
    resetPointers();
    return res;
}

SQLCommand* SQLCommandInterpreter::SQLCreateCommandTransfer()
{
    cout << "SQLCommandInterpreter::SQLCreateCommandTransfer" << endl;
    nextStep();
    if(command.compare("table") == 0) return SQLCreateTableCommandTransfer();
    if(command.compare("index") == 0) return SQLCreateIndexCommandTransfer();
    return transferError("key word after create is invalid");
}

SQLCommand* SQLCommandInterpreter::SQLDropTableCommandTransfer()
{
    cout << "SQLCommandInterpreter::SQLDropTableCommandTransfer" << endl;
    SQLCommandDropTable* res = new SQLCommandDropTable();
    res->commandType = DROP_TABLE;
    nextStep();
    tableName = new string(command);
    if(tableName->compare("")==0) return transferError("tableName after drop table lost");
    res->init(tableName);
    resetPointers();
    return res;
}

SQLCommand* SQLCommandInterpreter::SQLDropIndexCommandTransfer()
{
    cout << "SQLCommandInterpreter::SQLDropIndexCommandTransfer" << endl;
    SQLCommandDropIndex* res = new SQLCommandDropIndex();
    res->commandType = DROP_INDEX;
    nextStep();
    indexName = new string(command);
    if(indexName->compare("")==0) return transferError("indexName after drop index lost");
    res->init(indexName);
    resetPointers();
    return res;
}

SQLCommand* SQLCommandInterpreter::SQLDropCommandTransfer()
{
    cout << "SQLCommandInterpreter::SQLDropCommandTransfer" << endl;
    nextStep();
    if(command.compare("table") == 0) return SQLDropTableCommandTransfer();
    if(command.compare("index") == 0) return SQLDropIndexCommandTransfer();
    return transferError("key word after drop is invalid");
}



SQLCommand* SQLCommandInterpreter::SQLInsertCommandTransfer()
{
    cout << "SQLCommandInterpreter::SQLInsertCommandTransfer" << endl;
    SQLCommandInsertInto* res = new SQLCommandInsertInto();
    res->commandType = INSERT_INTO;
    nextStep();
    nextStep();
    tableName = new string(command);
    if((table = m_delegate->getTableInfo(tableName)) == NULL) return transferError("can\'t get infomation about this table");
    tableName = new string(command);
    nextStep();
    if(command.compare("values")!=0) return transferError("key word after tableName must be values");
    nextStep();
    if(command.compare("(")!=0) return transferError("\'(\' must follow values");
    tuple = new SQLTuple();
    for(int i=0;i<table->length;i++)
    {
        nextStep();
        switch(table->attributes[i]->attributeType)
        {
            case INT:
                if(transferStringToInt() == 0) return transferError("int value is not well formated");
                tuple->appendValue(new int(intValue), INT);
                break;
            case FLOAT:
                if(transferStringToFloat() == 0) return transferError("float value is not well formated");
                tuple->appendValue(new float(floatValue), FLOAT);
                break;
            case CHAR:
                if(!isString) return transferError("string value is not well formated");
                tuple->appendValue(new string(command),CHAR);
                break;
            default:return transferError("peculiar value type");
        }
    }
    nextStep();
    if(command.compare(")")!=0) return transferError("\')\' lost");	
    res->init(tableName,tuple);
    resetPointers();
    return res;
}

SQLCommand* SQLCommandInterpreter::SQLQuitCommandTransfer()
{
    SQLCommand* res = new SQLCommandQuit();
    res->commandType = QUIT;
    return res;
}

SQLCommand* SQLCommandInterpreter::transferStringToCommand(string inputString)
{
    cout << "SQLCommandInterpreter::transferStringToCommand" << endl;
    cout << "inputString = " << inputString << endl;
    index = 0;
    SQLCommandInterpreter::inputString = inputString;
    nextStep();
    if(command.compare("select") == 0) return SQLSelectCommandTransfer();
    if(command.compare("delete") == 0) return SQLDeleteCommandTransfer();
    if(command.compare("create") == 0) return SQLCreateCommandTransfer();
    if(command.compare("drop") == 0) return SQLDropCommandTransfer();
    if(command.compare("insert") == 0) return SQLInsertCommandTransfer();
    if(command.compare("quit") == 0) return SQLQuitCommandTransfer();
    return NULL;
}
