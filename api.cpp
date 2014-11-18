// @copyright (c) 2014 ryecao
// @license: MIT
// @author(s): ryecao/ryecao@gmail.com
// created by ryecao on Sept. 28, 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file:api.cpp
// @brief: API file.
//
// please compile with -std=c++11

#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <iterator>
#include "api.h"
#include "sql_command.h"
#include "catalog_manager.h"
#include "record_manager.h"
#include "index_manager.h"
#include "table_info.h"
#include "index_info.h"
#include <chrono>
#include <cassert>

void API::Switcher(SqlCommand* command){
  SqlCommandType command_type = command->command_type();
  Info info;
  // typedef std::chrono::high_resolution_clock Clock;
  // typedef std::chrono::milliseconds milliseconds;
  // Clock::time_point t0 = Clock::now();
  switch(command_type){
    case kSqlInvalid: { info = Info("Invalid Command, Please check your syntax."); break; }
    case kSqlCreateTable: { info = CreateTable(dynamic_cast<SqlCommandCreateTable *>(command)); break; }
    case kSqlCreateIndex: { info = CreateIndex(dynamic_cast<SqlCommandCreateIndex *>(command)); break; }
    case kSqlDeleteFrom: { info = DeleteFrom(dynamic_cast<SqlCommandDeleteFrom *>(command)); break; }
    case kSqlDropTable: { info = DropTable(dynamic_cast<SqlCommandDropTable *>(command)); break; }
    case kSqlDropIndex: { info = DropIndex(dynamic_cast<SqlCommandDropIndex *>(command)); break; }
    case kSqlInsertInto: { info = InsertInto(dynamic_cast<SqlCommandInsertInto *>(command)); break; }
    case kSqlSelectFrom: { info = SelectFrom(dynamic_cast<SqlCommandSelectFrom *>(command));break; }
  }
  info.PrintInfo();
  // Clock::time_point t1 = Clock::now();
  // milliseconds ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
  // std::cout<<"time used:"<<ms.count()<<" ms."<<std::endl;
}

Info API::CreateTable(SqlCommandCreateTable* command){
  IndexManager index_manager;
  CatalogManager catalog_manager;
  TableInfo table;
  std::string table_name = command->table_name();
  std::string primary_key = command->primary_key();
  std::vector<std::string> unique_keys = command->unique();
  int attribute_count = 0;

  //判断表是否存在
  if(catalog_manager.HasTable(table_name)){
    std::string error_info;
    error_info = "Table \"" + table_name + "\" already exists.";
    return Info(error_info);
  }
  else{
    table.set_table_name(table_name);

    //Check primary key.
    if (primary_key.empty()){
      return Info("Must assign a primary key.");   
    }

    std::map<std::string, std::pair<std::string,int>> attributes = command->attribute();
    for (auto it = attributes.begin(); it != attributes.end(); ++it){
      AttributeInfo single_attribute_info;
      single_attribute_info.set_name(it->first);

      attribute_count++;

      //判断类型是否有效
      if (it->second.first != "char" && it->second.first != "int" && it->second.first != "float"){
        std::string error_info;
        error_info = "The type \"" + it->second.first + "\" of attribute \"" + it->first + "\" is invalid.";
        return Info(error_info);
      }
      else{
        single_attribute_info.set_type(it->second.first);

        //判断char(n) n是否在1~255的范围内
        if (0<it->second.second && it->second.second<256){
          single_attribute_info.set_length(it->second.second);
        }
        else{
          std::string error_info;
          error_info = "The length of char of attribute \"" + it->first + "\" must in range of [1,256].";
          return Info(error_info);
        }
      }

      //判断是否是 primary_key
      if(it->first == primary_key){
        single_attribute_info.set_is_primary_key(true);
        single_attribute_info.add_index("#pri_" + table_name +"_"+it->first);
        index_manager.CreateIndex(it->second.first,it->second.second,IndexInfo("#pri_" + table_name +"_"+it->first,table_name,it->first));
        catalog_manager.RegisterIndex(IndexInfo("#pri_" + table_name +"_"+it->first,table_name,it->first));
      }
      else{
        single_attribute_info.set_is_primary_key(false);
      }

      //判断是否 unique
      if (std::find(unique_keys.begin(), unique_keys.end(), it->first) != unique_keys.end()){
        single_attribute_info.set_is_unique(true);
      }
      else{
        single_attribute_info.set_is_unique(false);
      }

      table.add_attribute(single_attribute_info);
    }
    
    if(!table.HasAttribute(primary_key)){
      std::string error_info;
      error_info = "assigned primary key \"" + primary_key + "\" is not an attribute of the table.";
      return Info(error_info);
    }

    table.set_attribute_number(attribute_count);


    table.set_attribute_names_ordered(command->attribute_names_ordered());
    if(catalog_manager.RegisterTable(table)){
      return Info();
    }
    else{
      return Info("Register table failed.");
    }
  }
}

