#ifndef INDEX_MANAGER_
#define INDEX_MANAGER_ 

class IndexManager {
public:
    bool CreateIndex(const IndexInfo &index);
    bool AddRecord(const string &table_name, const AttrType &K, int P);
    bool DeleteRecord(const string &table_name, const AttrType &K, int P);    

};

#endif
