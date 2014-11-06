#ifndef INDEX_MANAGER_
#define INDEX_MANAGER_ 

#include <string>
#include "index_info.h"
#include "attribute_type.h"

class IndexManager {
public:
    bool CreateIndex(const IndexInfo &index){return true;};
    bool AddRecord(const std::string &table_name, const AttrType &K, int P);
    bool DeleteRecord(const std::string &table_name, const AttrType &K, int P);    

};

#endif