Info API::CreateIndex(SqlCommandCreateIndex* command){
  IndexManager index_manager;
  CatalogManager catalog_manager;
  RecordManager record_manager;
  std::string index_name = command->index_name();
  std::string attribute_name = command->column_name();
  std::string table_name = command->table_name();

  if (catalog_manager.HasIndex(index_name)){
    std::string error_info;
    error_info = "Index \"" + index_name + "\" already exists.";
    return Info(error_info);
  }
  if (!catalog_manager.HasTable(table_name)){
    std::string error_info;
    error_info = "table \"" + table_name + "\" not exists.";
    return Info(error_info);
  }
  TableInfo table = catalog_manager.GetTableInfo(table_name);

  if(!table.HasAttribute(attribute_name)){
    std::string error_info;
    error_info = "attribute \"" + attribute_name + "\" not exists.";
    return Info(error_info);
  }
  if (!table.attribute(attribute_name).is_unique() && !table.attribute(attribute_name).is_primary_key()){
    std::string error_info;
    error_info = "attribute \"" + attribute_name + "\" is not an unique attribute.";
    return Info(error_info);
  }
  else{
    IndexInfo index_info(index_name, table_name, attribute_name);
    //create index in index manager
    if(index_manager.CreateIndex(table.attribute(attribute_name).type(),table.attribute(attribute_name).length(), index_info)){
      auto all_records = record_manager.SelectAllRecords(table);

      for (auto record : all_records){
          if(!index_manager.AddRecord(table,index_info,record.first.at(table.attribute_index(index_info.attribute_name())),record.second)){
          return Info("Update index failed.");
        }
      }
      if (!catalog_manager.RegisterIndex(index_info))
      {
        return Info("Register Index failed.");
      }
      //update table info in catalog manager
      table.add_index(attribute_name, index_name);
      catalog_manager.WriteTableInfo(table);
      return Info();
    }
    else{
      return Info("Create index failed.");
    }
  }
}

