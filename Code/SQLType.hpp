//
//  SQLType.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef SQLType_hpp
#define SQLType_hpp

#include <stdio.h>

typedef enum SQLCommandType{
    CREATE_TABLE,
    DROP_TABLE,
    CREATE_INDEX,
    DROP_INDEX,
    SELECT_FROM,
    INSERT_INTO,
    DELETE_FROM,
    QUIT
} SQLCommandType;

typedef enum SQLDataType{
    INT,
    FLOAT,
    CHAR
} SQLDataType;

#define UNIQUE 1
#define PRIMARY 2
#define NOTNULL 4
#define INDEX 8

#endif /* SQLType_hpp */
