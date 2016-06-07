//
//  bPlusTree.cpp
//  indexManagerTest
//
//  Created by drinking on 10/28/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#include "bPlusTree.hpp"

bPlusTree::bPlusTree(bufferManager * buffer,SQLDataType dataType,int dataSize):buffer(buffer),dataType(dataType),dataSize(dataSize)
{
    if( buffer->pageNumber == 0 )
    {
        buffer->getFreshPage();
        root = bPlusTreeNode::startUseTreeNode(0, dataType, dataSize, buffer);
        root->isLeaf = 1;
    } else root = bPlusTreeNode::startUseTreeNode(0, dataType, dataSize, buffer);
    if (root->validLength == 0) root->isLeaf = 1;
   // MaxSizePerNode = (BLOCKSIZE-1-sizeof(int))/(dataSize+sizeof(int)) - 1;
    MaxSizePerNode = 5;
}

bPlusTree::~bPlusTree()
{
    bPlusTreeNode::finishUseTreeNode(root, 0);
}

int bPlusTree::insertData(SQLData* data,int recordAddress)
{
    if(root->isLeaf)
    {
        if(root->validLength == 0)
        {
            root->appendData(data,recordAddress);
            return 1;
        }
        if(root->insertIntoNode(data,recordAddress) == 0) return 0;
        if(root->validLength<MaxSizePerNode) return 1;
        int adr1 = buffer->getFreshPage();
        int adr2 = buffer->getFreshPage();
        bPlusTreeNode *p1 = bPlusTreeNode::startUseTreeNode(adr1, dataType, dataSize, buffer);
        bPlusTreeNode *p2 = bPlusTreeNode::startUseTreeNode(adr2, dataType, dataSize, buffer);
        p1->isLeaf = p2->isLeaf = 1;
        
        int i;
        for(i=0;i<MaxSizePerNode/2;i++)
            p1->appendData(root->data[i],root->pointers[i]);
        p1->pointers.push_back(adr2);
        for(;i<MaxSizePerNode;i++)
            p2->appendData(root->data[i],root->pointers[i]);
        p2->pointers.push_back(-1);

        root->pointers[0] = adr1;
        root->pointers[1] = adr2;

        root->isLeaf = 0;
        root->setValidLength(1);
        root->data[0] = p2->getMinimalData();
        bPlusTreeNode::finishUseTreeNode(p1, adr1);
        bPlusTreeNode::finishUseTreeNode(p2, adr2);
        return 1;
    }

    int index = root->chooseIndex(data);
    bPlusTreeNode* node = bPlusTreeNode::startUseTreeNode(root->pointers[index], dataType, dataSize, buffer);
    if (node->insertIntoNode(data,recordAddress) == 0) {
        bPlusTreeNode::finishUseTreeNode(node, root->pointers[index]);
        return 0;
    }
    if (node->isApproriate()) {
        bPlusTreeNode::finishUseTreeNode(node, root->pointers[index]);
        return 1;
    }
    cout << "root Split node->validLength = " << node->validLength << endl;
    bPlusTreeNode::finishUseTreeNode(node, root->pointers[index]);
    if(root->splitIndex(index) == 0) return 0;
    if (root->isApproriate()) return 1;
    cout << "New p1&p2 in insert" << endl;
    int adr1 = buffer->getFreshPage(),adr2 = buffer->getFreshPage();
    bPlusTreeNode* p1 = bPlusTreeNode::startUseTreeNode(adr1, dataType, dataSize, buffer);
    bPlusTreeNode* p2 = bPlusTreeNode::startUseTreeNode(adr2, dataType, dataSize, buffer);
    p1->isLeaf = 0;
    p2->isLeaf = 0;
    p1->pointers.push_back(root->pointers[0]);
    int i;
    cout << "P1 append" << endl;
    for(i=0;i<(root->validLength)/2;i++)
        p1->appendData(root->data[i],root->pointers[i+1]);
    
    p2->pointers.push_back(root->pointers[i+1]);
    
    cout << "P2 append" << endl;
    for(i++;i<root->validLength;i++)
        p2->appendData(root->data[i],root->pointers[i+1]);
    cout << "Finish P2 append" << endl;
    root->setValidLength(1);
    root->pointers[0] = adr1;
    root->pointers[1] = adr2;
    root->data[0] = p2->getMinimalData();
    bPlusTreeNode::finishUseTreeNode(p1, adr1);
    bPlusTreeNode::finishUseTreeNode(p2, adr2);
    return 1;
}

int bPlusTree::deleteData(SQLData* data)
{
    if(root->isLeaf)
        return root->deleteFromNode(data);
    int index = root->chooseIndex(data);
    bPlusTreeNode* node = bPlusTreeNode::startUseTreeNode(root->pointers[index], dataType, dataSize, buffer);
    if (node->deleteFromNode(data) == 0) {
        bPlusTreeNode::finishUseTreeNode(node, root->pointers[index]);
        return 0;
    }
    if(index!=0)
    {
        delete root->data[index-1];
        root->data[index-1] = node->getMinimalData();
    }

    if (node->isApproriate() == 0) {
        bPlusTreeNode::finishUseTreeNode(node, root->pointers[index]);
        if (root->mergeIndex(index) == 0) return 0;
        if(root->getValidLength()==0)
        {
            bPlusTreeNode* p=root;
            root = bPlusTreeNode::startUseTreeNode(root->pointers[0], dataType, dataSize, buffer);
            void* block = p->block;
            p->block = root->block;
            root->block = block;
            bPlusTreeNode::finishUseTreeNode(p, p->pointers[0]);
        }
        return 1;
    } else {
        bPlusTreeNode::finishUseTreeNode(node, root->pointers[index]);
        return 1;
    }
}

int bPlusTree::findEqualTo(SQLData* data)
{
    if(root == NULL) return -1;
    if(root->validLength == 0)
    {
        bPlusTreeNode* p = bPlusTreeNode::startUseTreeNode(root->pointers[0], dataType, dataSize, buffer);
        int res = p->findEqualTo(data);
        bPlusTreeNode::finishUseTreeNode(p, root->pointers[0]);
        return res;
    }
    int index = root->chooseIndex(data);
    bPlusTreeNode* p = bPlusTreeNode::startUseTreeNode(root->pointers[index], dataType, dataSize, buffer);
    int res = p->findEqualTo(data);
    bPlusTreeNode::finishUseTreeNode(p, root->pointers[index]);
    return res;
}

int bPlusTree::findLargerThan(SQLData* data, vector<int>& recordAddress)
{
    if(root == NULL) return -1;
    int index = root->chooseIndex(data);
    bPlusTreeNode* p = bPlusTreeNode::startUseTreeNode(root->pointers[index], dataType, dataSize, buffer);
    int res = p->findLargerThan(data, recordAddress);
    bPlusTreeNode::finishUseTreeNode(p, root->pointers[index]);
    return res;
}

int bPlusTree::findLessThan(SQLData* data, vector<int>& recordAddress)
{
    if(root == NULL) return -1;
    int index = 0;
    bPlusTreeNode* p = bPlusTreeNode::startUseTreeNode(root->pointers[index], dataType, dataSize, buffer);
    int res = p->findLessThan(data, recordAddress);
    bPlusTreeNode::finishUseTreeNode(p, root->pointers[index]);
    return res;
}

void bPlusTree::printTree()
{
    root->printTree(0);
}
