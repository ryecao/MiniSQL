#ifndef BUFFER_MANAGER_
#define BUFFER_MANAGER_

#include "block.h"

class BufferManager {    
public:
    BufferManager();

    Block AllocateNewBlock(string fname);   //allocate a new block in file '$fname'
    Block GetBlock(string fname, int pos);  //return the block in file '${fname}' start at ${pos}
    void WriteBlock(Block &);               //save the block's data
    void FreeBlock(Block &);                //delete a block
    void ClearBlock(const string &fname);   //delete all block in file fname

private:
    //depende on implementation
};

#endif
