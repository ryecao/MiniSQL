#ifndef CATALOG_MANAGER_
#define CATALOG_MANAGER_

#include <string>
#include "table_info.h"
#include "index_info.h"

class CatalogManager {
public:
  bool HasTable(const std::string&); //check if table {table_name} is already exists
  bool HasIndex(const std::string&); //check if index {index_name} is already exists
  bool HasAttribute(const std::string &, const std::string &);
  
  bool RegisterTable(const TableInfo&); //register a new table {table}, return true if success
  bool DropTable(const std::string&); //drop table {table_name}
  TableInfo GetTableInfo(const std::string&); //get the table's information and set the correspongind value of {table_name}
  void WriteTableInfo(const TableInfo&); //write table's information to file

  bool RegisterIndex(const IndexInfo&); //register a new index {index}, return true if success
  bool DropIndex(const std::string&); //drop index {index_name}
  IndexInfo GetIndexInfo(const std::string&); //get the index's information and set the correspongind value of {index_name}
  void WriteIndexInfo(const IndexInfo&); //write index's information to file
};

#endif