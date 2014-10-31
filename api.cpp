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
#include "api.h"
#include "sql_command.h"
#include "catalog_manager.h"
#include "index_manager.h"
#include "table_info.h"
#include "index_info.h"

void API::Switcher(SqlCommand* command){
  SqlCommandType command_type = command->command_type();
  switch(command_type){
    case kSqlInvalid: {Info info("Invalid Command, Please check your syntax."); info.PrintInfo(); break;}
    case kSqlCreateTable: {Info info = CreateTable(command); info.PrintInfo(); break;}
    case kSqlCreateIndex: {Info info = CreateIndex(command); info.PrintInfo(); break;}
    case kSqlDeleteFrom: {Info info = DeleteFrom(command); info.PrintInfo(); break;}
    case kSqlDropTable: {Info info = DropTable(command); info.PrintInfo(); break;}
    case kSqlDropIndex: {Info info = DropIndex(command); info.PrintInfo(); break;}
    case kSqlInsertInto: {Info info = InsertInto(command); info.PrintInfo(); break;}
    case kSqlSelectFrom: {Info info = SelectFrom(command); break;}
  }
}

Info API::CreateTable(SqlCommand* command){
  CatalogManager catalog_manager;
  TableInfo table;
  std::string table_name = command->table_name();
  std::string primary_key = command->primary_key();
  std::vector<std::string> unique_keys = command->unique();
  int attribute_count = 0;
  
  //判断表是否存在
  if(catalog_manager.HasTable(table_name)){
    std::string error_info;
    error_info = "ERROR:Table \"" + table_name + "\" already exists.";
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
        error_info = "ERROR:The type \"" + it->second.first + "\" of attribute" + it->first + "is invalid.";
        return Info(error_info);
      }
      else{
        single_attribute_info.set_type(it->second.first);

        //判断char(n) n是否在1~255的范围内
        if (0<it->second.second<256){
          single_attribute_info.set_length(it->second.second);
        }
        else{
          std::string error_info;
          error_info = "ERROR:The length of char of attribute " + it->first + " must in range of [1,256]";
          return Info(error_info);
        }
      }

      //判断是否是 primary_key
      if(it->first == primary_key){
        single_attribute_info.set_is_primary_key(true);
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

    if(catalog_manager.RegisterTable(table)){
      return Info("Create table success.");
    }
    else{
      return Info("ERROR:Register table failed");
    }
  }
}

Info API::CreateIndex(SqlCommand* command){
  IndexManager index_manager;
  CatalogManager catalog_manager;
  std::string index_name = command->index_name();
  std::string attribute_name = command->column_name();
  std::string table_name = command->table_name();

  if (catalog_manager.HasIndex(index_name)){
    std::string error_info;
    error_info = "ERROR:Index \"" + index_name + "\" already exists.";
    return Info(error_info);
  }
  if (!catalog_manager.HasTable(table_name)){
    std::string error_info;
    error_info = "ERROR:table \"" + table_name + "\" not exists.";
    return Info(error_info);
  }
  else if(!catalog_manager.GetTableInfo(table_name).HasAttribute(attribute_name)){
    std::string error_info;
    error_info = "ERROR:attribute \"" + attribute_name + "\" not exists.";
    return Info(error_info);
  }
  else{
    if(index_manager.CreateIndex(IndexInfo(index_name, table_name, attribute_name))){
      return Info("Create index success.");
    }
    else{
      return Info("ERROR:Create index failed.");
    }
  }
}

Info API::DeleteFrom(SqlCommand* command){
}

Info API::DropTable(SqlCommand* command){
  CatalogManager catalog_manager;
  std::string table_name = command->table_name();

  if (!catalog_manager.HasTable(table_name)){
    std::string error_info;
    error_info = "ERROR:Table \"" + table_name + "\" not exists.";
    return Info(error_info);
  }
  else{
    if (catalog_manager.DropTable(table_name)){
      return Info("Drop table success.");
    }
    else{
      return Info("ERROR:Drop index Failed");
    }
  }
}

Info API::DropIndex(SqlCommand* command){
  CatalogManager catalog_manager;
  std::string index_name = command->index_name();

  if (!catalog_manager.HasIndex(index_name)){
    std::string error_info;
    error_info = "ERROR:Index \"" + index_name + "\" not exists.";
    return Info(error_info);
  }
  else{
    if (catalog_manager.DropIndex(index_name)){
      return Info("Drop index success.");
    }
    else{
      return Info("ERROR:Drop index Failed");
    }
  }

}

Info API::InsertInto(SqlCommand* command){

}

Info API::SelectFrom(SqlCommand* command){

}
