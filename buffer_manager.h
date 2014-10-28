class BufferManager {    
public:
    BufferManager();

    Block allocateNewBlock(string fname);   //allocate a new block in file '$fname'
    Block getBlock(string fname, int pos);  //return the block in file '${fname}' start at ${pos}
    void writeBlock(Block &);               //save the block's data
    void freeBlock(Block &);                //delete a block
    void clearBlock(const string &fname);   //delete all block in file fname

private:
    //depende on implementation
};
