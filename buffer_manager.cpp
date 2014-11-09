#ifndef BUFFER_MANAGER_
#define BUFFER_MANAGER_

#include "block.h"

class FreeBlockManager {
public:
    FreeBlockManager() {
        memset(null,0,sizeof(null));
    }
    int AllocateFreeBlock(const std::string &fname) {
        auto f=ds.find(fname);
        int o=-1;        
        if(f==ds.end()) {
            FILE *fp=fopen(FileName(fname).c_str(),"r");
            std::list<int> p;
            if(fp) {
                while(fscanf(fp,"%d",&o)==1)    p.push_back(o);                
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
        FILE *fp=fopen(fname.c_str(),"wb+");
        fseek(fp,0,SEEK_END);
        int o=ftell(fp);
        fwrite(null,BLOCK_SIZE,sizeof(null[0]),fp);
        return o;
    }
    bool HasFreeBlock(const std::string &fname) {        
        auto f=ds.find(fname);
        if(f!=ds.end()) return !f->second.empty();
        std::list<int> p;
        int o;
        FILE *fp=fopen((fname+".freeinfo").c_str(),"rb+");
        if(!fp) return 0;
        while(fscanf(fp,"%d",&o)==1)    p.push_back(o);
        if(p.empty())   return 0;
        ds[fname]=p;
        return 1;
    }
    bool BlockIsFree(const Block &b) {
        auto f=ds.find(b.fname);
        if(f==ds.end()) {
            FILE *fp=fopen(FileName(b.fname).c_str(),"rb+");
            if(!fp) return 0;
            int o;
            while(fscanf(fp,"%d",&o)==1) if(o==b.offset)    return 1;
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
            FILE *fp=fopen(FileName(it.first).c_str(),"wb");
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
unsigned char FreeBlockManager::null[BLOCK_SIZE];

Block BufferManager::AllocateNewBlock(std::string fname) {
    if(fm.HasFreeBlock(fname)) {        
        return GetBlock(fname,fm.AllocateFreeBlock(fname));
    }    
    return GetBlock(fname,fm.AllocateNewBlock(fname));
}
Block BufferManager::GetBlock(const std::string &fname,int offset) {
    for(auto it=pool.begin();it!=pool.end();++it) if(it->fname==fname && it->offset==offset) {        
        Block t=*it;    pool.erase(it),pool.push_back(t);
        return t;
    }
    return AddBlock(fname,offset);        
}
Block BufferManager::AddBlock(const std::string &fname,int offset)  {
    Block res(fname,offset);
    if((int)pool.size()==BufferSize) {
        WriteBack(pool.front());    pool.pop_front();        
    }
    FILE *fp=fopen(fname.c_str(),"rb+");
    fseek(fp,offset,SEEK_SET);
    fread(res.data,BLOCK_SIZE,sizeof(res.data[0]),fp);
    pool.push_back(res);
    return res;    
}
void BufferManager::WriteBack(const Block &b) {
    if(b.is_dirty) {
        FILE *fp=fopen(b.fname.c_str(),"wb+");
        fseek(fp,b.offset,SEEK_SET);
        fwrite(b.data,BLOCK_SIZE,sizeof(b.data[0]),fp);
        fclose(fp);
    }
}
void BufferManager::WriteBlock(const Block &b) {
    for(auto it=pool.begin();it!=pool.end();++it) if(it->fname==b.fname && it->offset==b.offset) {
        *it=b;  it->is_dirty=1;
        Block tmp=*it;
        pool.erase(it),pool.push_back(tmp);
        return;
    }
    AddBlock(b.fname,b.offset);
    Block &t=pool.back();   t.is_dirty=1;
    memcpy(t.data,b.data,BLOCK_SIZE);
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
    Flush();
}
void BufferManager::Flush() {
    for(auto &it:pool) {
        WriteBack(it);
        it.is_dirty=0;
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

#endif
