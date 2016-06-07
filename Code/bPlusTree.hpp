//
//  bPlusTree.hpp
//  indexManagerTest
//
//  Created by drinking on 10/28/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef bPlusTree_hpp
#define bPlusTree_hpp

#include <stdio.h>
#include "bPlusTreeNode.hpp"

class bPlusTree
{
public:
    bufferManager* buffer;
    //一颗B+树的所有节点必定都存在同一个文件里面，因此拥有一个buffer来进行文件管理
    
    bPlusTreeNode* root;
    //root作为B+树的根，bPlusTree将通过它来管理整棵B+树
    
    SQLDataType dataType;
    //记录这颗B+树的叶子节点的数据的数据类型，SQLDataType为自己设计的底层数据结构
    
    int dataSize;
    //用来记录B+树数据的大小
    
    int MaxSizePerNode;
    //根据B+树单个树节点的大小和一页4KB的大小，能够计算出每一个节点内最多有多少个数据
    
    bPlusTree(bufferManager* buffer,SQLDataType dataType, int dataSize);
    //构造函数
    
    ~bPlusTree();
    int insertData(SQLData* data,int recordAddress);
    //SQLData为自己设计的底层数据结构，包括SQLDataInt，SQLDataFloat，SQLDataChar，使用多态的技术实现这些操作，详细内容将在总体设计报告中呈现，recordAddress为伴随这个SQLData在record的地址。
    
    int deleteData(SQLData* data);
    //等值删除
    
    int findEqualTo(SQLData* data);
    //等值查找
    
    int findLargerThan(SQLData* data,vector<int>& recordAddress);
    int findLessThan(SQLData* data,vector<int>& recordAddress);
    //不等值查找，将查找到的符合要求的数据存在recordAddress内
    
    void printTree();
    //打印整棵树的相关信息
};

#endif /* bPlusTree_hpp */
