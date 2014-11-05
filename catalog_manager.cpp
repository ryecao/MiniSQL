#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include "catalog_manager.h"
#include "table_info.h"
#include "index_info.h"

bool CatalogManager::HasTable(const std::string &table_name){
  std::string file_name;
  file_name = table_name + "_t.cata";
  std::ifstream f(file_name);
  if (f){
    return true;
  }
  else{
    return false;
  }
}

bool CatalogManager::HasIndex(const std::string &index_name){
  std::string file_name;
  file_name = index_name +"_i.cata";
  std::ifstream f(file_name);

  if (f){
    return true;
  }
  else{
    return false;
  }

}

bool CatalogManager::HasAttribute(const std::string &table_name, const std::string &attribute_name){
  if (!HasTable(table_name)){
    return false;
  }

  TableInfo table = GetTableInfo(table_name);

  if (table.HasAttribute(attribute_name)){
    return true;
  }
  else{
    return false;
  }
}

bool CatalogManager::RegisterTable(const TableInfo &table){
  std::string table_name = table.table_name();
  std::string file_name = table_name + "_t.cata";
  std::ofstream file_table_out(file_name, std::ios::binary);

  if (!file_table_out){
    return false;
  }

  file_table_out << table_name << " " << table.attribute_number() <<std::endl;

  auto attribute_names_ordered = table.attribute_names_ordered();
  auto attributes = table.all_attributes();

  for (auto it: attribute_names_ordered){
    file_table_out << it << " " << attributes[it].type() << " " << attributes[it].length() <<" "
                   << attributes[it].is_unique() << " " << attributes[it].is_primary_key() << " ";
    std::vector<std::string> index_names = attributes[it].index_names();

    for (auto &iter : index_names){
      file_table_out<< iter <<" ";
    }
    file_table_out<<std::endl;
  }

  // for (auto it = attributes.begin(); it != attributes.end(); ++it){
  //   file_table_out << it->first << " " << it->second.type() << " " << it->second.length() <<" "
  //                  << it->second.is_unique() << " " << it->second.is_primary_key() << " ";
  //   std::vector<std::string> index_names = it->second.index_names();

  //   for (auto &iter : index_names){
  //     file_table_out<< iter <<" ";
  //   }
  //   file_table_out<<std::endl;
  // }

  file_table_out.close();
  return true;
}

void CatalogManager::WriteTableInfo(const TableInfo& table){
  RegisterTable(table);
}

bool CatalogManager::DropTable(const std::string &table_name){
  std::string file_name = table_name + "_t.cata";
  const char* file_name_cc = file_name.c_str();
  if (std::remove(file_name_cc) == 0){
    return true;
  }
  else{
    return false;
  }
}

TableInfo CatalogManager::GetTableInfo(const std::string &table_name){
  std::string file_name = table_name + "_t.cata";
  std::ifstream file_table_in(file_name, std::ios::binary);

  TableInfo table;
  int attribute_number;
  int type;
  int length;
  bool is_unique;
  bool is_primary_key;
  std::string table_name_in_file;
  std::string attribute_name;
  std::string index_name;
  std::vector<std::string> index_names;
  std::vector<std::string> attribute_names_ordered;
  
  int flag = 0;
  std::string line;
  while(std::getline(file_table_in, line)){
    index_names.clear();
    if (flag == 0){
      std::stringstream line_stream(line);
      line_stream >> table_name_in_file >> attribute_number;
      table.set_table_name(table_name_in_file);
      table.set_attribute_number(attribute_number);
      flag++;
    }
    else{
      std::stringstream line_stream(line);
      line_stream >> attribute_name >> type >> length >> is_unique >> is_primary_key;
      attribute_names_ordered.push_back(attribute_name);
      while(line_stream >> index_name){
        index_names.push_back(index_name);
      }
      table.add_attribute(AttributeInfo(attribute_name, type, length, index_names, is_unique, is_primary_key));
    }
  }

  table.set_attribute_names_ordered(attribute_names_ordered);
  return table;
}

bool CatalogManager::RegisterIndex(const IndexInfo& index){
  std::string index_name = index.name();
  std::string file_name = index_name + "_i.cata";

  std::ofstream file_index_out(file_name, std::ios::binary);
  
  if (!file_index_out){
    return false;
  }
  
  file_index_out << index_name << " " << index.table_name() << " " << index.attribute_name() <<std::endl;
  
  file_index_out.close();  
  return true;
}

bool CatalogManager::DropIndex(const std::string &index_name){
  std::string file_name = index_name + "_i.cata";
  const char *file_name_cc = file_name.c_str();
  if (std::remove(file_name_cc) == 0){
    return true;
  }
  else{
    return false;
  }
}

IndexInfo CatalogManager::GetIndexInfo(const std::string &index_name){
  std::string file_name = index_name + "_i.cata";
  std::ifstream file_index_in(file_name,std::ios::binary);
  std::string index;
  std::string table_name;
  std::string attribute_name;
  file_index_in >> index >> table_name >> attribute_name;

  return IndexInfo(index_name, table_name, attribute_name);  
}

void CatalogManager::WriteIndexInfo(const IndexInfo &index){
  RegisterIndex(index);
}
