//
//  interpreter.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef interpreter_hpp
#define interpreter_hpp

#include <stdio.h>
#include "SQLCondition.hpp"
#include "SQLType.hpp"
#include "SQLCommand.hpp"
#include "SQLTable.hpp"
#include "SQLIndex.hpp"
#include "SQLDelegates.hpp"

class SQLCommandInterpreter
{
private:
    //data
    static int index;
    //当前正在对inputString解析时，解析到的位置
    
    static int tmp;
    static string inputString;
    //用户输入的带解析成命令的字符串
    
    static string command;
    //使用nextStep函数后，下一条有效关键字将被以字符串形式载入command
    
    static int intValue;
    static float floatValue;
    static int isString;
    //使用transfer系列函数将command转化成对应的有效数据后，数据存在这组变量中
    
    static SQLTable* table;
    static SQLAttribute* attribute;
    
    static int attributeNumber;
    static string* attributeName;
    static SQLDataType attributeType;
    static string* tableName;
    static string* indexName;
    static int attributeProperty;
    static int attributeSize;
    static string* op;
    static SQLCondition* condition;
    static SQLTuple* tuple;
    static SQLIndex* sqlindex;
    //以上很多私有成员变量都是为了方便字符串解析所使用，之后再代码块中进行解释
    
    static SQLCommandInterpreterDelegate* m_delegate;
    //拥有一个委托作为私有变量，用来获得各种需要的信息
    
    
    static int transferStringToInt();
    static int transferStringToFloat();
    //将command中的字符串内容转化成int或者float后放到intValue或floatValue内
    
    static void nextStep();
    //从inputString中解析出下一条有效关键字放入command中
    
    static SQLCondition* transferStringToCondition();
    //将字符串转化成SQLCondition（数据结构）放到condition中
    
    static SQLCommand* SQLSelectCommandTransfer();
    static SQLCommand* SQLDeleteCommandTransfer();
    static SQLCommand* SQLCreateTableCommandTransfer();
    static SQLCommand* SQLCreateIndexCommandTransfer();
    static SQLCommand* SQLCreateCommandTransfer();
    static SQLCommand* SQLDropTableCommandTransfer();
    static SQLCommand* SQLDropIndexCommandTransfer();
    static SQLCommand* SQLDropCommandTransfer();
    static SQLCommand* SQLInsertCommandTransfer();
    static SQLCommand* SQLQuitCommandTransfer();
    //分别转化成对应的SQLCommand的，并传出指针，当指针值为NULL时说明转化失败
    
    static SQLCommand* transferError(string error);
    //当发生传输失败时调用此函数，他将输出error中的错误信息，将所有不为NULL的私有成员指针delete掉（这样才能防止内存泄露），传出NULL指针
    
    static void resetPointers();
    //将所有私有成员指针置为NULL
    
public:
    static SQLCommand* transferStringToCommand(string inputString);
    //传入用户输入的字符串，传出该字符串解析成的命令
    
    static void setDelegate(SQLCommandInterpreterDelegate* delegate)
    {
        m_delegate = delegate;
    }
    //设置委托
    
};
#endif /* interpreter_hpp */
