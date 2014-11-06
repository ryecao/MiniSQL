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

void API::Switcher(SqlCommand* command){
  SqlCommandType command_type = command->command_type();
  Info info;
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
        index_manager.CreateIndex(IndexInfo("#pri_" + table_name + "_" + it->first,table_name,it->first));
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
  else{
    IndexInfo index_info(index_name, table_name, attribute_name);
    //create index in index manager
    if(index_manager.CreateIndex(index_info)){
      if (!catalog_manager.RegisterIndex(index_info))
      {
        return Info("Register Index failed.");
      }
      //update table info in catalog manager
      table.attribute(attribute_name).add_index(index_name);
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
      if (table.attribute(it.kColumnName).is_primary_key()){
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
        IndexInfo index = catalog_manager.GetIndexInfo(it.kColumnName);
        std::vector<int> offsets_of_a_clause = index_manager.FindRecords(index, it);
        std::vector<std::pair<int,int>> results_of_a_clause = record_manager.FindRecordsWithIndex(offsets_of_a_clause, table, it);
        
        if (results.empty()){
          results = results_of_a_clause;
        }
        else{
          std::set_intersection(results.begin(),results.end(),
                                results_of_a_clause.begin(),results_of_a_clause.end(),
                                std::back_inserter(results));
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

    if(!record_manager.DeleteRecords(results, table)){
      return Info("Delete records failed.");
    }
    else{
      auto records = record_manager.SelectRecords(results, table);
      auto index_names = table.index_names();
      int pair_index = 0;
      for (auto it : records){
        for (auto i : index_names){
          IndexInfo index_info = catalog_manager.GetIndexInfo(i);
          if(!index_manager.DeleteRecord(index_info, it.at(table.attribute_index(i)), results.at(pair_index).first)){
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
    
    if (catalog_manager.DropTable(table_name)){
        //drop index
        for (auto it : table.index_names()){
          SqlCommandDropIndex* cmd = new SqlCommandDropIndex(it);
          if(!DropIndex(cmd).is_succeed()){
            return Info("Drop table success but index \"" + it +"\" failed to be dropped");
          }
        }
      return Info();
    }
    else{
      return Info("Drop table Failed");
    }
  }
}

Info API::DropIndex(SqlCommandDropIndex* command){
  CatalogManager catalog_manager;
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

    if (catalog_manager.DropIndex(index_name)){
      //update table info
      TableInfo table = catalog_manager.GetTableInfo(table_name);
      table.attribute(attribute_name).remove_index(index_name);
      catalog_manager.WriteTableInfo(table);
      return Info();
    }
    else{
      return Info("Drop index Failed");
    }
  }

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

  offset = record_manager.InsertRecord(table,values);
  if (offset == -1){
    return Info("Insert failed");
  }

  //update index
  std::vector<std::string> index_names = table.index_names();

  for (auto it : index_names){
    IndexInfo index_info = catalog_manager.GetIndexInfo(it);
    if(!index_manager.AddRecord(index_info,values.at(table.attribute_index(index_info.attribute_name())),offset)){
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

  for (auto it : column_names){
    if(!table.HasAttribute(it)){
      std::string error_info;
      error_info = "attribute \"" + it + "\" not exists.";
      return Info(error_info);
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
    records = record_manager.SelectRecords(results, table);
  }
  else{
    std::vector<WhereClause> where_clause_with_index;
    std::vector<WhereClause> where_clause_without_index;

    for (auto it : where_clause){
      if (table.attribute(it.kColumnName).is_primary_key()){
        where_clause_with_index.push_back(it);
      }
      else{
        where_clause_without_index.push_back(it);
      }
    }


    if (!where_clause_with_index.empty()) //条件里有属性有 index
    {
      for (auto it : where_clause_with_index){
        IndexInfo index = catalog_manager.GetIndexInfo(it.kColumnName);
        std::vector<int> offsets_of_a_clause = index_manager.FindRecords(index, it);
        std::vector<std::pair<int,int>> results_of_a_clause = record_manager.FindRecordsWithIndex(offsets_of_a_clause, table, it);
        
        if (results.empty()){
          results = results_of_a_clause;
        }
        else{
          std::set_intersection(results.begin(),results.end(),
                                results_of_a_clause.begin(),results_of_a_clause.end(),
                                std::back_inserter(results));
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
    std::cout<<"+ " << std::setw(10) << "-" <<" ";
  }

  std::cout<<"+"<<std::endl;
  //表头
  for (auto it : column_names){
    std::cout<<"| " << std::setw(10) << it <<" ";
  }

  std::cout<<"|"<<std::endl;

  for (auto it : column_names){
    std::cout<<"+ "<< std::setw(10) << "-" <<" ";
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
      std::cout<<"| "<<std::setw(10) << it.at(i)<<" ";
    }
    std::cout<<"|"<<std::endl;
  }

  for (auto it : column_names){
    std::cout<<"+ " << std::setw(10) << "-" <<" ";
  }

  std::cout<<"+"<<std::endl;
  return Info();
}
