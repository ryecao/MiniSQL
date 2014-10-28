class IndexManager {
public:
    bool createIndex(const IndexInfo &index);
    bool addRecord(const string &table_name, const AttrType &K, int P);
    bool deleteRecord(const string &table_name, const AttrType &K, int P);    
}
