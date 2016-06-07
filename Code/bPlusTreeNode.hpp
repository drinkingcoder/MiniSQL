//
//  bPlusTreeNode.hpp
//  indexManagerTest
//
//  Created by drinking on 10/28/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef bPlusTreeNode_hpp
#define bPlusTreeNode_hpp

#include<iostream>
#include<vector>
#include <stdio.h>
#include "SQLTuple.hpp"
#include "SQLType.hpp"
#include "SQLCondition.hpp"
#include "bufferManager.hpp"

using  namespace::std;

class bPlusTreeNode
{
private:
public:
    SQLDataType dataType;
    bufferManager* buffer;
    void* block;
    int MaxSizePerNode;
    int validLength;
    int isLeaf;
    int dataSize;
    vector<int> pointers;
    vector<SQLData*> data;
    //以上为bPlusTreeNode内部的相关数据
    
    int chooseIndex(SQLData* data);
    //当当前树节点不是叶子节点时，此函数根据data的值选择出下一次应当往哪一个分叉走

    SQLData* getMinimalData();
    //得到当前分支下的最小的节点值，当一个节点需要分裂时会使用此函数

    static bPlusTreeNode* constructNodeFromBuffer(void* buffer,SQLDataType dataType,int dataSize,bufferManager* buffermanager);
    //从一块4K大小的缓存区中，根据给定的相关元素来构造出一个bPlusTreeNode

    static int writeNodeIntoBuffer(bPlusTreeNode* node,int pageNumber);
    //将一个bPlusTreeNode根据一定的数据格式写到当前bPlusTree对应文件的第pageNumber页内

    bPlusTreeNode(void* block,int isLeaf,int MaxSizePerNode,SQLDataType dataType,int dataSize,bufferManager* buffer):block(block),MaxSizePerNode(MaxSizePerNode),isLeaf(isLeaf),validLength(0),dataType(dataType),dataSize(dataSize),buffer(buffer)
   	{
    }
    //构造函数，根据给的相关信息构造一个新的bPlusTreeNode节点

    ~bPlusTreeNode()
    {
        int i=0;
        for(vector<SQLData*>::iterator itr = data.begin() ; itr!=data.end() ; itr++)
            delete *itr;
    }
    //析构函数，将相关的指针全部释放掉

    int appendData(SQLData* data,int pointer=-1)
    {
        validLength++;
        this->data.push_back(data);
        this->pointers.push_back(pointer);
        return 1;
    }
    //将一组数据放到当前树节点的数据区的尾部
    
    static bPlusTreeNode* startUseTreeNode(int pageNumber,SQLDataType dataType,int dataSize,bufferManager* buffer);
    //在index文件中一个4K的页对应一个树节点，此函数接受一个bufferManager* buffer来了解是哪一个文件，通过pageNumber知道是此文件的第几页，然后根据相关参数将此页的内容构造成一个bPlusTreeNode传出
    
    static void finishUseTreeNode(bPlusTreeNode* node,int pageNumber);
    //类似startUseTreeNode函数，将node的数据根据某种格式写到其对应文件的第pageNumber页，然后析构掉node节点，结束对该节点的使用
    
    int getValidLength() { return validLength; }
    //获得当前节点分叉数的个数

    void setValidLength(int len) {
        validLength = len;
        for(vector<SQLData*>::iterator itr = data.begin()+len;itr!=data.end();)
              data.erase(itr);
        for (vector<int>::iterator itr = pointers.begin()+len+1; itr!=pointers.end(); )
            pointers.erase(itr);
        if(isLeaf)
            pointers.erase(pointers.begin()+len);
    }
    //将当前节点的分叉数置为len个，len之后的分叉将被删除，在分割操作时将调用此函数

    int mergeIndex(int index);
    //当前节点的第index分叉的值已经不满足B+树的性质了（因为之前在这一分叉下面曾经删除过一个叶子节点），所以需要将index分叉与index+1分叉或index-1分叉进行合并

    int splitIndex(int index);
    //当前节点的第index分叉的值已经不满足B+树的性质了（因为之前在这一分叉下面曾经插入过一个叶子节点，或者曾经在该分叉上进行了合并），所以需要将第index分叉进行平均分割

    int insertIntoNode(SQLData* data,int recordAddress);
    //将一组数据插入到B+树的叶子中，并对受到影响的B+树节点进行调整

    int deleteFromNode(SQLData* data);
    //将一个数据从B+树叶子节点中删除，与之对应的recordAddress也会被删除，并对受到应先的B+树节点进行调整

    int findEqualTo(SQLData* data);
    //等值查找

    int findLargerThan(SQLData* data,vector<int>& recordAddress);
    //由于B+树是有序树，此函数的做法是找到最左端符合要求的值找到并放到recordAddress中，然后利用B+树底层叶子节点之间的指针将从该节点起，知道最右端的叶子节点的值全部取出

    int findLessThan(SQLData* data,vector<int>& recordAddress);
    //由于B+树势有序数，此函数的做法是从B+树的最左端一个叶子开始向右扫描，直到遇到一个不符合条件的值，将此期间所有的值全部放入recordAddress中

    int isApproriate()
    {
        if(isLeaf) {
            if(validLength>=MaxSizePerNode/2 && validLength<=MaxSizePerNode-1) return 1;
            else return 0;
        } else {
            if(validLength+1>=(MaxSizePerNode+1)/2 && validLength+1<=MaxSizePerNode) return 1;
            else return 0;
        }
    }
    //判断当前节点的分叉数量是否满足B+树的性质

    void printTree(int tabs);
    //将整棵B+树的信息全部输出到屏幕
};

#endif /* bPlusTreeNode_hpp */
