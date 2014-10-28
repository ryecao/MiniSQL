class CatalogManager {
public:
    bool        hasTable(const string &table_name);          //check if table {table_name} is already exists
    bool        hasIndex(const string &index_name);         //check if index {index_name} is already exists
    
    bool        registerTable(const TableInfo &table);      //register a new table {table}, return true if success
    bool        dropTable(const string &table_name);        //drop table {table_name}
    TableInfo   getTableInfo(const string &table_name);     //get the table's information and set the correspongind value of {table_name}
    void        writeTableInfo(const TableInfo &table);     //write table's information to file

    bool        registerIndex(const IndexInfo &index);      //register a new index {index}, return true if success
    bool        dropIndex(const string &index_name);        //drop index {index_name}
    IndexInfo   getIndexInfo(const string &index_name);     //get the index's information and set the correspongind value of {index_name}
    void        writeIndexInfo(const IndexInfo &index);     //write index's information to file
};