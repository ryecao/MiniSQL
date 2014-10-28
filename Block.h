class Block {    
public:    
    Block():    pos(0)  {}
    Block(const Block &);               //must be implemented!
    bool operator=(const Block &);      //must be overloaded!
    template<class T>
    void append(const T& x);            //add x to the end
    void seek(int);                     //set the pointer's pos
    int getpos() const;                 //return the current pointer's pos

    int pos;                            //current pointer's pos
    unsigned char data[BLOCK_SIZE];     //data
    string fname;                       //the filename
};