class IndexInfo {
public:
    IndexInfo();
    IndexInfo(const string &index_name, const string &table_name, int aid);
    
    string  getName();
    string  getTableName();
    int     getAttributeID();       //return the number that the attribute's position in table's attributes list
}