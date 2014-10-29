#ifndef CATALOG_MANAGER_
#define CATALOG_MANAGER_

#include <string>
#include "table_info.h"
#include "index_info.h"

class CatalogManager {
public:
  bool HasTable(const std::string &table_name);          //check if table {table_name} is already exists
  bool HasIndex(const std::string &index_name);         //check if index {index_name} is already exists
  
  bool RegisterTable(const TableInfo &table);      //register a new table {table}, return true if success
  bool DropTable(const std::string &table_name);        //drop table {table_name}
  TableInfo GetTableInfo(const std::string &table_name);     //get the table's information and set the correspongind value of {table_name}
  void WriteTableInfo(const TableInfo &table);     //write table's information to file

  bool RegisterIndex(const IndexInfo &index);      //register a new index {index}, return true if success
  bool DropIndex(const std::string &index_name);        //drop index {index_name}
  IndexInfo GetIndexInfo(const std::string &index_name);     //get the index's information and set the correspongind value of {index_name}
  void WriteIndexInfo(const IndexInfo &index);     //write index's information to file
};

#endif