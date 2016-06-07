//
//  buffer.hpp
//  MiniSQL-final
//
//  Created by drinking on 10/26/15.
//  Copyright © 2015 drinking. All rights reserved.
//

#ifndef buffer_hpp
#define buffer_hpp

#include <stdio.h>
#include<iostream>
#include<map>
#include<string>
#include<vector>

using namespace std;

#define MAXBLOCKNUMBER 100000
//bufferManager总计10^5页，缓存区大小为400MB
#define BLOCKSIZE 4096
//4K一页

class bufferManager
{
private:
    static unsigned char* buffer[MAXBLOCKNUMBER];
    //共10^5个指针，每个指针指向一块4K大小的内存空间
    
    static int index;
    //用来为clock算法记录当前在第几快内存页处。
    
    static int countRef[MAXBLOCKNUMBER];
    //用来记录每一页被多少个指针引用了
    
    static int timeStamp[MAXBLOCKNUMBER];
    //用来实现clock算法，timeStamp=1时表示当前不可被置换，timeStamp=0时表示当前可以被置换
    
    static FILE* bufferToFile[MAXBLOCKNUMBER];
    //记录缓存区中每一页分别对应的哪一个文件
    
    static int bufferToPageNumber[MAXBLOCKNUMBER];
    //记录缓存区中每一页分别是对应文件的第几页
    
    static map<pair<FILE*,int>,int> filePageNumberTobuffer;
    //记录一个二元组<FILE*,int>(表示一个文件和该文件的第几页）在缓存区中的第几页
    
    static map<void*,int> pointerToPageNumber;
    //记录一个指针对应缓存区中的第几页
    
    static vector<FILE*> files;
    //记录当前缓存区中所有相关联的文件指针
    
    static vector<string> fileNames;
    //记录当前缓存区中所有相关联的文件名
    
    static int getIndexFromFileAndPage(FILE* fp,int pageNumber);
    //传入一个文件指针，和该文件的第几页，返回该二元组对应的在缓存区中的索引

    static int findAnAvailableBlock();
    //使用clock算法在缓存区中找到一块可以用的页，返回该页的索引
    
    static int findABlockToReplace(FILE* fp,int pageNumber);
    //使用clock算法在缓存区中找到一页，用该页的内容来替换，载入fp文件中的第pageNumber页，返回该页的索引
    
    static int loadIntoMemory(FILE* fp,int pageNumber,int index);
    //将fp文件的第pageNumber页载入缓存区的第index页中
    
    static int flushIntoDisk(FILE* fp,int pageNumber,int index);
    //将缓存区的第index页存入fp文件的第pageNumber页中
    
    FILE* fp;
    //注意！该私有变量非静态，表示此类实例化的对象所绑定的文件
    string* fileName;
    //注意！该私有变量非静态，表示此类实例化的对象所绑定的文件的文件名
public:
    ~bufferManager();
    bufferManager(string* fileName);
    //使用文件名实例化一个bufferManager类，将该对象与文件名对应的文件进行绑定
    
    static int initialMemory();
    //对整个bufferManager类进行初始化
    static void quitBufferManager();
    //结束对bufferManager类的所有使用
    
    void* startUsePage(int pageNumber);
    //通过实例化的buffer调用，传出一个指针，该指针对应buffer所绑定的文件的第pageNumber页的内容，可以通过该指针对该内容进行修改
    
    void finishUsePage(int pageNumber);
    //通过实例化的buffer调用，buffer所绑定的文件的第pageNumber页的countRef将减一（少一个指针引用），当countRef为0时，bufferManager将不再确保该页被pin在缓存区内。
    
    void* startUseBuffer(int offset);
    //通过实例化的buffer调用，传出一个指针，该指针对应buffer所绑定的文件的offset偏移位置所在的页的内容，可以通过该指针对该内容进行修改
    
    void finishUseBuffer(int offset);
    //通过实例化的buffer调用，buffer所绑定的文件的偏移位置offset所对应的页得出countRef将减一（少一个指针引用），当countRef为0时，bufferManager将不再确保该页被pin在缓存区内。
    
    int getFreshPage();
    //通过实例化的buffer调用，buffer将从绑定的文件中找出一页可以用的空白页以供使用，返回该空白页在文件中所对应的页号
    
    int pageNumber; //表示绑定的文件共有多少页，即文件大小为pageNumber*BLOCKSIZE
};
#endif
