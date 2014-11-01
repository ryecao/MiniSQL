#include <iostream>
#include <cstdio>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <stack>
#include <set>
#include <cstring>

#define LOCAL_TEST
#ifdef LOCAL_TEST
#define out(x)  cout<<#x<<"= "<<x<<endl
using namespace std;
#endif
using std::string;
const int BLOCK_SIZE=4*1024;
class IndexTypeInfo {
public:
    int getIndexSize()  {return index_size;}
    int getIndexType()  {return index_type;}
    int getStringSize() {return string_size;}
private:
    int index_size;
    int index_type;
    int string_size;
};
class Block {
public:    
    Block():    pos(0)  {}
    Block(const Block &);
    bool operator=(const Block &);    
    int getpos() const;

    int pos;
    unsigned char data[BLOCK_SIZE];
    string fname;    
};
struct BufferManage {    
    Block allocateNewBlock(string fname);   //allocate a new block in file '$fname'
    Block getBlock(string fname, int pos);  //return the block in file '${fname}' start at ${pos}
    void writeBlock(Block &);
    void freeBlock(Block &);
}BM;
namespace B_Plus_Tree {
    template<class T,class S>
    void readVector(std::istringstream &ss,int sz,std::vector<S> &p) {
        T x;
        for(int i=0;i<sz;++i) {
            ss>>x;  p.push_back(S(x));
        }
    }
    class Node {
    public:
        Node()  {}
        Node(bool f1,bool f2,int firstp=-1):  isleaf(f1),isroot(f2)   {P.push_back(firstp);}
        Node(const Block &b) { 
            static char cc[BLOCK_SIZE];
            memcpy(cc,b.data,sizeof(b.data));                        
            std::istringstream ss((string(cc)));
            ss>>isroot>>isleaf;
            int psz,ksz;    ss>>psz>>ksz;
            readVector<int,int>(ss,psz,P);            
            if(ksz) {
                int t;  ss>>t;                
                if(t==0)    readVector<int,AttrType>(ss,ksz,K);
                if(t==1)    readVector<double,AttrType>(ss,ksz,K);
                if(t==2)    readVector<string,AttrType>(ss,ksz,K);
            }
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
        void setP(int pos,int p) {
            P[pos]=p;
        }
        void setK(int pos,const AttrType &k) {
            K[pos]=k;
        }
        void eraseK(int pos) {
            K.erase(K.begin()+pos);
        }
        void eraseP(int pos) {
            P.erase(P.begin()+pos);
        }
        void clearAll() {
            P.clear(),K.clear();
        }
        void setRoot(bool d)    {isroot=d;}
        void writeToBlock(Block &b) {
            std::stringstream ss;
            ss<<isroot<<" "<<isleaf<<" ";
            ss<<PSize()<<" "<<KSize()<<" ";            
            for(int i=0;i<PSize();++i)  ss<<getP(i)<<" ";
            if(KSize()) ss<<getK(0).t<<" ";
            for(int i=0;i<KSize();++i)  ss<<getK(i)<<" ";
            memset(b.data,0,sizeof(b.data));                    
            memcpy(b.data,ss.str().c_str(),BLOCK_SIZE);
        }        
        void show() {
            printf("isleaf=%d isroot=%d\n",isleaf,isroot);
            puts("P");
            for(auto x:P)   std::cout<<x<<" ";   std::cout<<std::endl;
            puts("K");
            for(auto x:K)   std::cout<<x<<" ";   std::cout<<std::endl;
        }
    // private:
        bool isleaf, isroot;        
        std::vector<int> P;
        std::vector<AttrType> K;
    };
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
    class BPTree {
    public:
        BPTree()    {}
        BPTree(const string &fname, const IndexTypeInfo &_tinfo): fname(fname) {
            tinfo=_tinfo, root_pos = BLOCK_SIZE;
            fanout=calcFanout();
            setBTreeInfo();
            root_node=Node(1,1,-1);
            Block rb=BM.allocateNewBlock(fname);
            root_node.writeToBlock(rb);
        }
        BPTree(const string &fname) {
            Block header_block = BM.getBlock(fname,0);
            root_node=Node(header_block);
            root_pos = getRootPos(header_block);
        }
        int lower_bound_leaf(const AttrType &V) {            
            Node u=root_node;
            int k=root_pos;
            while(!st.empty())  st.pop();
            st.push(k);
            while(!u.isLeaf()) {
                k=u.getLastP();
                for(int i=0;i<u.KSize();++i) if(u.getK(i)>V) {
                    k=i;   break;
                }
                st.push(k);
                u=Node(BM.getBlock(fname,k));                
            }
            return k;
        }
        int lower_bound(const AttrType &V) {            
            int k=lower_bound_leaf(V);      
            Node u=Node(BM.getBlock(fname,k));
            k=u.getLastP();
            for(int i=0;i<u.KSize();++i) if(u.getK(i)>=V) {
                k=i;    break;
            }
            if(k==-1)   return k;
            return Node(BM.getBlock(fname,k)).getP(0);
        }
        