Info API::DeleteFrom(SqlCommandDeleteFrom* command){
  CatalogManager catalog_manager;
  IndexManager index_manager;
  RecordManager record_manager;

  bool delete_all_records = command->delete_all_records();
  std::string table_name = command->table_name();
  std::vector<WhereClause> where_clause = command->where_clause();
  if (!catalog_manager.HasTable(table_name)){
    std::string error_info;
    error_info = "Table \"" + table_name + "\" not exists.";
    return Info(error_info);
  }

  auto table = catalog_manager.GetTableInfo(table_name);

  for (auto it: where_clause){
    if(!table.HasAttribute(it.kColumnName)){
      std::string error_info;
      error_info = "attribute \"" + it.kColumnName + "\"in where clause \""+ it.kColumnName + it.kOperator + it.kCondition +"\" not exists.";
      return Info(error_info);
    }
  }

  if (delete_all_records){
    if(record_manager.DeleteAllRecords(table)){
      //update index
      for (auto it : table.index_names()){
        IndexInfo index_info = catalog_manager.GetIndexInfo(it);
        index_manager.EmptyIndex(index_info);
      }
      return Info();
    }
    else{
      return Info("Delete records failed");
    }
  }
  else{
    std::vector<WhereClause> where_clause_with_index;
    std::vector<WhereClause> where_clause_without_index;

    for (auto it : where_clause){
      if (!table.attribute(it.kColumnName).index_names().empty() && it.kOperator != "!="){
        where_clause_with_index.push_back(it);
      }
      else{
        where_clause_without_index.push_back(it);
      }
    }

    std::vector<std::pair<int,int>> results;

    if (!where_clause_with_index.empty()) //条件里有属性有 index
    {

      for (auto it : where_clause_with_index){
        std::string index_name = table.attribute(it.kColumnName).index_names().at(0);
        IndexInfo index = catalog_manager.GetIndexInfo(index_name);
        std::vector<int> offsets_of_a_clause = index_manager.FindRecords(table,index, it);
        if(offsets_of_a_clause.empty()){
          std::string error_info;
          error_info = "record not found with index, deletion failed";
          return Info(error_info);
        }
        std::sort(offsets_of_a_clause.begin(), offsets_of_a_clause.end());
        offsets_of_a_clause.erase(std::unique(offsets_of_a_clause.begin(), offsets_of_a_clause.end()), offsets_of_a_clause.end());

        std::vector<std::pair<int,int>> results_of_a_clause = record_manager.FindRecordsWithIndex(offsets_of_a_clause, table, it);

        if (results.empty()){
          results = results_of_a_clause;
        }
        else{
          std::vector<std::pair<int,int> > results_temp;
          std::set_intersection(results.begin(),results.end(),
                                results_of_a_clause.begin(),results_of_a_clause.end(),
                                std::back_inserter(results_temp));
          results = results_temp;

        }
      }
      for(auto it : where_clause_without_index){
        if(results.empty()){
          std::string error_info;
          error_info = "record not found with index, deletion failed";
          return Info(error_info);
        }
        results = record_manager.RecordsFilter(results, table, it);
      }
    }
    else{ //条件里属性都没有 index
      for (auto it: where_clause_without_index){
        if (results.empty()){
          results = record_manager.FindRecordsWithNoIndex(table, it);
        }
        else{
          results = record_manager.RecordsFilter(results,table, it);
        }
      }
      if(results.empty()){
        std::string error_info;
        error_info = "record not found with no index, deletion failed";
        return Info(error_info);
      }

    }
    if(results.empty()){
        std::string error_info;
        error_info = "record not found, deletion failed";
        return Info(error_info);
    }
    auto records = record_manager.SelectRecords(results, table);
    int ret = record_manager.DeleteRecords(results, table);
    if(!ret){
      return Info("Delete records failed.");
    }
    else{
      auto index_names = table.index_names();
      int pair_index = 0;
      for (auto it : records){
        for (auto i : index_names){
          IndexInfo index_info = catalog_manager.GetIndexInfo(i);
          auto index = table.attribute_index(index_info.attribute_name());
          std::string value = it.at(index);
          int offset_i = results.at(pair_index).first;
          bool ret = index_manager.DeleteRecord(table, index_info, value, offset_i);
          if(!ret){
            return Info("Delete record in index \"" + index_info.name() + "on attribute \"" + index_info.attribute_name()
                        +"\" of table \"" + index_info.table_name() + "\"failed.");
          }
        }
        pair_index++;
      }
      return Info();
    }
  }
}

Info API::DropTable(SqlCommandDropTable* command){
  CatalogManager catalog_manager;
  std::string table_name = command->table_name();

  if (!catalog_manager.HasTable(table_name)){
    std::string error_info;
    error_info = "Table \"" + table_name + "\" not exists.";
    return Info(error_info);
  }
  else{
    TableInfo table = catalog_manager.GetTableInfo(table_name);
    
    for (auto it : table.index_names()){
      SqlCommandDropIndex* cmd = new SqlCommandDropIndex(it);
        if(!DropIndex(cmd).is_succeed()){
          return Info("Drop table success but index \"" + it +"\" failed to be dropped");
        }
    }

    if (catalog_manager.DropTable(table_name)){
        //drop index
      return Info();
    }
    else{
      return Info("Drop table Failed");
    }
  }
}

Info API::DropIndex(SqlCommandDropIndex* command){
  CatalogManager catalog_manager;
  IndexManager index_manager;
  std::string index_name = command->index_name();

  if (!catalog_manager.HasIndex(index_name)){
    std::string error_info;
    error_info = "Index \"" + index_name + "\" not exists.";
    return Info(error_info);
  }
  else{
    IndexInfo index = catalog_manager.GetIndexInfo(index_name);
    std::string table_name = index.table_name();
    std::string attribute_name = index.attribute_name();
    index_manager.EmptyIndex(index);
    if (catalog_manager.DropIndex(index_name)){
      //update table info
      TableInfo table = catalog_manager.GetTableInfo(table_name);
      table.remove_index(attribute_name,index_name);
      catalog_manager.WriteTableInfo(table);
      return Info();
    }
    else{
      return Info("Drop index Failed");
    }
  }

}

