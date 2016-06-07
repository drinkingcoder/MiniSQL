#include"bufferManager.hpp"
#include<stdlib.h>
#include<stdio.h>

int bufferManager::index = 0;

unsigned char* bufferManager::buffer[MAXBLOCKNUMBER];
        //buffer[i]表示第i页内存，每页4K
int bufferManager::countRef[MAXBLOCKNUMBER];
        //countRef[i]表示第i页正被多少个指针引用，当countRef[i]=0时，第i页才允许被替换
int bufferManager::timeStamp[MAXBLOCKNUMBER];
        //时间戳，使用表盘法实现LRU替换策略

FILE* bufferManager::bufferToFile[MAXBLOCKNUMBER];
        //记录第i页内存属于哪一个文件
int bufferManager::bufferToPageNumber[MAXBLOCKNUMBER];
        //记录第i页内存为源文件中的第几页
map<pair<FILE*,int>,int> bufferManager::filePageNumberTobuffer;
        //记录（文件指针+页号）组成的这一页在内存中的第几页
vector<FILE*> bufferManager::files;
        //记录当前内存中共存在多少个文件的内容
vector<string> bufferManager::fileNames;
        //为files文件指针对应的文件名

int bufferManager::initialMemory() //完成所有需要的初始化
{
    index = 0;
    quitBufferManager();
    memset(countRef,0,sizeof(countRef));
    memset(timeStamp,0,sizeof(timeStamp));
    memset(bufferToFile,0,sizeof(bufferToFile));
    memset(bufferToPageNumber,0,sizeof(bufferToPageNumber));
    filePageNumberTobuffer.clear();
    files.clear();
    fileNames.clear();
    for(int tmp = 0; tmp<MAXBLOCKNUMBER ; tmp++)
        buffer[tmp] = (unsigned char*)malloc(BLOCKSIZE);
    return 1;
}

bufferManager::bufferManager(string* fileName) //使用文件名构造一个自己的buffer管理器
{
    fp = NULL;
    for(int i=0; i<files.size();i++) //检查这个文件指针是否已经被记录
        if(fileNames[i] == *fileName)
        {
            fp = files[i];
            break;
        }
    if(fp == NULL)  //该文件为新出现的文件，需要创建新的文件指针
    {
        fp = fopen(fileName->c_str(),"rb+");
        if(fp == NULL) //文件创建失败
        {
            fp = fopen(fileName->c_str(),"a");
            fclose(fp);
            fp = fopen(fileName->c_str(),"rb+");
            if(fp == NULL){
                cout << "Failed To open File: " << *fileName << endl;
                delete fileName;
                return;
            }
        }
        fileNames.push_back(*fileName); //将新的文件放入容器
        files.push_back(fp);
    }
    this->fileName = fileName;
    fseek(fp, 0, SEEK_END);
    pageNumber = ftell(fp)/BLOCKSIZE; //计算这个文件共有多少页
}

bufferManager::~bufferManager()
{
    delete fileName;
}

void bufferManager::quitBufferManager() //将所有内存页中的内容写入文件并销毁所有内容
{
    for ( map<pair<FILE*,int>,int>::iterator itr = filePageNumberTobuffer.begin(); itr!=filePageNumberTobuffer.end();itr++) //将所有在内存块中的内容写入磁盘
        flushIntoDisk(itr->first.first, itr->first.second, itr->second);
    filePageNumberTobuffer.clear();
    for(vector<FILE*>::iterator itr = files.begin() ; itr!=files.end() ; itr++) //关闭所有打开的文件
        fclose(*itr);
    for(int tmp = 0; tmp<MAXBLOCKNUMBER ; tmp++) //释放申请的内存块
        free(buffer[tmp]);
}

int bufferManager::loadIntoMemory(FILE* fp,int pageNumber,int index) //将文件对应的页载入buffer[index]
{
    if(fseek(fp,pageNumber*BLOCKSIZE,SEEK_SET) != 0) return 0;
    if(fread(buffer[index],BLOCKSIZE,1,fp) == 0) return 0;
    return 1;
}

int bufferManager::flushIntoDisk(FILE* fp,int pageNumber,int index) //将buffer[index]的内容写入文件对应的位置
{
    if(fseek(fp,pageNumber*BLOCKSIZE,SEEK_SET) != 0) return 0;
    if(fwrite(buffer[index],BLOCKSIZE,1,fp) == 0) return 0;
    fflush(fp);
    return 1;
}

