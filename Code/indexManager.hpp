//
//  indexManager.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/29/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef indexManager_hpp
#define indexManager_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "SQLDelegates.hpp"
#include "SQLCommand.hpp"
#include "bufferManager.hpp"
#include "bPlusTree.hpp"

using namespace std;

class indexManager {
private:
    static indexDelegate* m_delegate;
    //委托
public:
    static int deleteIndex(string* indexName,SQLData* data);
    //删除
    static int insertIndex(string* indexName,SQLData* data,int recordAddress);
    //插入
    static int deleteIndexWithCondition(string* indexName,SQLCondition* condition,vector<SQLCondition*>& otherConditions);
    
    /*——————————————————————————————————————————————————————————————————*/
    /*    带条件的删除，其中condition为indexManager筛选所依靠的条件，而        */
    /*  otherConditions将通过委托deleteRecordAddressFromIndex()来传送给API */
    /*  一共record进行进一步筛选时作为筛选条件。                              */
    /*——————————————————————————————————————————————————————————————————*/
    
    static int selectIndexWithCondition(string* indexName,SQLCondition* condition,vector<SQLCondition*>& otherConditions);
    //类似deleteIndexWithCondition

    static void setDelegate(indexDelegate* delegate)
    {
        m_delegate = delegate;
    }
};

#endif /* indexManager_hpp */
