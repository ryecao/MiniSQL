#include <iostream>
#include <cstdio>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <stack>
#include <set>
#include <cstring>
#include <string>
#include <list>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <cassert>
#include <fstream>
#include "block.h"
#include "buffer_manager.h"


#define LOCAL_TEST
#ifdef LOCAL_TEST
#define out(x)  std::cout<<#x<<"= "<<x<<std::endl
#endif
using std::string;
int dcmp(double x) {
    return fabs(x)<1e-9 ? 0 : x<0 ? -1 : 1;
}
struct AttrType {
    int idata;
    double fdata;
    string sdata;
    int t;
    AttrType():             t(-1)           {}
    AttrType(int d):        idata(d),t(0)    {}
    AttrType(double d):     fdata(d),t(1)    {}
    AttrType(string d):     sdata(d),t(2)    {}    
    friend bool operator<(const AttrType &lhs, const AttrType &rhs) {
        int t=lhs.t;
        if(t==0)    return lhs.idata<rhs.idata;
        if(t==1)    return dcmp(lhs.fdata-rhs.fdata)<0;
        if(t==2)    return lhs.sdata<rhs.sdata;
        throw puts("AttrType Not Found Type");
    }
    friend bool operator>(const AttrType &lhs, const AttrType &rhs) {
        int t=lhs.t;
        if(t==0)    return lhs.idata>rhs.idata;
        if(t==1)    return dcmp(lhs.fdata-rhs.fdata)>0;
        if(t==2)    return lhs.sdata>rhs.sdata;
        throw puts("AttrType Not Found Type");
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
        throw puts("AttrType Not Found Type");
    }
    friend bool operator!=(const AttrType &lhs, const AttrType &rhs) {
        return !(lhs==rhs);
    }    
};
std::ostream& operator<<(std::ostream &os,const AttrType &u) {
    os.precision(6);    int t=u.t;    
    if(t==0)    os<<u.idata;
    if(t==1)    os<<u.fdata;
    if(t==2)    os<<u.sdata;
    return os;
}
class IndexTypeInfo {
public:
    IndexTypeInfo(){};
    IndexTypeInfo(int is, int it, int ss):index_size(is),index_type(it),string_size(ss){};
    int getIndexSize()  {return index_size;};
    int getIndexType()  {return index_type;};
    int getStringSize() {return string_size;};

