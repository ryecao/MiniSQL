#include "b_plus_tree.h"

#ifdef LOCAL_TEST
#define out(x)  std::cout<<#x<<"= "<<x<<std::endl
#else
#define out(x)
#endif

namespace B_Plus_Tree {        
    Node::Node()  {}
    Node::Node(bool f1,bool f2,int firstp):  isroot(f1),isleaf(f2)   {
    }
    Node::Node(const Block &bk) { 
        static unsigned char mem[BLOCK_SIZE];
        memcpy(mem,bk.data,BLOCK_SIZE);
        auto b=mem;
        isleaf=b[0],isroot=b[1];
        auto o=b+2;            
        int psize=readint(o),ksize=readint(o),type=readint(o),len=readint(o);                                
        for(int i=0;i<psize;++i) {            
            int p=readint(o);
            P.push_back(p);
        }
        for(int i=0;i<ksize;++i) {
            AttrType k;      
            assert(o<b+BLOCK_SIZE);
            if(type==0) k=AttrType(readint(o));
            if(type==1) k=AttrType(readfloat(o));
            if(type==2) k=AttrType(readstring(o,len));                
            K.push_back(k);
        }                        
    }
    bool Node::isLeaf() const          {return isleaf;}
    bool Node::isRoot() const          {return isroot;}
    int Node::getP(int k) const        {return P[k];}
    int Node::getLastP() const         {return P.back();}
    AttrType Node::getK(int k) const   {return K[k];}
    int Node::PSize() const            {return P.size();}
    int Node::KSize() const            {return K.size();}
    void Node::insert(const AttrType &_K,int _P,int pos) {
        P.insert(P.begin()+pos,_P), K.insert(K.begin()+pos,_K);
    }
    void Node::insert(const AttrType &_K,int _P) {            
        K.push_back(_K),P.insert(P.end()-1,_P);            
    }
    void Node::appendK(const AttrType &_K) {
        K.push_back(_K);
    }
    void Node::appendP(const int _P) {
        P.push_back(_P);
    }
    void Node::insertK(const AttrType &_K,int pos) {
        K.insert(K.begin()+pos,_K);
    }
    void Node::insertP(const int _P,int pos) {
        P.insert(P.begin()+pos,_P);
    }
    void Node::setP(int pos,int p) {
        P[pos]=p;
    }
    void Node::setK(int pos,const AttrType &k) {
        K[pos]=k;
    }
    void Node::eraseK(int pos) {
        K.erase(K.begin()+pos);
    }
    void Node::eraseP(int pos) {
        P.erase(P.begin()+pos);
    }
    void Node::clearAll() {
        P.clear(),K.clear();
    }
    void Node::popbackP() {
        P.pop_back();
    }
    void Node::popbackK() {
        K.pop_back();
    }
    void Node::setRoot(bool d)    {isroot=d;}
    void Node::writeToBlock(Block &bk,IndexTypeInfo tinfo) {
        auto &b=bk.data;
        memset(b,0,BLOCK_SIZE);
        b[0]=isleaf,b[1]=isroot;
        auto o=b+2;                        
        writeint(PSize(),o),writeint(KSize(),o);            
        // out(tinfo.getIndexType());
        writeint(tinfo.getIndexType(),o),writeint(tinfo.getStringSize(),o);
        for(int i=0;i<(int)P.size();++i)
            writeint(P[i],o);
        int t=tinfo.getIndexType();                        
        for(int i=0;i<(int)K.size();++i) {
            if(t==0)    writeint(K[i].idata,o);
            if(t==1)    writefloat(K[i].fdata,o);
            if(t==2)    writestring(K[i].sdata,tinfo.getStringSize(),o);                
	       }            
        BufferManager BM;
        BM.WriteBlock(bk);
    }        
    void Node::show() {
        printf("isleaf=%d isroot=%d\n",isleaf,isroot);
        puts("P");
        for(auto x:P)   std::cout<<x<<" ";   std::cout<<std::endl;
        puts("K");            
        for(auto x:K)   std::cout<<x<<" ";   std::cout<<std::endl;
    }        
    int Node::readint(unsigned char*&b) {
        int x;
        memcpy(&x,b,sizeof(int));
        b+=sizeof(int);
        return x;
    }
    double Node::readfloat(unsigned char*&b) {
        double x;
        memcpy(&x,b,sizeof(double));
        b+=sizeof(double);
        return x;
    }
    string Node::readstring(unsigned char*&b,int len) {
        static char x[BLOCK_SIZE];
        memcpy(x,b,sizeof(x[0])*len);   x[len]=0;
        b+=sizeof(x[0])*len;            
        return x;
    }
    void Node::writeint(int x,unsigned char *&b) {
        memcpy(b,&x,sizeof(int));
        b+=sizeof(int);
    }
    void Node::writefloat(double x,unsigned char *&b) {
        memcpy(b,&x,sizeof(double));
        b+=sizeof(double);
    }
    void Node::writestring(string x,int len,unsigned char *&b) {
        memcpy(b,x.c_str(),x.length());
        b+=len;
    }    
    bool lessF(const AttrType &a,const AttrType &b) {
        return a<b;
    }
    bool LessOrEqualF(const AttrType &a,const AttrType &b) {
        return a<=b;
    }
    bool greaterF(const AttrType &a,const AttrType &b) {
        return a>b;
    }
    bool greaterOrEqualF(const AttrType &a,const AttrType &b) {
        return a>=b;
    }      

