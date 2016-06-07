//
//  bPlusTreeNode.cpp
//  indexManagerTest
//
//  Created by drinking on 10/28/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#include "bPlusTreeNode.hpp"

bPlusTreeNode* bPlusTreeNode::constructNodeFromBuffer(void* bufferIn, SQLDataType dataType,int dataSize,bufferManager* buffermanager)
//通过给定参数，从bufferIn给的4K内存构建出一个bPlusTreeNode
{
    char* buffer = (char*)bufferIn;
    char str[100];
    int isLeaf = (int)(buffer[0]);
    buffer++;
    int validLength = *(int*)buffer;
    buffer+=sizeof(int);
   // int MaxSizePerNode = (BLOCKSIZE-1-sizeof(int))/(dataSize+sizeof(int)) - 1;
    int MaxSizePerNode = 5;
    bPlusTreeNode* node = new bPlusTreeNode(bufferIn,isLeaf,MaxSizePerNode,dataType,dataSize,buffermanager);
    if (validLength == 0) return node;
    SQLData* data;
    int pointer;
    pointer = *(int*)buffer;
    buffer+=sizeof(int);
    node->pointers.push_back(pointer);
    //取走第一个指针
    for(int i=0; i<validLength ; i++) //取走所有值和指针对
    {
        switch (dataType) {
            case INT:
                data = new SQLDataInt(*(int*)buffer);
                break;
            case FLOAT:
                data = new SQLDataFloat(*(float*)buffer);
                break;
            case CHAR:
                memcpy(str, buffer, dataSize);
                str[dataSize] = '\0';
                data = new SQLDataChar(new string(str));
                break;
            default:
                break;
        }
        buffer+=dataSize;
        pointer = *(int*)buffer;
        buffer+=sizeof(int);
        node->appendData(data,pointer);
    }
   // if(isLeaf) node->pointers.erase(node->pointers.begin()+node->validLength);
    return node;
}

int bPlusTreeNode::writeNodeIntoBuffer(bPlusTreeNode* node, int pageNumber)
//将node的内容写到pageNumber这一页缓冲区中
{
    char* buffer = (char*)(node->block);
    buffer[0] = char(node->isLeaf);
    buffer++;
    *(int*)buffer = node->validLength;
    if(node->validLength == 0) return 1;
    buffer+=sizeof(int);
    *(int*)buffer = node->pointers[0];
    buffer+=sizeof(int);
    for(int i=0; i<node->validLength ; i++) //取走所有值和指针对
    {
        switch (node->dataType) {
            case INT:
                *(int*)buffer = node->data[i]->intValue;
                break;
            case FLOAT:
                *(float*)buffer = node->data[i]->floatValue;
                break;
            case CHAR:
                memcpy(buffer, node->data[i]->charValue->c_str(), node->dataSize);
                break;
            default:
                break;
        }
        buffer+=node->dataSize;
        *(int*)buffer = node->pointers[i+1];
        buffer+=sizeof(int);
    }
    return 1;
}

bPlusTreeNode* bPlusTreeNode::startUseTreeNode(int pageNumber,SQLDataType dataType,int dataSize,bufferManager* buffer)
{
    void* block = buffer->startUsePage(pageNumber);
    bPlusTreeNode* node = bPlusTreeNode::constructNodeFromBuffer(block, dataType ,dataSize,buffer);
    return node;
}

void bPlusTreeNode::finishUseTreeNode(bPlusTreeNode* node,int pageNumber)
{
    bPlusTreeNode::writeNodeIntoBuffer(node,pageNumber);
    node->buffer->finishUsePage(pageNumber);
    delete node;
}

SQLData* bPlusTreeNode::getMinimalData()
{
    if(isLeaf) {
        if (dataType == INT) return new SQLDataInt(*(data[0]));
        if (dataType == FLOAT) return new SQLDataFloat(*(data[0]));
        if (dataType == CHAR) return new SQLDataChar(*(data[0]));
    }
    bPlusTreeNode* node = startUseTreeNode(pointers[0],dataType,dataSize,buffer);
    SQLData* res = node->getMinimalData();
    finishUseTreeNode(node, pointers[0]);
    return res;
}