    void set_index_size(const int i)  {index_size = i;};
    void set_index_type(const int i)  {index_type = i;};
    void set_string_size(const int i) {string_size = i;};

private:
    int index_size;
    int index_type;
    int string_size;
};

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
        Node(bool f1,bool f2,int firstp=-1):  isroot(f1),isleaf(f2)   {
            // P.push_back(firstp);
        }
        Node(const Block &bk) { 
            static unsigned char mem[BLOCK_SIZE];
            memcpy(mem,bk.data,BLOCK_SIZE);
            auto b=mem;
            isleaf=b[0],isroot=b[1];
            auto o=b+2;            
            int psize=readint(o),ksize=readint(o),type=readint(o),len=readint(o);                        
            // out(bk.getoffset());
            // out(psize);
            // out(ksize);
            // out(type);
            // out(len);
            for(int i=0;i<psize;++i) {
                // assert(o<b+BLOCK_SIZE);
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
        void writeToBlock(Block &bk,IndexTypeInfo tinfo) {
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
        void show() {
            printf("isleaf=%d isroot=%d\n",isleaf,isroot);
            puts("P");
            for(auto x:P)   std::cout<<x<<" ";   std::cout<<std::endl;
            puts("K");            
            for(auto x:K)   std::cout<<x<<" ";   std::cout<<std::endl;
        }
        bool isleaf,isroot;        
    private:        
        std::vector<int> P;
        std::vector<AttrType> K;
        int readint(unsigned char*&b) {
            int x;
            memcpy(&x,b,sizeof(int));
            b+=sizeof(int);
            return x;
        }
        double readfloat(unsigned char*&b) {
            double x;
            memcpy(&x,b,sizeof(double));
            b+=sizeof(double);
            return x;
        }
        string readstring(unsigned char*&b,int len) {
            static char x[BLOCK_SIZE];
            memcpy(x,b,sizeof(x[0])*len);   x[len]=0;
            b+=sizeof(x[0])*len;            
            return x;
        }
        void writeint(int x,unsigned char *&b) {
            memcpy(b,&x,sizeof(int));
            b+=sizeof(int);
        }
        void writefloat(double x,unsigned char *&b) {
            memcpy(b,&x,sizeof(double));
            b+=sizeof(double);
        }
        void writestring(string x,int len,unsigned char *&b) {
            memcpy(b,x.c_str(),x.length());
            b+=len;
        }
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
    BufferManager BM;
    class BPTree {
    public:
        BPTree()    {}
        BPTree(const string &_fname, const IndexTypeInfo &_tinfo): fname(_fname) {
            tinfo=_tinfo, root_pos=0;
            fanout=calcFanout();
            out(fanout);
            setBTreeInfo();
            root_node=Node(1,1);
            root_node.appendP(-1);
            Block rb=BM.AllocateNewBlock(fname);
            root_node.writeToBlock(rb,tinfo);
            Block bb=BM.GetBlock(fname,0);            
        }
        BPTree(const string &fname) {            
            FILE *fp=fopen((fname+".btree_info").c_str(),"r");
            int a,b,c;
            fscanf(fp,"%d%d%d%d%d",&root_pos,&a,&b,&c,&fanout);
            fclose(fp);
            Block header_block = BM.GetBlock(fname,root_pos);
            root_node=Node(header_block); 
            this->fname=fname;
            tinfo=IndexTypeInfo(a,b,c);
        }
        int lower_bound_leaf(const AttrType &V) {            
            Node u=BM.GetBlock(fname,root_pos);
            int k=root_pos;
            while(!st.empty())  st.pop();
            st.push(k);     
            // printf("Find at %d\n",k);
            while(!u.isLeaf()) {
                // puts("*********");
                // u.show();
                // puts("***********");
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
                // printf("Find at %d\n",k); 
            }            
            return k;
        }
        int lower_bound(const AttrType &V) {            
            int k=lower_bound_leaf(V);      
            Node u=Node(BM.GetBlock(fname,k));
            k=u.getLastP();
            for(int i=0;i<u.KSize();++i) if(u.getK(i)>=V) {
                k=i;    break;
            }
            if(k==-1)   return k;
            return Node(BM.GetBlock(fname,k)).getP(0);
        }
        
        bool contains(const AttrType &V) {
            int o=lower_bound(V);
            if(o==-1)   return 0;
            Node u=Node(BM.GetBlock(fname,o));            
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
            if(!f) {                        
                u.appendK(V);
                u.insertP(P,u.PSize()-1);
            }
        }
        void insertIntoInnerNode(Node &p,const AttrType &K,int p1,int p2) {
            bool f=0;
            for(int i=0;i<p.PSize();++i) if(p.getP(i)==p1) {
                p.insertP(p2,i+1),p.insertK(K,i);                
                f=1;
                break;
            }
            if(!f)  throw puts("insertIntoInnerNode f=0!!!");
        }
        void insertInParent(Block &b1,const AttrType &K,Block &b2) {
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
                // printf("@ %d\n",st.top());  fflush(stdout);
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
        void insert(const AttrType &V,const int P) {    //insert the pair(V,P)  V->index P->the block of the record
            int k=lower_bound_leaf(V);                        
            Block nowb=BM.GetBlock(fname,k);
            Node u=Node(nowb);            
            // printf("offset=%d %d\n",k,u.KSize());
            if(u.KSize()+1<fanout) {        //The block still has capcity
                insertIntoLeafNode(u,V,P);
                u.writeToBlock(nowb,tinfo);                
            }else {               
                // puts("BIG");
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
                // puts("stack");
                // while(!tmp.empty()) printf("%d ",tmp.top()),tmp.pop();  fflush(stdout);

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
                        u1.writeToBlock(b,tinfo),u2.writeToBlock(newb,tinfo);
                        delete_entry(pb,newp,newb.getoffset());
                        BM.FreeBlock(newb);
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
                    throw puts("Unexpected Situation at 'b_plus_tree.cpp '");
                }
            }            
        }
        bool erase(AttrType &V) {
            int k=lower_bound_leaf(V);
            Block b=BM.GetBlock(fname,k);
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
        void show() {            
            dfs(root_pos,-1);
        }        
        void dfs(int u,int f) {           
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
                std::cout<<uu.getK(i)<<" "; 
            }std::cout<<std::endl;
            puts("----------------------------------------------");
            if(uu.isLeaf()) return;
            for(int i=0;i<uu.PSize();++i)
                dfs(uu.getP(i),u);
        }
    private:
        std::string fname;   // {tablename}_{indename}.index
        IndexTypeInfo tinfo;
        Node root_node;
        int fanout;
        int root_pos;
        std::stack<int> st;     // maintain a stack to get the node's parent
        int calcFanout() {
            int u=BLOCK_SIZE;
            u-=2*sizeof(char)+4*sizeof(int);    //isRoot,isLeaf,PSize(),KSize(),type,length
            int v=sizeof(int)+tinfo.getIndexSize();
            return u/v;
        }
        void setBTreeInfo() {            
            FILE *fp=fopen((fname+".btree_info").c_str(),"w");            
            fprintf(fp,"%d %d %d %d %d\n",root_pos,tinfo.getIndexSize(),tinfo.getIndexType(),tinfo.getStringSize(),fanout);
            fclose(fp);
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
            p.writeToBlock(pb,tinfo),u1.writeToBlock(b,tinfo),u2.writeToBlock(newb,tinfo);
        }        
        std::set<int> findAllOp1(const AttrType &V,bool (*cmp)(const AttrType &a,const AttrType &b)) {
            int o=root_pos;
            while(1) {
                Node u(BM.GetBlock(fname,o));
                if(u.isLeaf())  break;
                o=u.getP(0);
            }
            std::set<int> res;
            while(~o) {
                Node u(BM.GetBlock(fname,o));
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
                Node u(BM.GetBlock(fname,o));
                for(int i=0;i<u.KSize();++i) {
                    if(cmp(u.getK(i),V))    res.insert(u.getP(i))                        ;
                }
                o=u.getP(u.PSize()-1);
            }
            return res;
        }
    };
}

#ifdef LOCAL_TEST
void dd_binary_node() {
    using namespace B_Plus_Tree;
    Node u;
    u.isleaf=1,u.isroot=1;
    for(int i=0;i<10;++i)   u.appendP(i);
    for(int i=0;i<10;++i)   u.appendK(AttrType("ssss"));
    Block b;
    IndexTypeInfo t(2,10,10);
    u.writeToBlock(b,t);
    Node v(b);
    out(v.KSize());
    v.show();
}
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
    BPTree tree1(fname);    
    for(int i=0;i<50000;++i) {
        // printf("Insert %d\n",i);    fflush(stdout);
        // tree1.insert(AttrType(i),i);        
    }    
    puts("FileServer");
    for(auto &it:FPServer)
        printf("%s %d\n",it.first.c_str(),it.second.fsize);    
    tree1.show();
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
    // dd_binary_node();
    // dd_create_tree();
    // dd_insert();
    // dd_buffer_allocate_new();
    
    return 0;
}
#endif
