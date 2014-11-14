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

#include <string>
#include <vector>
#include "index_manager.h"
#include "table_info.h"
#include "index_info.h"
#include "attribute_type.h"
#include "sql_command.h"
#include "attribute_type.h"
#include "b_plus_tree.h"

bool IndexManager::CreateIndex(std::string type, int size, const IndexInfo &index){
  std::string file_name = index.name() + ".index";
  int type_i;
  if (type == "int"){
    type_i = 0;
  }
  else if(type == "float"){
    type_i = 1;
  }
  else if(type == "char"){
    type_i = 2;
  }

  IndexTypeInfo index_type_info(size, type_i, size);
  B_Plus_Tree::BPTree bpt(file_name, index_type_info);
  return true;
}

bool IndexManager::CreateIndex(int type, int size, const IndexInfo &index){
  std::string file_name = index.name() + ".index";
  IndexTypeInfo index_type_info(size, type, size);
  B_Plus_Tree::BPTree bpt(file_name, index_type_info);
  return true;
}

bool IndexManager::AddRecord(const TableInfo &table, const IndexInfo &index, std::string value, int offset){
  int value_i;
  float value_f;
  int type = table.attribute(index.attribute_name()).type();
  std::string file_name = index.name() + ".index";
  B_Plus_Tree::BPTree bpt(file_name);

  if (type == 0){
    value_i = std::stoi(value);
    AttrType attr_type(value_i);
    bpt.insert(attr_type,offset);
  }
  else if( type == 1){
    value_f = std::stof(value);
    AttrType attr_type(value_i);
    bpt.insert(attr_type,offset);
  }
  else{
    AttrType attr_type(value);
    bpt.insert(attr_type,offset);
  }

  return true;
}

bool IndexManager::DeleteRecord(const TableInfo &table, const IndexInfo &index, std::string value, int offset){
  int value_i;
  float value_f;
  int type = table.attribute(index.attribute_name()).type();
  std::string file_name = index.name() + ".index";
  B_Plus_Tree::BPTree bpt(file_name);

  if (type == 0){
    value_i = std::stoi(value);
    AttrType attr_type(value_i);
    bpt.erase(attr_type);
  }
  else if( type == 1){
    value_f = std::stof(value);
    AttrType attr_type(value_i);
    bpt.erase(attr_type);
  }
  else{
    AttrType attr_type(value);
    bpt.erase(attr_type);
  }

  return true;

}

bool IndexManager::EmptyIndex(const IndexInfo &index){
  std::string file_name = index.name() + ".index";
  B_Plus_Tree::BPTree bpt(file_name);

  bpt.deleteAll();
  return true;  
}

bool IndexManager::FindValue(const TableInfo &table, const IndexInfo& index, std::string value){
  std::string file_name = index.name() + ".index";
  B_Plus_Tree::BPTree bpt(file_name);
  int res;
  int type = table.attribute(index.attribute_name()).type();
  if (type == 0){
    res = bpt.find(AttrType(std::stoi(value)));
  }
  else if(type == 1){
    res = bpt.find(AttrType(std::stof(value)));
  }
  else{
    res = bpt.find(AttrType(value));
  }

  if (res == -1){
    return false;
  }
  else{
    return true;
  }
}

std::vector<int> IndexManager::FindRecords(const TableInfo &table, const IndexInfo& index, const WhereClause& where_clause){
  std::string file_name = index.name() + ".index";
  B_Plus_Tree::BPTree bpt(file_name);
  int type = table.attribute(index.attribute_name()).type();
  std::vector<int> res;
  std::string attribute_name = index.attribute_name();
  std::string condition = where_clause.kCondition;
  std::string op = where_clause.kOperator;

  if (op == "<"){
    if (type == 0){
      res = bpt.getAllLess(AttrType(std::stoi(condition)));
    }
    else if(type == 1){
      res = bpt.getAllLess(AttrType(std::stof(condition)));
    }
    else{
      res = bpt.getAllLess(AttrType(condition));
    }
  }
  else if (op =="<="){
    if (type == 0){
      res = bpt.getAllLessOrEqual(AttrType(std::stoi(condition)));
    }
    else if(type == 1){
      res = bpt.getAllLessOrEqual(AttrType(std::stof(condition)));
    }
    else{
      res = bpt.getAllLessOrEqual(AttrType(condition));
    }
  }
  else if (op == "="){
    if (type == 0){
      res.push_back(bpt.find(AttrType(std::stoi(condition))));
    }
    else if(type == 1){
      res.push_back(bpt.find(AttrType(std::stof(condition))));
    }
    else{
      res.push_back(bpt.find(AttrType(condition)));
    }
  }
  else if (op == ">="){
    if (type == 0){
      res = bpt.getAllGreaterOrEqual(AttrType(std::stoi(condition)));
    }
    else if(type == 1){
      res = bpt.getAllGreaterOrEqual(AttrType(std::stof(condition)));
    }
    else{
      res = bpt.getAllGreaterOrEqual(AttrType(condition));
    }
  }
  else if (op == ">"){
    if (type == 0){
      res = bpt.getAllGreater(AttrType(std::stoi(condition)));
    }
    else if(type == 1){
      res = bpt.getAllGreater(AttrType(std::stof(condition)));
    }
    else{
      res = bpt.getAllGreater(AttrType(condition));
    }
  }

  return res;
}