int bPlusTreeNode::deleteFromNode(SQLData* data)
{
    int index;
    if(!isLeaf)
    {
        for(index = 0;index<validLength;index++)
            if(*(this->data[index])>*data) break;
        
        bPlusTreeNode* nodeIndex = startUseTreeNode(pointers[index],dataType,dataSize,buffer);
        if(nodeIndex->deleteFromNode(data) == 0)
        {
            cout << "Delete Failed" << endl;
            finishUseTreeNode(nodeIndex, pointers[index]);
            return 0;
        }
        if(index!=0)
        {
            delete this->data[index-1];
            this->data[index-1] = nodeIndex->getMinimalData();
        }
        finishUseTreeNode(nodeIndex, pointers[index]);
        if(nodeIndex->isApproriate()==0) this->mergeIndex(index);
        return 1;
    }
    else {
        vector<SQLData*>::iterator itr;
        for(itr = this->data.begin() ; itr != this->data.end() ;itr++)
            if(**itr == *data) break;
        if(itr == this->data.end()) return 0;
        int i = int(itr - this->data.begin());
        this->pointers.erase(this->pointers.begin()+i);
        delete *itr;
        this->data.erase(itr);
        validLength--;
        return 1;
    }
}

int bPlusTreeNode::mergeIndex(int index)
{
    if(index == validLength) index--;
    bPlusTreeNode* node1 = startUseTreeNode(pointers[index],dataType,dataSize,buffer);
    bPlusTreeNode* node2 = startUseTreeNode(pointers[index+1],dataType,dataSize,buffer);
    if(node1->isLeaf == 0)
        node1->appendData(node2->getMinimalData(),node2->pointers[0]);
    else node1->pointers.erase(node1->pointers.begin()+node1->validLength);

    int increment = 1 - node1->isLeaf;
    for(int i=0;i<node2->getValidLength();i++)
    {
        switch (dataType) {
            case INT:
                node1->appendData(new SQLDataInt(*(node2->data[i])),node2->pointers[i+increment]);
                break;
            case FLOAT:
                node1->appendData(new SQLDataFloat(*(node2->data[i])),node2->pointers[i+increment]);
                break;
            case CHAR:
                node1->appendData(new SQLDataChar(*(node2->data[i])),node2->pointers[i+increment]);
                break;
            default:
                break;
        }
    }
    if(node1->isLeaf) node1->pointers.push_back(node2->pointers[node2->validLength]);
    finishUseTreeNode(node2, pointers[index+1]);
    //没动
    delete data[index];
    data.erase(data.begin()+index);

    pointers.erase(pointers.begin()+index+1);
    validLength--;

    if (node1->getValidLength()+1 <= MaxSizePerNode) {
        finishUseTreeNode(node1, pointers[index]);
        return 1;
    } else {
        finishUseTreeNode(node1, pointers[index]);
        return splitIndex(index);
    }
}

int bPlusTreeNode::insertIntoNode(SQLData* data,int recordAddress)
{
    int index;
    for(index = 0;index<validLength;index++)
        if(*(this->data[index])>=*(data)) break;
    cout << "Insert into index:" << index << endl;
    
    if(!isLeaf)
    {
        bPlusTreeNode* node = startUseTreeNode(pointers[index],dataType,dataSize,buffer);
        if (node->insertIntoNode(data,recordAddress)==0) {
            finishUseTreeNode(node, pointers[index]);
            return 0;
        }
        if (node->getValidLength()+1>MaxSizePerNode) {
            cout << "node(not Leaf)->validLength = " << node->validLength << endl;
            finishUseTreeNode(node, pointers[index]);
            return splitIndex(index);
        }
        finishUseTreeNode(node, pointers[index]);
    }
    else {

        this->data.insert(this->data.begin()+index,data);
        this->pointers.insert(this->pointers.begin()+index, recordAddress);
        validLength++;
    }
    return 1;
}

int bPlusTreeNode::splitIndex(int index)
{
    cout << "Split Start at Index: " <<index << endl;

    pointers.insert(pointers.begin()+index+1, buffer->getFreshPage());
    data.insert(data.begin()+index, NULL);
    cout << "data Insert" << endl;
    validLength++;
    bPlusTreeNode* node1 = startUseTreeNode(pointers[index],dataType,dataSize,buffer);
    cout << "node1->validLength = " << node1->validLength << endl;
    bPlusTreeNode* node2 = startUseTreeNode(pointers[index+1],dataType,dataSize,buffer);
    node2->isLeaf = node1->isLeaf;
    cout << "node2->validLength = " << node2->validLength << endl;
    
    int mid = (node1->getValidLength()+1)/2;
    
//    if(node2->isLeaf == 0) node2->pointers.push_back(node1->pointers[mid]);

    node2->pointers.push_back(node1->pointers[mid]);
    for(int i=mid; i<node1->getValidLength(); i++)
        node2->appendData(node1->data[i],node1->pointers[i+1]);

    cout << "mid = " << mid << endl;
    if(node1->isLeaf)
    {
        node1->setValidLength(mid);
        node1->pointers.push_back(pointers[index+1]);
    } else node1->setValidLength(mid-1);
    this->data[index] = node2->getMinimalData();
    cout << "after split " << endl << "node1->validLength = " << node1->validLength << endl;
    finishUseTreeNode(node1, pointers[index]);
    cout << "finish node1"  << endl;
    finishUseTreeNode(node2, pointers[index+1]);
    cout << "finish node2" << endl;
    cout << "node2->validLength = " << node2->validLength << endl;

    return 1;
}