        bool contains(const AttrType &V) {
            int o=lower_bound(V);
            if(o==-1)   return 0;
            Node u=Node(BM.getBlock(fname,o));            
            for(int i=0;i<u.KSize();++i) if(u.getK(i)==V)
                return 1;
            return 0;
        }
        
        void insertIntoLeafNode(Node &u,const AttrType &V,const int P) {
            bool f=0;
            for(int i=0;i<u.KSize();++i) if(u.getK(i)>V) {
                u.insert(V,P,i);
                f=1;
                break;
            }
            if(!f)  u.insert(V,P);            
        }
        void insertIntoInnerNode(Node &p,const AttrType &K,int p1,int p2) {
            for(int i=0;i<p.PSize();++i) if(p.getP(i)==p1) {
                p.insertP(p2,i+1),p.insertK(K,i);
                return;
            }
        }
        void insertInParent(Block &b1,const AttrType &K,Block &b2) {
            Node u1=Node(b1),u2=Node(b2);
            st.pop();
            if(u1.isRoot()) {
                Block pb=BM.allocateNewBlock(fname);
                Node p(1,0);
                p.appendP(b1.getpos()),p.appendP(b2.getpos());
                p.appendK(K);
                u1.setRoot(0),u1.writeToBlock(b1);
                root_node=p,root_pos=pb.getpos();
                setBTreeInfo();
            }else {
                Block nowb=BM.getBlock(fname,st.top());
                Node p=Node(nowb);
                if(p.PSize()<fanout) {  //the parent node still has capcity                    
                    insertIntoInnerNode(p,K,b1.getpos(),b2.getpos());
                }else {
                    Node pp=p;
                    insertIntoInnerNode(pp,K,b1.getpos(),b2.getpos());
                    Node &p1=p,p2(0,0);
                    p1.clearAll();
                    Block newb=BM.allocateNewBlock(fname);
                    for(int i=0;i<(fanout+1)/2;++i) {
                        p1.appendP(pp.getP(i));
                        if(i<(fanout+1)/2)
                            p1.appendK(pp.getK(i));
                    }
                    for(int i=(fanout+1)/2;i<=fanout;++i) {
                        p2.appendP(pp.getP(i));
                        if(i<fanout)
                            p2.appendK(pp.getK(i));
                    }
                    p1.writeToBlock(nowb),p2.writeToBlock(newb);
                    insertInParent(nowb,pp.getK((fanout+1)/2),newb);
                }
            }
        }        
        void insert(const AttrType &V,const int P) {    //insert the pair(V,P)  V->index P->the block of the record
            int k=lower_bound_leaf(V);
            Block nowb=BM.getBlock(fname,k);
            Node u=Node(nowb);
            if(u.KSize()+1<fanout) {        //The block still has capcity
                insertIntoLeafNode(u,V,P);
                u.writeToBlock(nowb);
            }else {
                Node v=u,u2(1,0,k=u.getLastP());
                insertIntoLeafNode(v,V,P);        
                Node &u1=u;
                u1.clearAll(),u2.clearAll();
                Block newb=BM.allocateNewBlock(fname);      //Split the node into two nodes
                for(int i=0;i<(fanout+1)/2;++i)
                    u1.appendK(v.getK(i)),u1.appendP(v.getP(i));
                u1.appendP(newb.getpos());
                for(int i=(fanout+1)/2;i<fanout;++i)
                    u2.appendK(v.getK(i)),u2.appendP(v.getP(i));
                u2.appendP(k);
                u.writeToBlock(nowb),u2.writeToBlock(newb);                
                insertInParent(nowb,u2.getK(0),newb);
            }
        }
        void deleteFromNode(Node &u,const AttrType &K,int P) {
            for(int i=0;i<u.KSize();++i) if(u.getK(i)==K)
                u.eraseK(i);
            for(int i=0;i<u.PSize();++i) if(u.getP(i)==P)
                u.eraseP(i);
        }
        bool delete_entry(Block &b,const AttrType &K,int P) {
            Node u(b);
            deleteFromNode(u,K,P);
            if(u.isRoot() && u.PSize()==1) {    //u is root and u has only one child
                if(u.getP(0)==-1) {
                    u.writeToBlock(b);
                    return 1;
                }
                Block newb=BM.getBlock(fname,u.getP(0));
                Node v(newb);   v.setRoot(1);
                v.writeToBlock(newb);
                BM.freeBlock(b);
                root_pos=newb.getpos(),root_node=v;
                setBTreeInfo();
                return 1;
            }else {
                u.writeToBlock(b);
                if(!u.isRoot() && checkNodeIsTooSmall(u)) {
                    st.pop();
                    int p_pos=st.top();
                    Block pb=BM.getBlock(fname,p_pos);
                    Node p(pb);
                    Block newb; AttrType newp;
                    bool f=0,ft=0;
                    for(int i=0;i<p.PSize();++i) if(p.getP(i)==b.getpos()) {    //find neibor                      
                        f=1;
                        if(i+1==p.PSize()) {
                            newb=BM.getBlock(fname,p.getP(i-1));
                            newp=p.getK(i-1);
                            ft=0;
                        }else {
                            newb=BM.getBlock(fname,p.getP(i+1));
                            newp=p.getK(i);
                            ft=1;
                        }
                    }
                    Node u2(newb),u1=u;
                    if(u1.PSize()+u2.PSize()<=fanout) {     //Able to merge to one node
                        if(ft)
                            std::swap(u1,u2),std::swap(b,newb);
                        if(!u1.isLeaf()) {
                            u2.appendK(newp);
                            for(int i=0;i<u.KSize();++i)
                                u2.appendK(u1.getK(i));
                            for(int i=0;i<u.PSize();++i)
                                u2.appendP(u1.getP(i));
                        }else {
                            u2.appendP(-1);
                            for(int i=0;i<u1.KSize();++i)
                                u2.appendK(u1.getK(i));
                            for(int i=0;i<u1.PSize();++i)
                                u2.appendP(u1.getP(i));
                        }
                        u1.writeToBlock(b),u2.writeToBlock(newb);
                        delete_entry(pb,newp,newb.getpos());
                        BM.freeBlock(newb);
                    }else {
                        if(!ft) {
                            if(!u1.isLeaf()) {
                                int nppm=u2.getP(u2.PSize()-1); 
                                AttrType t=u2.getK(u2.KSize()-1);                                
                                u2.eraseP(u2.PSize()-1);
                                u2.eraseP(u2.KSize()-1);
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
                    throw "Unexpected Situation at 'b_plus_tree.cpp '";
                }
            }            
        }
        bool erase(AttrType &V) {
            int k=lower_bound_leaf(V);
            Block b=BM.getBlock(fname,k);
            Node u(b);
            for(int i=0;i<u.KSize();++i) if(u.getK(i)==V)
                return delete_entry(b,V,u.getP(i));
            return 0;
        }
        std::set<int> getAllLess(const AttrType &V) {
            return findAllOp1(V,lessF);            
        }
        std::set<int> getAllLessOrEqual(const AttrType &V) {
            return findAllOp1(V,LessOrEqualF);
        }
        std::set<int> getAllGreater(const AttrType &V) {
            return findAllOp2(V,greaterF);   
        }
        std::set<int> getAllGreaterOrEqual(const AttrType &V) {
            return findAllOp2(V,greaterOrEqualF);
        }
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
        void setBTreeInfo() {
            // Block btree_head_info_block = BM.allocateNewBlock(fname);
            // btree_head_info_block.seek(0);
            // btree_head_info_block.append(root_pos);
            // btree_head_info_block.append(tinfo.getIndexSize());
            // btree_head_info_block.append(tinfo.getStringSize());
            // BM.writeBlock(btree_head_info_block);
            FILE *fp=fopen((fname+".btree_info").c_str(),"w");            
            fprintf(fp,"%d\n",root_pos);
            fclose(fp);
        }
        int getRootPos(Block &b) {
            FILE *fp=fopen((fname+".btree_info").c_str(),"w");
            if(!fp)     throw "Can not find the btree info error";
            int p;  fscanf(fp,"%d",&p);
            return p;
        }
        bool checkNodeIsTooSmall(Node &u) {
            return (u.isLeaf() && u.KSize()<fanout/2) || (!u.isLeaf() && u.PSize()<(fanout+1)/2);
        }
        void deleteOp1(Node &p,Block &pb,Node &u1,Block &b,Node &u2,Block &newb,AttrType &newp,AttrType t) {
            bool f=0;
            for(int i=0;i<p.KSize();++i) if(p.getK(i)==newp) {
                f=1;
                p.setK(i,t);   break;
            }
            p.writeToBlock(pb),u1.writeToBlock(b),u2.writeToBlock(newb);
        }        
        std::set<int> findAllOp1(const AttrType &V,bool (*cmp)(const AttrType &a,const AttrType &b)) {
            int o=root_pos;
            while(1) {
                Node u(BM.getBlock(fname,o));
                if(u.isLeaf())  break;
                o=u.getP(0);
            }
            std::set<int> res;
            while(~o) {
                Node u(BM.getBlock(fname,o));
                bool f=0;
                for(int i=0;i<u.KSize();++i) {
                    if(cmp(u.getK(i),V)) res.insert(u.getP(i));
                    else {
                        f=1;    break;
                    }
                    if(f)       break;
                    o=u.getP(u.PSize()-1);
                }
            }
            return res;
        }
        std::set<int> findAllOp2(const AttrType &V,bool (*cmp)(const AttrType &a,const AttrType &b)) {
            int o=lower_bound(V);
            std::set<int> res;
            while(~o) {
                Node u(BM.getBlock(fname,o));
                for(int i=0;i<u.KSize();++i) {
                    if(cmp(u.getK(i),V))    res.insert(u.getP(i))                        ;
                }
                o=u.getP(u.PSize()-1);
            }
            return res;
        }
    };
}
void test() {
    using namespace B_Plus_Tree;
    Node u;
    u.isleaf=u.isroot=1;
    for(int i=0;i<10;++i)   u.appendP(i);
    for(int i=0;i<20;++i)   u.appendK(AttrType(123.3));
    Block b;
    u.writeToBlock(b);
    Node u2(b);
    u2.show();
}
#ifdef LOCAL_TEST
int main(int argc, char const *argv[]) {
    // dd();
    int a,b;
    while(std::cin>>a>>b)    std::cout<<(a+b)<<"\n";
    return 0;
}
#endif
