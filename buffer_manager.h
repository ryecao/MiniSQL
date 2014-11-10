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
#include <set>
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
extern std::map<std::string,std::set<int> > FreeFPServer;
FilePointer &FPOpenFile(const std::string &fname);
class FreeBlockManager {
public:
    typedef FilePointer FP;
    FreeBlockManager() {
        memset(null,0,sizeof(null));
    }
    int AllocateFreeBlock(const std::string &fname) {
        FP &F=FPOpenFile(fname);
        auto f=FreeFPServer.find(fname);        
        assert(f!=FreeFPServer.end() && !f->second.empty());
        int o=*(f->second.begin());
        fseek(F.fp,o,SEEK_SET);
        fwrite(null,BLOCK_SIZE,1,F.fp);
        return o;
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
        int o;        
        FilePointer F=FPOpenFile(fname+".freeinfo");
        fseek(F.fp,0,SEEK_SET);
        bool f=0;
        while(fscanf(F.fp,"%d",&o)==1)    f=1;
        return f;
    }
    bool BlockIsFree(const Block &b) {
        auto f=FreeFPServer.find(b.fname);
        if(f==FreeFPServer.end()) return 0;
        return f->second.count(b.getoffset());        
    }
    void AddFreeBlock(const Block &b) {
        FreeFPServer[b.fname].insert(b.getoffset());
    }
    void ClearAllBlock(const std::string &fname) {
        auto f=FreeFPServer.find(fname);
        if(f!=FreeFPServer.end())   FreeFPServer.erase(f);
        remove((fname+".freeinfo").c_str());
    }
    void Flush() {
        for(auto &it:FreeFPServer) {
            FILE *fp=fopen(FileName(it.first).c_str(),"w");
            for(auto x:it.second)
                fprintf(fp,"%d ",x);            
            fclose(fp);
        }
    }
private:    
    static unsigned char null[BLOCK_SIZE];
    std::string FileName(const std::string &fname) {
        return fname+".freeinfo";
    }    
};
class BufferManager {    
public:
    static const int BufferSize = 100;
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