bool CheckUnqiueAndPrimaryKey(const TableInfo& table, const std::vector<std::string>& values){
  IndexManager index_manager;
  RecordManager record_manager;
  std::vector<std::string> attributes_to_be_checked;
  attributes_to_be_checked = table.unique();
  attributes_to_be_checked.push_back(table.primary_key());

  for (auto it : attributes_to_be_checked){
    int attribute_index = table.attribute_index(it);

    if (!table.attribute(it).index_names().empty()){//has index
      std::string index_name = table.attribute(it).index_names().at(0);
      IndexInfo index_info(index_name, table.table_name(), it);
      if (index_manager.FindValue(table,index_info,values.at(attribute_index))){ // has value
        return false;
      }
    }
    else{
      WhereClause where;
      where.kColumnName = it;
      where.kOperator ="=";
      where.kCondition = values.at(attribute_index);
      if (!record_manager.FindRecordsWithNoIndex(table,where).empty()){
        return false;
      }
    }
  }
  return true;
}

bool is_int_overflow_notchecked(std::string s){
  int st=0;
  if(s[0]=='-') st++;
  for(int i=st;i<s.length();i++){
    if( !(s[i]>='0' && s[i]<='9') )
      return false;
  }
  return true;
}

bool is_float_overflow_notchecked(std::string s){
  int st=0;
  if(s[0]=='-') st++;
  bool dot=false;
  for(int i=st;i<s.length();i++){
    if(s[i]=='.' && !dot){
      dot=true;
      continue;
    }
    if( !(s[i]>='0' && s[i]<='9') )
      return false;
  }
  return true;
}

Info API::InsertInto(SqlCommandInsertInto* command){
  CatalogManager catalog_manager;
  RecordManager record_manager;
  IndexManager index_manager;
  int offset;
  std::string table_name = command->table_name();
  auto values = command->values();
  if (!catalog_manager.HasTable(table_name)){
    std::string error_info;
    error_info = "Table \"" + table_name + "\" not exists.";
    return Info(error_info);
  }
  TableInfo table = catalog_manager.GetTableInfo(table_name);
  if (table.attribute_names_ordered().size()!=values.size()){
    return Info("Number of values not equals to the number of attributes");
  }

  auto ano = table.attribute_names_ordered();

  //Check string length and overflow
  for (int i = 0; i<values.size(); ++i){
    int type = table.attribute(ano.at(i)).type();
    int length = table.attribute(ano.at(i)).length();
    if (type == 0){
      if (!is_int_overflow_notchecked(values.at(i))){
        std::string error_info;
        error_info = "Value \"" + values.at(i)  + "\" is not a int number.";
        return Info(error_info);
      }

      try{
        int v = std::stoi(values.at(i));
      }
      catch(const std::out_of_range& e){
        std::string error_info;
        error_info = "Value \"" + values.at(i)  + "\" is out of range of int.";
        return Info(error_info);
      }
    }
    if (type == 1){
      if (!is_float_overflow_notchecked(values.at(i))&&!is_int_overflow_notchecked(values.at(i))){
        std::string error_info;
        error_info = "Value \"" + values.at(i)  + "\" is not a float number.";
        return Info(error_info);
      }

      try{
        float v = std::stof(values.at(i));
      }
      catch(const std::out_of_range& e){
        std::string error_info;
        error_info = "Value \"" + values.at(i)  + "\" is out of range of float.";
        return Info(error_info);
      }
    }
    if (type == 2){
      if (values.at(i).length() > length){
        std::string error_info;
        error_info = "Value \"" + values.at(i)  + "\" is out of range of char("+ std::to_string(length) +").";
        return Info(error_info);
      }
    }

  }

  if (!CheckUnqiueAndPrimaryKey(table,values)){
    return Info("Violation of uniqueness");
  }


  offset = record_manager.InsertRecord(table,values);
  if (offset == -1){
    return Info("Insert failed");
  }

  //update index
  std::vector<std::string> index_names = table.index_names();

  for (auto it : index_names){
    IndexInfo index_info = catalog_manager.GetIndexInfo(it);
    if(!index_manager.AddRecord(table,index_info,values.at(table.attribute_index(index_info.attribute_name())),offset)){
      return Info("Update index failed.");
    }
  }

  return Info();
}

