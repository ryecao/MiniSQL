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
#include <map>
#include <vector>
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

//@author: ryecao
//@brief: 条件语句结构
struct WhereClause
{
  std::string kColumnName; //列名
  std::string kOperator; //算数比较符
  std::string kCondition; //条件 
};

//@author: ryecao
//@brief: SQL 语句类
class SqlCommand
{
public:
  void set_command_type(SqlCommandType command_type) { command_type_ = command_type; };
  virtual void set_database_name(std::string database_name) { database_name_ = database_name; };
  SqlCommandType command_type() const { return command_type_; };
protected:
  SqlCommandType command_type_;
  std::string database_name_;
};

//@author: ryecao
//@brief: SQL create table 语句类
class SqlCommandCreateTable : public SqlCommand
{
public:
  SqlCommandCreateTable();
  ~SqlCommandCreateTable();
private:
  std::string database_name_;
  std::string table_name_;
  //primary key marked as true
  std::map<std::string, bool> primary_key_;
  //unique attribute as unique
  std::map<std::string, bool> unique_;
  //<列名，<类型,长度>>
  std::map<std::string, std::map<std::string,int> > attribute_;
};

//@author: ryecao
//@brief: SQL create index 语句类
class SqlCommandCreateIndex : public SqlCommand
{
public:
  SqlCommandCreateIndex();
  ~SqlCommandCreateIndex();
private:
  std::string index_name_;
  std::string table_name_;
  std::string coloum_name_;
};

//@author: ryecao
//@brief: SQL delete from 语句类
class SqlCommandDeleteFrom : public SqlCommand
{
public:
  SqlCommandDeleteFrom();
  ~SqlCommandDeleteFrom();
private:
  std::string table_name_;
  std::vector<WhereClause> where_clause_;
};

//@author: ryecao
//@brief: SQL drop table 语句类
class SqlCommandDropTable : public SqlCommand
{
public:
  SqlCommandDropTable();
  ~SqlCommandDropTable();
private:
  std::string table_name_;
};

//@author: ryecao
//@brief: SQL drop index 语句类
class SqlCommandDropIndex : public SqlCommand
{
public:
  SqlCommandDropIndex();
  ~SqlCommandDropIndex();
private:
  std::string index_name_;
  
};

//@author: ryecao
//@brief: SQL select from 语句类
class SqlCommandSelectFrom : public SqlCommand
{
public:
  SqlCommandSelectFrom();
  ~SqlCommandSelectFrom();
public:
  std::string coloum_name_;
  std::string table_name_;
  std::vector<WhereClause> where_clause_;
};

#endif