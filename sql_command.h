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
  kSqlInvalid,
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
  void set_database_name(std::string& database_name) { database_name_ = database_name; };
  SqlCommandType command_type() const { return command_type_; };
  std::string database_name() const {return database_name_; };
protected:
  SqlCommandType command_type_;
  std::string database_name_;
};

//@author: ryecao
//@brief: SQL create table 语句类
class SqlCommandCreateTable : public SqlCommand
{
public:
  void set_table_name(std::string& table_name) { table_name_ = table_name; };
  void set_primary_key(std::string& key_name) { primary_key_.push_back(key_name); };
  void set_unique(std::string& key_name) { unique_.push_back(key_name); };
  void set_attribute(std::string& column_name, std::string type, std::string char_length)
  {
    int char_length_int = std::stoi(char_length);
    auto pointer_to_an_attribute_pair = std::make_pair(column_name, std::make_pair(type, char_length_int));
    attribute_.insert(pointer_to_an_attribute_pair);
  }
  std::string table_name() const { return  table_name_; };
  std::vector<std::string> primary_key() const { return  primary_key_; };
  std::vector<std::string> unique() const { return unique_; };
  std::map<std::string, std::pair<std::string,int> > attribute() const { return attribute_; };
private:
  std::string table_name_;
  //primary keys
  std::vector<std::string> primary_key_;
  //unique keys
  std::vector<std::string> unique_;
  //<列名，<类型,长度>>
  std::map<std::string, std::pair<std::string,int> > attribute_;
};

//@author: ryecao
//@brief: SQL create index 语句类
class SqlCommandCreateIndex : public SqlCommand
{
public:
  void set_index_name(std::string& index_name){ index_name_ = index_name; };
  void set_table_name(std::string& table_name){ table_name_ = table_name; };
  void set_column_name(std::string& column_name){ column_name_ = column_name; };
  std::string index_name() const { return index_name_; };
  std::string table_name() const { return table_name_; };
  std::string column_name() const { return column_name_; };
private:
  std::string index_name_;
  std::string table_name_;
  std::string column_name_;
};

//@author: ryecao
//@brief: SQL delete from 语句类
class SqlCommandDeleteFrom : public SqlCommand
{
public:
  void set_table_name(std::string& table_name) { table_name_ = table_name; };
  void set_where_clause(WhereClause& where_clause) { where_clause_.push_back(where_clause); };
  void set_delete_all_records(bool delete_all_records) { delete_all_records_ = delete_all_records; };
  bool delete_all_records() const{ return delete_all_records_; };
  std::string table_name() const { return table_name_; };
  std::vector<WhereClause> where_clause() const { return where_clause_; };
private:
  bool delete_all_records_;
  std::string table_name_;
  std::vector<WhereClause> where_clause_;
};

//@author: ryecao
//@brief: SQL drop table 语句类
class SqlCommandDropTable : public SqlCommand
{
public:
  void set_table_name(std::string& table_name) { table_name_ = table_name; };
  std::string table_name() const { return table_name_; };
private:
  std::string table_name_;
};

//@author: ryecao
//@brief: SQL drop index 语句类
class SqlCommandDropIndex : public SqlCommand
{
public:
  void set_index_name(std::string& index_name) { index_name_ = index_name; };
  std::string index_name() const { return index_name_; };
private:
  std::string index_name_;
  
};

//@author: ryecao
//@brief: SQL insert into 语句类
class SqlCommandInsertInto : public SqlCommand
{
public:
  void set_table_name(std::string& table_name){ table_name_ = table_name; };
  void set_values(std::vector<std::string>& values){ values_ = values; };
  std::string table_name() const { return table_name_; };
  std::vector<std::string> values() const { return values_; };
private:
  std::string table_name_;
  std::vector<std::string> values_;
};


//@author: ryecao
//@brief: SQL select from 语句类
class SqlCommandSelectFrom : public SqlCommand
{
public:
  void set_column_names(std::string& column_name){ column_names_.push_back(column_name); };
  void set_table_name(std::string& table_name){ table_name_ = table_name; };
  void set_where_clause(WhereClause& where_clause){ where_clause_.push_back(where_clause); };
  void set_select_all_columns(bool select_all_columns){ select_all_columns_ = select_all_columns; };
  void set_select_all_records(bool select_all_records) { select_all_records_ = select_all_records; };
  bool select_all_records() const{ return select_all_records_; };
  bool select_all_columns() const { return select_all_columns_; };  
  std::vector<std::string> column_names() const { return column_names_; };
  std::string table_name() const { return table_name_; };
  std::vector<WhereClause> where_clause() const { return where_clause_; };
private:
  bool select_all_columns_;
  bool select_all_records_;
  std::vector<std::string> column_names_;
  std::string table_name_;
  std::vector<WhereClause> where_clause_;
};

#endif