Info API::SelectFrom(SqlCommandSelectFrom* command){
  CatalogManager catalog_manager;
  RecordManager record_manager;
  IndexManager index_manager;

  bool select_all_columns = command->select_all_columns();
  bool select_all_records = command->select_all_records();
  std::vector<std::string> column_names = command->column_names();
  std::string table_name = command->table_name();
  std::vector<WhereClause> where_clause = command->where_clause();
  if (!catalog_manager.HasTable(table_name)){
    std::string error_info;
    error_info = "Table \"" + table_name + "\" not exists.";
    return Info(error_info);
  }

  TableInfo table = catalog_manager.GetTableInfo(table_name);

  if (column_names.empty()){
    column_names = table.attribute_names_ordered();
  }
  else{
    for (auto it : column_names){
      if(!table.HasAttribute(it)){
        std::string error_info;
        error_info = "attribute \"" + it + "\" not exists.";
        return Info(error_info);
      }
    }
  }

  for (auto it: where_clause){
    if(!table.HasAttribute(it.kColumnName)){
      std::string error_info;
      error_info = "attribute \"" + it.kColumnName + "\"in where clause \""+it.kColumnName + it.kOperator + it.kCondition +"\" not exists.";
      return Info(error_info);
    }
  }

  std::vector<std::pair<int,int> > results;
  std::vector<std::vector<std::string> > records;
  if (select_all_records){
    auto records_offset_pairs = record_manager.SelectAllRecords(table);
    for (auto i : records_offset_pairs){
      records.push_back(i.first);
    }
  }
  else{
    std::vector<WhereClause> where_clause_with_index;
    std::vector<WhereClause> where_clause_without_index;

    for (auto it : where_clause){
      if (!table.attribute(it.kColumnName).index_names().empty() && it.kOperator != "!="){
        where_clause_with_index.push_back(it);
      }
      else{
        where_clause_without_index.push_back(it);
      }
    }


    if (!where_clause_with_index.empty()) //条件里有属性有 index
    {
      for (auto it : where_clause_with_index){
        std::string index_name = table.attribute(it.kColumnName).index_names().at(0);
        IndexInfo index = catalog_manager.GetIndexInfo(index_name);
        std::vector<int> offsets_of_a_clause = index_manager.FindRecords(table,index, it);
        std::sort(offsets_of_a_clause.begin(), offsets_of_a_clause.end());
        offsets_of_a_clause.erase(std::unique(offsets_of_a_clause.begin(), offsets_of_a_clause.end()), offsets_of_a_clause.end());
        std::vector<std::pair<int,int>> results_of_a_clause = record_manager.FindRecordsWithIndex(offsets_of_a_clause, table, it);

        if (results.empty()){
          results = results_of_a_clause;
        }
        else{
          std::vector<std::pair<int,int> > results_temp;
          std::set_intersection(results.begin(),results.end(),
                                results_of_a_clause.begin(),results_of_a_clause.end(),
                                std::back_inserter(results_temp));
          results = results_temp;
        }
      }

      for(auto it : where_clause_without_index){
        results = record_manager.RecordsFilter(results, table, it);
      }
    }
    else{ //条件里属性都没有 index
      for (auto it: where_clause_without_index){
        if (results.empty()){
          results = record_manager.FindRecordsWithNoIndex(table, it);
        }
        else{
          results = record_manager.RecordsFilter(results,table, it);
        }
      }
    }
    records = record_manager.SelectRecords(results, table);
  }

  //print table
  for (auto it : column_names){
    std::cout<<"+" << "--------------";
  }

  std::cout<<"+"<<std::endl;
  //表头
  for (auto it : column_names){
    std::cout<<"| " << std::setw(12) << it <<" ";
  }

  std::cout<<"|"<<std::endl;

  for (auto it : column_names){
    std::cout<<"+" << "--------------";
  }
  std::cout<<"+"<<std::endl;

  std::vector<int> index;
  for (auto it: column_names){
    auto attribute_names_ordered = table.attribute_names_ordered();
    auto i = std::find(attribute_names_ordered.begin(), attribute_names_ordered.end(),it);
    int position= std::distance(attribute_names_ordered.begin(),i);
    index.push_back(position);
  }
  //print records
  for (auto it : records){
    for (auto i : index){
      std::cout<<"| "<<std::setw(12) << it.at(i)<<" ";
    }
    std::cout<<"|"<<std::endl;
  }

  for (auto it : column_names){
    std::cout<<"+" << "--------------";
  }

  std::cout<<"+"<<std::endl;
  return Info();
}
