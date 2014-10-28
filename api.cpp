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
#include "api.h"
#include "sql_command.h"
#include "catalog_manager.h"
#include "index_manager.h"


void API::Switcher(SqlCommand* command){
  SqlCommandType command_type = command->command_type();
  Info info;
  switch(command_type){
    case kSqlInvalid: Info info("Invalid Command, Please check your syntax.");break;
    case kSqlCreateTable: info = CreateTable(command);break;
    case kSqlCreateIndex: info = CreateIndex(command);break;
    case kSqlDeleteFrom: info = DeleteFrom(command);break;
    case kSqlDropTable: info = DropTable(command);break;
    case kSqlDropIndex: info = DropIndex(command);break;
    case kSqlInsertInto: info = InsertInto(command);break;
    case kSqlSelectFrom: info = SelectFrom(command);break;
  }
  if (command_type != kSqlSelectFrom)
  {
    info.PrintInfo();
  }
}

Info API::CreateTable(SqlCommand* command){

}

Info API::CreateIndex(SqlCommand* command){

}

Info API::DeleteFrom(SqlCommand* command){

}

Info API::DropTable(SqlCommand* command){

}

Info API::DropIndex(SqlCommand* command){

}

Info API::InsertInto(SqlCommand* command){

}

Info API::SelectFrom(SqlCommand* command){

}
