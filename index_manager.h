#ifndef INDEX_MANAGER_
#define INDEX_MANAGER_ 

#include <string>
#include <set>
#include "index_info.h"
#include "attribute_type.h"
#include "sql_command.h"

class IndexManager {
public:
    bool CreateIndex(const IndexInfo &index){return true;};
    bool AddRecord(const IndexInfo &index, std::string value, int offset);
    bool DeleteRecord(const IndexInfo &index, std::string value, int offset);
    bool EmptyIndex(const IndexInfo &index);    
    //return offsets of blocks found.
    std::vector<int> FindRecords(const IndexInfo& index, const WhereClause& where_clause);

};

#endif
