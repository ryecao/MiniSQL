// @copyright (c) 2014 ryecao
// @license: MIT
// @author(s): ryecao/ryecao@gmail.com
// created by ryecao on Sept. 28, 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file:interpreter.cpp
// @brief: Header file of class of sql commands.
//
// please compile with -std=c++11

#ifndef SQL_COMMAND_
#define SQL_COMMAND_

#include <string>
#include "sql_command.h"

//@author: ryecao
//@brief: SQL 语句类型
enum SqlCommandType
{
  kSqlCreateTable = 0,
  kSqlCreateIndex,
  kSqlDeleteFrom,
  kSqlDropTable,
  kSqlDropIndex,
  kSqlExecfile,
  kSqlInsertInto,
  kSqlQuit,
  kSqlSelectFrom,
};

//@auhtor: ryecao
//@brief: SQL 语句类
class SqlCommand
{
public:

private:
  const SqlCommandType command_type_;
  std::string database_name_;
  std::string table_name_;
  std::string coloum_name_;
  std::string index_name_;
};

#endif