    BufferManager BM;

    BPTree::BPTree()    {}
    BPTree::BPTree(const string &_fname, const IndexTypeInfo &_tinfo): fname(_fname) {
        tinfo=_tinfo, root_pos=0;
        fanout=calcFanout();
        out(fanout);
        setBTreeInfo();
        root_node=Node(1,1);
        root_node.appendP(-1);
        Block rb=BM.AllocateNewBlock(fname);
        root_node.writeToBlock(rb,tinfo);        
    }
    BPTree::BPTree(const string &fname) {            
        FILE *fp=fopen((fname+".btree_info").c_str(),"r");
        int a,b,c;
        fscanf(fp,"%d%d%d%d%d",&root_pos,&a,&b,&c,&fanout);
        fclose(fp);
        Block header_block = BM.GetBlock(fname,root_pos);
        root_node=Node(header_block); 
        this->fname=fname;
        tinfo=IndexTypeInfo(a,b,c);
    }
    int BPTree::lower_bound_leaf(const AttrType &V) {            
        Node u=BM.GetBlock(fname,root_pos);
        int k=root_pos;
        while(!st.empty())  st.pop();
        st.push(k);             
        while(!u.isLeaf()) {
            k=-1;
            for(int i=0;i<u.KSize();++i) if(u.getK(i)>V) {
                k=u.getP(i);   break;
            }
            if(~k) {                    
                u=Node(BM.GetBlock(fname,k));                
            }else {
                k=u.getLastP();
                u=Node(BM.GetBlock(fname,k));
            }       
            st.push(k);
        }            
        return k;
    }        
    