int bPlusTreeNode::findEqualTo(SQLData* data)
{
    if( isLeaf )
    {
        for (int i=0; i<getValidLength(); i++)
            if (*(this->data[i]) == *data) return pointers[i];
        return -1;
    }
    int i;
    for (i=0; i<getValidLength(); i++)
        if (*(this->data[i])>*data)
        {
            bPlusTreeNode* node = bPlusTreeNode::startUseTreeNode(pointers[i], dataType, dataSize, buffer);
            int res = node->findEqualTo(data);
            bPlusTreeNode::finishUseTreeNode(node, pointers[i]);
            return res;
        }
    bPlusTreeNode* node = bPlusTreeNode::startUseTreeNode(pointers[i], dataType, dataSize, buffer);
    int res = node->findEqualTo(data);
    bPlusTreeNode::finishUseTreeNode(node, pointers[i]);
    return res;
}

int bPlusTreeNode::chooseIndex(SQLData* data)
{
    for (int i=0; i<getValidLength(); i++)
        if (*(this->data[i])>*data) return i;
    return validLength;
}

void bPlusTreeNode::printTree(int tabs)
{
    if(!isLeaf && validLength!=0)
    {
        bPlusTreeNode* node = startUseTreeNode(pointers[0],dataType,dataSize,buffer);
        node->printTree(tabs+1);
        finishUseTreeNode(node, pointers[0]);
    }
    for (int i=0; i<validLength; i++)
    {
        if(isLeaf)
        {
            for(int j=0;j<tabs;j++)
                cout << ' ';
            cout << (this->pointers[i]) <<endl;
        }
        for(int j=0;j<tabs;j++)
            cout << ' ';
        cout << *(this->data[i]->charValue) <<endl;

        if(!isLeaf)
        {
            bPlusTreeNode* node = startUseTreeNode(pointers[i+1],dataType,dataSize,buffer);
            node->printTree(tabs+1);
            finishUseTreeNode(node, pointers[i+1]);
        }
    }
    if(isLeaf) cout <<pointers[validLength] << endl;
}

int bPlusTreeNode::findLargerThan(SQLData* data, vector<int>& recordAddress)
{
    if(isLeaf)
    {
        for(int i=0; i<validLength ; i++)
            if(*this->data[i] > *data ) recordAddress.push_back(pointers[i]);
        if(pointers[validLength] == -1)
        {
            delete data;
            return 0;
        }
        bPlusTreeNode* p = startUseTreeNode(pointers[validLength], dataType, dataSize, buffer);
        p->findLargerThan(data, recordAddress);
        finishUseTreeNode(p, pointers[validLength]);
        return 0;
    }
    int index = chooseIndex(data);
    bPlusTreeNode* p = startUseTreeNode(pointers[index], dataType, dataSize, buffer);
    p->findLargerThan(data, recordAddress);
    finishUseTreeNode(p, pointers[index]);
    return 0;
}

int bPlusTreeNode::findLessThan(SQLData* data, vector<int>& recordAddress)
{
    if(isLeaf)
    {
        for(int i=0; i<validLength ; i++)
        {
            if(*this->data[i] >= *data)
            {
                delete data;
                return 0;
            }
            recordAddress.push_back(pointers[i]);
        }
        if(pointers[validLength] == -1)
        {
            delete data;
            return 0;
        }
        bPlusTreeNode* p = startUseTreeNode(pointers[validLength], dataType, dataSize, buffer);
        p->findLessThan(data, recordAddress);
        finishUseTreeNode(p, pointers[validLength]);
        return 0;
    }
    bPlusTreeNode* p = startUseTreeNode(pointers[0], dataType, dataSize, buffer);
    p->findLessThan(data, recordAddress);
    finishUseTreeNode(p, pointers[0]);
    return 0;
}