int bufferManager::findAnAvailableBlock() //采用LRU算法找到一个可以使用的块
{
    int tmp = 0;
    while(tmp<MAXBLOCKNUMBER*2) //设置搜索次数上限
    {
        tmp++;
        if(countRef[index] == 0) //当countRef>0时说明该内存块正被引用，被pin在内存内，无法被替换
        {
            if(timeStamp[index] == 0) return 1; //当timeStamp = 0 时代表可用
            timeStamp[index] = 0;   //否则将timeStamp置为0，这样在下一圈就可以被搜索到
        }
        index++;
        if(index == MAXBLOCKNUMBER) index = 0;
    }
    return 0; //当指针循环两圈仍然找不到可以放的块，说明所有块都被pin在内存内
}

int bufferManager::getIndexFromFileAndPage(FILE* fp,int pageNumber) //找到这个文件页对用的内存页序号
{
    map<pair<FILE*,int>,int>::iterator itr;
    itr = filePageNumberTobuffer.find(
                                      pair<FILE*,int>(fp,pageNumber)
                                      );
    return itr->second;
}

int bufferManager::findABlockToReplace(FILE* fp,int pageNumber) //用该文件页来替换掉一块内存
{
    if(findAnAvailableBlock() == 0) return -1; //没找到可以替换的内存
    if(bufferToFile[index] != NULL) //index即找到的那一块内存的序号
    {
        if(flushIntoDisk(bufferToFile[index],bufferToPageNumber[index],index) == 0) return -1; //写入磁盘失败
        map<pair<FILE*,int>,int>::iterator itr;
        itr = filePageNumberTobuffer.find(
                                          pair<FILE*,int>(bufferToFile[index],bufferToPageNumber[index])
                                          );
        filePageNumberTobuffer.erase(itr); //将原来的文件页从容器中去掉
    }
    if(loadIntoMemory(fp,pageNumber,index) == 0) return -1; //载入数据失败
    bufferToFile[index] = fp;
    bufferToPageNumber[index] = pageNumber; //记录对应的数据
    filePageNumberTobuffer.insert(pair<pair<FILE*,int>,int>(pair<FILE*,int>(fp,pageNumber),index)); //写入容器
    timeStamp[index] = 1;
    countRef[index]++;  //LRU算法的相关参数设置，引用加一
    return index;
}

int bufferManager::getFreshPage() //扩展源文件，取一块文件页出去
{
    fseek(fp, 0, SEEK_END);
    char* space = new char[BLOCKSIZE];
    memset(space, 0, BLOCKSIZE);
    fwrite(space, 1, BLOCKSIZE, fp); //扩展文件4k
    fflush(fp);
   /* fclose(fp);
    fp = fopen(fileName->c_str(),"r+");*/
    delete[] space;
    pageNumber++;
    return pageNumber-1; //返回这一页的地址
}

void* bufferManager::startUsePage(int pageNumber) //使用这一页内容
{
    map<pair<FILE*,int>,int>::iterator itr = filePageNumberTobuffer.find(pair<FILE*,int>(fp,pageNumber));
    if(itr != filePageNumberTobuffer.end()) //如果这一页原本就在内存中
    {
        countRef[itr->second]++;
        timeStamp[itr->second] = 1;
        index = itr->second;
        return (void*)(buffer[index]);
    } else {
        int index = findABlockToReplace(fp,pageNumber); //否则就找一页来进行替换
        if (index == -1) return  NULL;
        return (void*)(buffer[index]);
    }
}

void bufferManager::finishUsePage(int pageNumber) //这一页内容使用完毕，释放指针
{
    int index = getIndexFromFileAndPage(fp,pageNumber); //取得这一个文件页所在的内存指针
    countRef[index]--;  //销毁引用
}

void* bufferManager::startUseBuffer(int offset) //使用文件内这个偏移量对应的那一页，返回这个偏移量对应的内存指针
{
    int pageNumber = offset/BLOCKSIZE; //取得这个偏移量所在的页号
    char* ptr = (char*)startUsePage(pageNumber);
    int offInPage = offset - BLOCKSIZE* pageNumber;
    if (ptr == NULL) return NULL;
    return (void*)(ptr + offInPage);
}

void bufferManager::finishUseBuffer(int offset) //使用完毕
{
    int pageNumber = offset/BLOCKSIZE;
    finishUsePage(pageNumber);
}