    bool BPTree::contains(const AttrType &V) {
        int o=lower_bound_leaf(V);
        if(o==-1)   return 0;
        Node u=Node(BM.GetBlock(fname,o));            
        for(int i=0;i<u.KSize();++i) if(u.getK(i)==V)
            return 1;
        return 0;
    }
    int BPTree::find(const AttrType &V) {
        int o=lower_bound_leaf(V);
        if(o==-1)   return -1;
        Node u=Node(BM.GetBlock(fname,o));            
        for(int i=0;i<u.KSize();++i) if(u.getK(i)==V)
            return u.getP(i);
        return -1;
    }
    void BPTree::insertIntoLeafNode(Node &u,const AttrType &V,const int P) {
        bool f=0;
        for(int i=0;i<u.KSize();++i) if(u.getK(i)>V) {
            u.insert(V,P,i);
            f=1;
            break;
        }            
        if(!f) {                        
            u.appendK(V);
            u.insertP(P,u.PSize()-1);
        }
    }
    void BPTree::insertIntoInnerNode(Node &p,const AttrType &K,int p1,int p2) {
        bool f=0;
        for(int i=0;i<p.PSize();++i) if(p.getP(i)==p1) {
            p.insertP(p2,i+1),p.insertK(K,i);                
            f=1;
            break;
        }
        if(!f)  throw puts("insertIntoInnerNode f=0!!!");
    }
    void BPTree::insertInParent(Block &b1,const AttrType &K,Block &b2) {
        Node u1=Node(b1),u2=Node(b2);
        st.pop();
        if(u1.isRoot()) {
            Block pb=BM.AllocateNewBlock(fname);
            Node p(1,0);
            p.appendP(b1.getoffset()),p.appendP(b2.getoffset());
            p.appendK(K);
            u1.setRoot(0),u1.writeToBlock(b1,tinfo);
            root_node=p,root_pos=pb.getoffset();                
            setBTreeInfo();
            p.writeToBlock(pb,tinfo),u1.writeToBlock(b1,tinfo),u2.writeToBlock(b2,tinfo);
        }else {
            Block nowb=BM.GetBlock(fname,st.top());
            Node p=Node(nowb);
            if(p.PSize()<fanout) {  //the parent node still has capcity                    
                insertIntoInnerNode(p,K,b1.getoffset(),b2.getoffset());
                p.writeToBlock(nowb,tinfo);
            }else {
                Node pp=p;
                insertIntoInnerNode(pp,K,b1.getoffset(),b2.getoffset());
                if(!(pp.PSize()==fanout+1))                    
                    throw puts("insertInParent yoyoyo");
                Node &p1=p,p2(0,0);
                p1.clearAll();
                Block newb=BM.AllocateNewBlock(fname);
                for(int i=0;i<(fanout+1)/2;++i) {
                    p1.appendP(pp.getP(i));
                    if(i<(fanout+1)/2-1)
                        p1.appendK(pp.getK(i));
                }                    
                for(int i=(fanout+1)/2;i<=fanout;++i) {
                    p2.appendP(pp.getP(i));
                    if(i<fanout)
                        p2.appendK(pp.getK(i));
                }
                p1.writeToBlock(nowb,tinfo),p2.writeToBlock(newb,tinfo);
                insertInParent(nowb,pp.getK((fanout+1)/2-1),newb);
            }
        }
    }        
    void BPTree::insert(const AttrType &V,const int P) {    //insert the pair(V,P)  V->index P->the block of the record
        int k=lower_bound_leaf(V);                        
        Block nowb=BM.GetBlock(fname,k);
        Node u=Node(nowb);            
        if(u.KSize()+1<fanout) {        //The block still has capcity
            insertIntoLeafNode(u,V,P);
            u.writeToBlock(nowb,tinfo);                
        }else {               
            Node v=u,u2(0,1);
            k=u.getLastP();
            insertIntoLeafNode(v,V,P);        
            Node &u1=u;
            u1.clearAll(),u2.clearAll();
            Block newb=BM.AllocateNewBlock(fname);      //Split the node into two nodes
            for(int i=0;i<(fanout+1)/2;++i)
                u1.appendK(v.getK(i)),u1.appendP(v.getP(i));
            u1.appendP(newb.getoffset());
            for(int i=(fanout+1)/2;i<fanout;++i)
                u2.appendK(v.getK(i)),u2.appendP(v.getP(i));
            u2.appendP(k);
            u1.writeToBlock(nowb,tinfo),u2.writeToBlock(newb,tinfo);                
            std::stack<int> tmp=st;            
            insertInParent(nowb,u2.getK(0),newb);
        }
    }
    void BPTree::deleteFromNode(Node &u,const AttrType &K,int P) {
        for(int i=0;i<u.KSize();++i) if(u.getK(i)==K) {
            u.eraseK(i);
            break;
        }
        for(int i=0;i<u.PSize();++i) if(u.getP(i)==P) {
            u.eraseP(i);
            break;
        }
    }
    bool BPTree::delete_entry(Block &b,const AttrType &K,int P) {
        Node u(b);
        deleteFromNode(u,K,P);
        if(u.isRoot() && u.PSize()==1) {    //u is root and u has only one child                
            if(u.getP(0)==-1) {
                u.writeToBlock(b,tinfo);
                return 1;
            }
            Block newb=BM.GetBlock(fname,u.getP(0));
            Node v(newb);   v.setRoot(1);
            v.writeToBlock(newb,tinfo);
            BM.FreeBlock(b);
            root_pos=newb.getoffset(),root_node=v;
            setBTreeInfo();
            return 1;
        }else {
            u.writeToBlock(b,tinfo);
            assert(u.PSize()==u.KSize()+1);
            if(!u.isRoot() && checkNodeIsTooSmall(u)) {                    
                st.pop();
                int p_pos=st.top();
                Block pb=BM.GetBlock(fname,p_pos);
                Node p(pb);
                Block newb; AttrType newp;
                bool f=0,ft=0;
                for(int i=0;i<p.PSize();++i) if(p.getP(i)==b.getoffset()) {    //find neibor                      
                    f=1;
                    if(i+1==p.PSize()) {
                        newb=BM.GetBlock(fname,p.getP(i-1));
                        newp=p.getK(i-1);
                        ft=0;
                    }else {
                        newb=BM.GetBlock(fname,p.getP(i+1));
                        newp=p.getK(i);
                        ft=1;
                    }
                }
                Node u2(newb),u1=u;
                assert(f);
                if(u1.PSize()+u2.PSize()<=fanout) {     //Able to merge to one node
                    if(ft)
                        std::swap(u1,u2),std::swap(b,newb);
                    if(!u1.isLeaf()) {
                        u2.appendK(newp);
                        for(int i=0;i<u1.KSize();++i)
                            u2.appendK(u1.getK(i));
                        for(int i=0;i<u1.PSize();++i)
                            u2.appendP(u1.getP(i));
                    }else {
                        u2.popbackP();
                        for(int i=0;i<u1.KSize();++i)
                            u2.appendK(u1.getK(i));
                        for(int i=0;i<u1.PSize();++i)
                            u2.appendP(u1.getP(i));
                    }
                    u1.writeToBlock(b,tinfo),u2.writeToBlock(newb,tinfo);
                    delete_entry(pb,newp,b.getoffset());
                    BM.FreeBlock(b);    //!!HERE
                }else {
                    if(!ft) {
                        if(!u1.isLeaf()) {
                            int nppm=u2.getP(u2.PSize()-1); 
                            AttrType t=u2.getK(u2.KSize()-1);                                                                
                            u2.popbackP();
                            u2.popbackK();
                            u1.insertP(nppm,0),u1.insertK(newp,0);
                            deleteOp1(p,pb,u1,b,u2,newb,newp,t);                                
                        }else {
                            AttrType t=u2.getK(u2.KSize()-1);
                            int m=u2.KSize()-1;
                            u1.insertK(u2.getK(m),0),u1.insertP(u2.getP(m),0);
                            u2.eraseK(m),u2.eraseP(m);
                            deleteOp1(p,pb,u1,b,u2,newb,newp,t);
                        }
                    }else {
                        if(!u1.isLeaf()) {
                            int nppm=u2.getP(0);
                            AttrType t=u2.getK(0);
                            u2.eraseP(0),u2.eraseK(0);
                            u1.appendP(nppm),u1.appendK(newp);
                            deleteOp1(p,pb,u1,b,u2,newb,newp,t);
                        }else {
                            int m=u1.PSize()-1;
                            u1.appendK(u2.getK(0)),u1.insertP(u2.getP(0),m);
                            u2.eraseK(0),u2.eraseP(0);
                            deleteOp1(p,pb,u1,b,u2,newb,newp,u2.getK(0));
                        }
                    }
                }
            }else {
                // throw puts("Unexpected Situation at 'b_plus_tree.cpp '");
            }
        }            
    }
    bool BPTree::erase(const AttrType &V) {
        int k=lower_bound_leaf(V);
        out(k);
        Block b=BM.GetBlock(fname,k);
        Node u(b);
        for(int i=0;i<u.KSize();++i) if(u.getK(i)==V) {
            puts("Find to erase");
            bool f=delete_entry(b,V,u.getP(i));
            return 1;
        }
        return 0;
    }
    std::vector<int> BPTree::getAllLess(const AttrType &V) {
        return findAllOp1(V,lessF);            
    }
    std::vector<int> BPTree::getAllLessOrEqual(const AttrType &V) {
        return findAllOp1(V,LessOrEqualF);
    }
    std::vector<int> BPTree::getAllGreater(const AttrType &V) {
        return findAllOp2(V,greaterF);   
    }
    std::vector<int> BPTree::getAllGreaterOrEqual(const AttrType &V) {
        return findAllOp2(V,greaterOrEqualF);
    }
    void BPTree::show() {            
        dfs(root_pos,-1);
    }        
    void BPTree::dfs(int u,int f) {           
        puts("********************");
        Node uu(BM.GetBlock(fname,u));
        puts("----------------------------------------------");
        printf("at offset=%d fa=%d isroot=%d isleaf=%d\n",u,f,uu.isRoot(),uu.isLeaf());
        puts("pointer:");
        for(int i=0;i<uu.PSize();++i)
            printf("%d ",uu.getP(i));   puts("");
        puts("keys:");      
        fflush(stdout);
        for(int i=0;i<uu.KSize();++i) {   
            out(uu.getK(i).t);
            std::cout<<uu.getK(i)<<" "; 
        }std::cout<<std::endl;
        puts("----------------------------------------------");
        if(uu.isLeaf()) return;
        for(int i=0;i<uu.PSize();++i)
            dfs(uu.getP(i),u);
    }    
    int BPTree::calcFanout() {
        int u=BLOCK_SIZE;
        u-=2*sizeof(char)+4*sizeof(int);    //isRoot,isLeaf,PSize(),KSize(),type,length
        int v=sizeof(int)+tinfo.getIndexSize();
        return u/v;
    }
    void BPTree::setBTreeInfo() {            
        FILE *fp=fopen((fname+".btree_info").c_str(),"w");            
        fprintf(fp,"%d %d %d %d %d\n",root_pos,tinfo.getIndexSize(),tinfo.getIndexType(),tinfo.getStringSize(),fanout);
        fclose(fp);
    }        
    bool BPTree::checkNodeIsTooSmall(Node &u) {
        return (u.isLeaf() && u.KSize()<fanout/2) || (!u.isLeaf() && u.PSize()<(fanout+1)/2);
    }
    void BPTree::deleteOp1(Node &p,Block &pb,Node &u1,Block &b,Node &u2,Block &newb,AttrType &newp,AttrType t) {
        bool f=0;
        for(int i=0;i<p.KSize();++i) if(p.getK(i)==newp) {
            f=1;
            p.setK(i,t);   break;
        }
        p.writeToBlock(pb,tinfo),u1.writeToBlock(b,tinfo),u2.writeToBlock(newb,tinfo);
    }        
    std::vector<int> BPTree::findAllOp1(const AttrType &V,bool (*cmp)(const AttrType &a,const AttrType &b)) {
        std::vector<int> res;
        int o=root_pos;
        while(1) {
            Node u(BM.GetBlock(fname,o));
            if(u.isLeaf())  break;
            o=u.getP(0);
        }
        out(o);            
        while(~o) {
            Node u(BM.GetBlock(fname,o));
            bool f=0;
            for(int i=0;i<u.KSize();++i) {
                if(cmp(u.getK(i),V)) res.push_back(u.getP(i));
                else {
                    f=1;    break;
                }                    
            }
            if(f)       break;
            o=u.getP(u.PSize()-1);
        }
        return res;
    }
    std::vector<int> BPTree::findAllOp2(const AttrType &V,bool (*cmp)(const AttrType &a,const AttrType &b)) {
        int o=lower_bound_leaf(V);
        std::vector<int> res;
        while(~o) {
            Node u(BM.GetBlock(fname,o));
            for(int i=0;i<u.KSize();++i) {
                if(cmp(u.getK(i),V))    res.push_back(u.getP(i));
            }
            o=u.getP(u.PSize()-1);
        }
        return res;
    }
    void BPTree::deleteAll() {
        BM.ClearFile(fname);
        root_pos=0;
        fanout=calcFanout();        
        setBTreeInfo();
        root_node=Node(1,1);
        root_node.appendP(-1);
        Block rb=BM.AllocateNewBlock(fname);
        out(rb.getoffset());
        root_node.writeToBlock(rb,tinfo);
    }
}

