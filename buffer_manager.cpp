#include "block.h"
#include "buffer_manager.h"

std::map<std::string,FilePointer> FPServer;
FilePointer &FPOpenFile(const std::string &fname) {
    auto f=FPServer.find(fname);    
    if(f!=FPServer.end()) return f->second;    
    FilePointer F;
    F.fp=fopen(fname.c_str(),"rb+");
    if(!F.fp) {
        F.fp=fopen(fname.c_str(),"w");        
        fclose(F.fp);
        F.fp=fopen(fname.c_str(),"rb+");
        F.fsize=0;
    }else {
        fseek(F.fp,0,SEEK_END);
        F.fsize=ftell(F.fp);
    }    
    return FPServer[fname]=F;
}

unsigned char FreeBlockManager::null[BLOCK_SIZE];
std::list<Block> BufferManager::pool;
FreeBlockManager BufferManager:: fm;
BufferManager::BufferManager()  {}
Block BufferManager::AllocateNewBlock(std::string fname) {    
    if(fm.HasFreeBlock(fname)) {
        return GetBlock(fname,fm.AllocateFreeBlock(fname));
    }        
    return GetBlock(fname,fm.AllocateNewBlock(fname));
}
Block BufferManager::GetBlock(const std::string &fname,int offset) {    
    for(auto it=pool.begin();it!=pool.end();++it) if(it->fname==fname && it->offset==offset) {        
        Block t=*it;    
        memcpy(t.data,it->data,BLOCK_SIZE);
        pool.erase(it),pool.push_back(t);        
        return t;
    }    
    return AddBlock(fname,offset);        
}
Block BufferManager::AddBlock(const std::string &fname,int offset)  {
    Block res(fname,offset);    
    if((int)pool.size()==BufferSize) {        
        WriteBack(pool.front());    pool.pop_front();        
    }    
    FilePointer F=FPOpenFile(fname);
    fseek(F.fp,offset,SEEK_SET);
    fread(res.data,BLOCK_SIZE,sizeof(res.data[0]),F.fp);
    pool.push_back(res);        
    return res;    
}
void BufferManager::WriteBack(const Block &b) {
    if(b.is_dirty) {                
        FilePointer &F=FPOpenFile(b.fname);        
        fseek(F.fp,b.offset,SEEK_SET);
        fwrite(b.data,BLOCK_SIZE,sizeof(b.data[0]),F.fp);
        // Block v;
        // fseek(F.fp,b.offset,SEEK_SET);
        // fread(v.data,BLOCK_SIZE,1,F.fp);
        // for(int i=0;i<BLOCK_SIZE;++i)
        //     assert(v.data[i]==b.data[i]);
    }
}
void BufferManager::WriteBlock(const Block &b) {    
    for(auto it=pool.begin();it!=pool.end();++it) if(it->fname==b.fname && it->offset==b.offset) {
        Block tmp=b;        
        tmp.is_dirty=1;
        pool.erase(it),pool.push_back(tmp);
        return;
    }
    AddBlock(b.fname,b.offset);
    Block &t=pool.back();   t=b;
    t.is_dirty=1;    
}
void BufferManager::FreeBlock(const Block &b) {
    if(fm.BlockIsFree(b))
        throw puts("You are freeing a block which has been freed!!");
    for(auto it=pool.begin();it!=pool.end();++it) if(it->fname==b.fname && it->offset==b.offset) {
        pool.erase(it);
        break;
    }
    fm.AddFreeBlock(b);
}
void BufferManager::ClearFile(const std::string &fname) {
    fm.ClearAllBlock(fname);
    for(auto it=pool.begin();it!=pool.end();) {
        if(it->fname==fname)
            pool.erase(it++);
        else    ++it;
    }
    remove(fname.c_str());
}
BufferManager::~BufferManager() {    
}
void BufferManager::Flush() {
    // out(pool.size());
    BufferManager BM;
    for(auto it=pool.begin();it!=pool.end();++it) {        
        BM.WriteBack(*it);
        it->is_dirty=0;        
    }    
    fm.Flush();
}
void BufferManager::PinBlock(const Block &b) {
    int c=0,f=0;
    for(auto &it:pool) {
        if(it.fname==b.fname && it.offset==b.offset)
            it.is_pined=1,f=1;
        c+=it.is_pined;
    }
    if(!f)
        puts("Can not find the block to pin!");
    if(c==BufferSize) {
        puts("Warning! You have pined all the blocks in buffer!");
    }
}
void BufferManager::ReleasePinBlock(const Block &b) {
    bool f=0;
    for(auto &it:pool) {
        if(it.fname==b.fname && it.offset==b.offset)
            it.is_pined=0,f=1;
    }
    if(!f)
        puts("Can not find the block to releaase pin!");    
}
std::list<Block> BufferManager::seebuffer() {
    return pool;
}

class FinalFlush {
public:
    ~FinalFlush() {
        BufferManager().Flush();
    }
}___ff;
