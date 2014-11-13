#ifndef B_PLUS_TREE_
#define B_PLUS_TREE_

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
#include <cassert>
#include <fstream>
#include "block.h"
#include "buffer_manager.h"
#include "attribute_type.h"

using std::string;

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
    class Node {
    public:
        Node();
        Node(bool f1,bool f2,int firstp=-1);
        Node(const Block &bk);
        bool isLeaf() const;
        bool isRoot() const;
        int getP(int k) const;
        int getLastP() const;
        AttrType getK(int k) const;
        int PSize() const;
        int KSize() const;
        void insert(const AttrType &_K,int _P,int pos);
        void insert(const AttrType &_K,int _P);
        void appendK(const AttrType &_K);
        void appendP(const int _P);
        void insertK(const AttrType &_K,int pos);
        void insertP(const int _P,int pos);
        void setP(int pos,int p);
        void setK(int pos,const AttrType &k);
        void eraseK(int pos);
        void eraseP(int pos);
        void clearAll();
        void popbackP();
        void popbackK();
        void setRoot(bool d);
        void writeToBlock(Block &bk,IndexTypeInfo tinfo);
        void show();        
    private:        
        bool isleaf,isroot;      
        std::vector<int> P;
        std::vector<AttrType> K;
        int readint(unsigned char*&b);
        float readfloat(unsigned char*&b);
        string readstring(unsigned char*&b,int len);
        void writeint(int x,unsigned char *&b);
        void writefloat(float x,unsigned char *&b);
        void writestring(string x,int len,unsigned char *&b);
    };

    bool lessF(const AttrType &a,const AttrType &b);
    bool LessOrEqualF(const AttrType &a,const AttrType &b);
    bool greaterF(const AttrType &a,const AttrType &b);
    bool greaterOrEqualF(const AttrType &a,const AttrType &b);

    extern BufferManager BM;

    class BPTree {
    public:
        BPTree();
        BPTree(const string &_fname, const IndexTypeInfo &_tinfo);
        BPTree(const string &fname);        
        bool contains(const AttrType &V);
        int find(const AttrType &V);        
        void insert(const AttrType &V,const int P);        
        bool erase(const AttrType &V);
        std::vector<int> getAllLess(const AttrType &V);
        std::vector<int> getAllLessOrEqual(const AttrType &V);
        std::vector<int> getAllGreater(const AttrType &V);
        std::vector<int> getAllGreaterOrEqual(const AttrType &V);
        void deleteAll();
        void show();        
    private:
        std::string fname;   // {tablename}_{indename}.index
        IndexTypeInfo tinfo;
        Node root_node;
        int fanout;
        int root_pos;
        std::stack<int> st;     // maintain a stack to get the node's parent
        int calcFanout();
        void setBTreeInfo();
        bool checkNodeIsTooSmall(Node &u);
        void deleteOp1(Node &p,Block &pb,Node &u1,Block &b,Node &u2,Block &newb,AttrType &newp,AttrType t);
        std::vector<int> findAllOp1(const AttrType &V,bool (*cmp)(const AttrType &a,const AttrType &b));
        std::vector<int> findAllOp2(const AttrType &V,bool (*cmp)(const AttrType &a,const AttrType &b));

        int lower_bound_leaf(const AttrType &V);
        void insertIntoLeafNode(Node &u,const AttrType &V,const int P);
        void insertIntoInnerNode(Node &p,const AttrType &K,int p1,int p2);
        void insertInParent(Block &b1,const AttrType &K,Block &b2);   
        void deleteFromNode(Node &u,const AttrType &K,int P);
        bool delete_entry(Block &b,const AttrType &K,int P);
        void dfs(int u,int f);
    };
};
#endif