#ifdef LOCA_TEST_BPTREE
void dd_create_tree() {
    IndexTypeInfo t(10,2,10);
    string fname="testtable_testindex.index";
    using namespace B_Plus_Tree;
    BPTree tree(fname,t);
    BPTree tree1(fname);
    tree1.show();
}
void dd_insert() {
    IndexTypeInfo t(4,0,-1);
    string fname="testtable_testindex.index";
    using namespace B_Plus_Tree;
    BPTree tree(fname,t);    
    for(int i=1;i<=10;++i) {        
        tree.insert(AttrType(i),-i);
    }    
    puts("FileServer");
    for(auto &it:FPServer)
        printf("%s %d\n",it.first.c_str(),it.second.fsize);    
    tree.show();
}
void dd_delete() {    
    IndexTypeInfo t(sizeof(int),0,-1);
    string fname="testtable_testindex.index";
    remove(fname.c_str());
    remove((fname+".btree_info").c_str());
    remove((fname+".freeinfo").c_str());
    using namespace B_Plus_Tree;
    BPTree tree(fname,t);    
    for(int i=1;i<=1000;++i) {        
        tree.insert(AttrType(i),-i);
    }    
    // using std::vector;    
    // for(int i=-1000;i<=10005;++i) {
    //     int f=tree.find(i);
    //     if(i>=1&&i<=1000) {
    //         if(f!=-i) {
    //             out(i);
    //             out(f);
    //             assert(0);
    //         }
    //     }else {
    //         assert(f==-1);
    //     }
    // }
    // tree.show();
    tree.deleteAll();
    tree.show();
}
void dd_buffer_allocate_new() {
    BufferManager BM;
    Block b1=BM.AllocateNewBlock("testt");
    out(b1.getoffset());
    Block b2=BM.AllocateNewBlock("testt");
    out(b2.getoffset());
    Block b3=BM.AllocateNewBlock("testt");
    out(b3.getoffset());
}
int main(int argc, char const *argv[]) {
    dd_delete();
    return 0;
}
#endif
