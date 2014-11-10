#ifndef BUFFER_MANAGER_
#define BUFFER_MANAGER_

#include <string>
#include <list>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <fstream>
#define WRITE_MODE "a+"
#define READ_MODE "r"
using std::fstream;
#ifdef LOCAL_TEST
#define out(x)  std::cout<<#x<<" = "<<x<<std::endl
#endif
struct FilePointer {
    FILE *fp;
    int fsize;
};
extern std::map<std::string,FilePointer> FPServer;
FilePointer &FPOpenFile(const std::string &fname);
class FreeBlockManager {
public:
    typedef FilePointer FP;
    FreeBlockManager() {
        memset(null,0,sizeof(null));
    }
    int AllocateFreeBlock(const std::string &fname) {
        auto f=ds.find(fname);
        int o=-1;        
        if(f==ds.end()) {            
            FilePointer F=FPOpenFile(FileName(fname));
            std::list<int> p;        
            {
                while(fscanf(F.fp,"%d",&o)==1)    p.push_back(o);                
                if(p.empty())   throw "Try to get a free block which not exixts!!! 1";
                o=p.front();
                p.erase(p.begin());
                if(!p.empty())  ds[fname]=p;                
                return o;
            }                        
            throw "Try to get a free block which not exixts!!!  2";
            return -1;
        }else {
            if(f->second.empty())
                throw "Try to get a free block which not exixts!!! 3";
            o=f->second.front();
            f->second.erase(f->second.begin());            
            return o;
        }
    }
    int AllocateNewBlock(const std::string &fname) {        
        FP &F=FPOpenFile(fname);        
        fseek(F.fp,0,SEEK_END);                                        
        fwrite(null,BLOCK_SIZE,sizeof(null[0]),F.fp);
        int o=F.fsize;        
        F.fsize+=BLOCK_SIZE;        
        return o;
    }
    bool HasFreeBlock(const std::string &fname) {        
        auto f=ds.find(fname);
        if(f!=ds.end()) return !f->second.empty();
        std::list<int> p;
        int o;        
        FilePointer F=FPOpenFile(fname+".freeinfo");        
        while(fscanf(F.fp,"%d",&o)==1)    p.push_back(o);
        if(p.empty())   return 0;
        ds[fname]=p;        
        return 1;
    }
    bool BlockIsFree(const Block &b) {
        auto f=ds.find(b.fname);
        if(f==ds.end()) {            
            FilePointer F=FPOpenFile(FileName(b.fname));            
            int o;
            while(fscanf(F.fp,"%d",&o)==1) if(o==b.offset) {                
                return 1;
            }            
            return 0;
        }else {
            for(auto x:f->second) if(x==b.offset)   return 1;
            return 0;
        }
        return 0;        
    }
    void AddFreeBlock(const Block &b) {
        ds[b.fname].push_back(b.offset);
    }
    void ClearAllBlock(const std::string &fname) {
        auto f=ds.find(fname);
        if(f!=ds.end()) ds.erase(f);
        remove(FileName(fname).c_str());
    }
    void Flush() {
        for(auto &it:ds) {                        
            FILE *fp=fopen(FileName(it.first).c_str(),"w");
            for(auto x:it.second)
                fprintf(fp,"%d ",x);            
            fclose(fp);
        }
    }
private:
    std::map<std::string,std::list<int> > ds;
    static unsigned char null[BLOCK_SIZE];
    std::string FileName(const std::string &fname) {
        return fname+".freeinfo";
    }    
};
class BufferManager {    
public:
    static const int BufferSize = 1;
    BufferManager();
    ~BufferManager();
    Block AllocateNewBlock(std::string fname);   //allocate a new block in file '$fname'
    Block GetBlock(const std::string &fname,int offset);  //return the block in file '${fname}' start at ${pos}
    void WriteBlock(const Block &);               //save the block's data
    void FreeBlock(const Block &);                //delete a block
    void ClearFile(const std::string &fname);   //delete all block in file fname
    void PinBlock(const Block &);                   
    void ReleasePinBlock(const Block &);
    void Flush();
    std::list<Block> seebuffer();    
private:
    //depende on implementation
    static std::list<Block> pool;
    static FreeBlockManager fm;        
    Block AddBlock(const std::string &fname,int offset);    
    void WriteBack(const Block &b);    
};
#endif
