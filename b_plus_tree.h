//The Union type, more convenient
struct AttrType {
    int idata;
    double fdata;
    string sdata;
    int t;
    AttrType(int d):        idata(d),t(0)    {}
    AttrType(double d):     fdata(d),t(1)    {}
    AttrType(string d):     sdata(d),t(2)    {}    
    friend bool operator<(const AttrType &lhs, const AttrType &rhs) {
        int t=lhs.t;
        if(t==0)    return lhs.idata<rhs.idata;
        if(t==1)    return dcmp(lhs.fdata-rhs.fdata)<0;
        if(t==2)    return lhs.sdata<rhs.sdata;
    }
    friend bool operator>(const AttrType &lhs, const AttrType &rhs) {
        int t=lhs.t;
        if(t==0)    return lhs.idata>rhs.idata;
        if(t==1)    return dcmp(lhs.fdata-rhs.fdata)>0;
        if(t==2)    return lhs.sdata>rhs.sdata;
    }
    friend bool operator<=(const AttrType &lhs, const AttrType &rhs) {
        return !(lhs>rhs);
    }
    friend bool operator>=(const AttrType &lhs, const AttrType &rhs) {
        return !(lhs<rhs);
    }
    friend bool operator==(const AttrType &lhs, const AttrType &rhs) {
        int t=lhs.t;
        if(t==0)    return lhs.idata==rhs.idata;
        if(t==1)    return dcmp(lhs.fdata-rhs.fdata)==0;
        if(t==2)    return lhs.sdata==rhs.sdata;
    }
    friend bool operator!=(const AttrType &lhs, const AttrType &rhs) {
        return !(lhs==rhs);
    }
};


namespace B_Plus_Tree {    
    class Node {
    public:
        Node()  {}
        Node(bool f1,bool f2,int firstp=-1):  isleaf(f1),isroot(f2)   {P.push_back(firstp);}
        Node(const Block &b) {

        }
        bool isLeaf() const          {return isleaf;}
        bool isRoot() const          {return isroot;}
        int getP(int k) const        {return P[k];}
        int getLastP() const         {return P.back();}
        AttrType getK(int k) const   {return K[k];}
        int PSize() const            {return P.size();}
        int KSize() const            {return K.size();}
        void insert(const AttrType &_K,int _P,int pos) {
            P.insert(P.begin()+pos,_P), K.insert(K.begin()+pos,_K);
        }
        void insert(const AttrType &_K,int _P) {
            K.push_back(_K),P.insert(P.end()-1,_P);
        }
        void appendK(const AttrType &_K) {
            K.push_back(_K);
        }
        void appendP(const int _P) {
            P.push_back(_P);
        }
        void insertK(const AttrType &_K,int pos) {
            K.insert(K.begin()+pos,_K);
        }
        void insertP(const int _P,int pos) {
            P.insert(P.begin()+pos,_P);
        }
        void clearAll() {
            P.clear(),K.clear();
        }
        void setRoot(bool d)    {isroot=d;}
        void writeToBlock(const Block &b) {
        
        }        
    private:
        bool isleaf, isroot;        
        std::vector<int> P;
        std::vector<AttrType> K;
    };
    class BPTree {
    public:
        BPTree()    {}
        BPTree(const string &fname, const IndexTypeInfo &_tinfo); //Create a bptree by fname(fname, format is described below)
        BPTree(const string &fname);                              //load a bptree from file 
        int lower_bound(const AttrType &V);                       //return the first block_offset(refer to the table data file, not the index file) that >= V   if not find return -1
        bool contains(const AttrType &V);                         //test if contains index V
        void insert(const AttrType &V,const int P);    //insert the pair(V,P)  V->index P->the block of the record                    

        //the following functions are not finished yet, the interface might be modified        
        void delete(const AttrType &V);             //delete key V        
        vector<int> getAllLess(const AttrType &V);  //find all record that the key is <V
        set<int> getAllLess(const AttrType &V);
        vector<int> getAllLessOrEqual(const AttrType &V);  //find all record that the key is <=V
        set<int> getAllLessOrEqual(const AttrType &V);
        vector<int> getAllGreater(const AttrType &V);  //find all record that the key is >V
        set<int> getAllLessGreater(const AttrType &V);
        vector<int> getAllGreaterOrEqual(const AttrType &V);  //find all record that the key is >=V
        set<int> getAllLessGreaterOrEqual(const AttrType &V);
    private:
        string fname;   // {database}_{tablename}_{indename}.index
        IndexTypeInfo tinfo;
        Node root_node;
        int fanout;
        int root_pos;
        std::stack<int> st;     // maintain a stack to get the node's parent
        int calcFanout() {
            int u=BLOCK_SIZE;
            u-=2*sizeof(bool)+2*sizeof(int);
            int v=sizeof(int)+tinfo.getIndexSize();
            return u/v;
        }
        void setBTreeInfo();
        int lower_bound_leaf(const AttrType &V);
        void insertIntoLeafNode(Node &u,const AttrType &V,const int P);
        void insertInParent(const Block &b1,const AttrType &K,const Block &b2);
        void insertIntoInnerNode(Node &p,const AttrType &K,int p1,int p2);
    };
};