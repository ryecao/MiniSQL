// @copyright (c) 2014 ryecao
// @license: MIT
// @author(s): ryecao/ryecao@gmail.com
// created by ryecao on Sept. 28, 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file:index_manager.cpp
// @brief: used to manage the indices of the databases.
//
// please compile with -std=c++11

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
