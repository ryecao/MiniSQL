#ifndef INDEX_MANAGER_
#define INDEX_MANAGER_ 

#include <string>
#include <set>
#include "index_info.h"
#include "attribute_type.h"
#include "sql_command.h"
#include "table_info.h"

class IndexManager {
public:
    bool CreateIndex(std::string type, int size, const IndexInfo &index);
    bool CreateIndex(int type, int size, const IndexInfo &index);
    bool AddRecord(const TableInfo &table,const IndexInfo &index, std::string value, int offset);
    bool DeleteRecord(const TableInfo &table,const IndexInfo &index, std::string value, int offset);
    bool EmptyIndex(const IndexInfo &index);    
    //return offsets of blocks found.
    bool FindValue(const TableInfo &table, const IndexInfo& index, std::string value);
    std::vector<int> FindRecords(const TableInfo &table,const IndexInfo& index, const WhereClause& where_clause);

};

#endif
