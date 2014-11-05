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
  SqlCommand(){};
  SqlCommand(SqlCommandType sct){command_type_ = sct;};
  void set_command_type(SqlCommandType command_type) { command_type_ = command_type; };
  void set_database_name(std::string& database_name) { database_name_ = database_name; };
  SqlCommandType command_type() const { return command_type_; };
  std::string database_name() const {return database_name_; };

  virtual std::string table_name() const = 0;
  virtual std::string index_name() const = 0;
  virtual std::string column_name() const = 0;
  virtual std::string primary_key() const = 0;
  virtual std::vector<std::string> unique() const = 0;
  virtual std::map<std::string, std::pair<std::string,int>> attribute() const = 0;
  virtual std::vector<std::string> attribute_names_ordered() const = 0;
protected:
  SqlCommandType command_type_;
  std::string database_name_;
};

//@author: ryecao
//@brief: SQL create table 语句类
class SqlCommandCreateTable : public SqlCommand
{
public:
  SqlCommandCreateTable(){};
  SqlCommandCreateTable(const SqlCommandCreateTable& ct){
    SqlCommand::set_command_type(ct.command_type());
    table_name_=ct.table_name();primary_key_=ct.primary_key();
    unique_=ct.unique();attribute_=ct.attribute();
    attribute_names_ordered_=ct.attribute_names_ordered();
  };
  void set_table_name(std::string& table_name) { table_name_ = table_name; };
  void set_primary_key(std::string& key_name) { primary_key_ = key_name; };
  void set_unique(std::string& key_name) { unique_.push_back(key_name); };
  void set_attribute_names_ordered(std::string& attribute_name){attribute_names_ordered_.push_back(attribute_name);};
  void set_attribute(std::string& column_name, std::string type, std::string char_length)
  {
    int char_length_int = std::stoi(char_length);
    auto pointer_to_an_attribute_pair = std::make_pair(column_name, std::make_pair(type, char_length_int));
    attribute_.insert(pointer_to_an_attribute_pair);
  }
  virtual std::string index_name() const{};
  virtual std::string column_name() const{};
  
  virtual std::string table_name() const { return  table_name_; };
  virtual std::string primary_key() const { return  primary_key_; };
  virtual std::vector<std::string> unique() const { return unique_; };
  virtual std::map<std::string, std::pair<std::string,int> > attribute() const { return attribute_; };
  virtual std::vector<std::string> attribute_names_ordered() const {return attribute_names_ordered_;};
private:
  std::string table_name_;
  //primary key
  std::string primary_key_;
  //unique keys
  std::vector<std::string> unique_;
  //<列名，<类型,长度>>
  std::map<std::string, std::pair<std::string,int> > attribute_;
  std::vector<std::string> attribute_names_ordered_;
};

//@author: ryecao
//@brief: SQL create index 语句类
class SqlCommandCreateIndex : public SqlCommand
{
public:
  SqlCommandCreateIndex(){};
  SqlCommandCreateIndex(const SqlCommandCreateIndex& ci){
    SqlCommand::set_command_type(ci.command_type());
    index_name_=ci.index_name();
    table_name_=ci.table_name();
    column_name_=ci.column_name();
  };
  void set_index_name(std::string& index_name){ index_name_ = index_name; };
  void set_table_name(std::string& table_name){ table_name_ = table_name; };
  void set_column_name(std::string& column_name){ column_name_ = column_name; };

  virtual std::string primary_key() const{};
  virtual std::vector<std::string> unique() const{};
  virtual std::map<std::string, std::pair<std::string,int>> attribute() const{};
  virtual std::vector<std::string> attribute_names_ordered() const{};

  virtual std::string index_name() const { return index_name_; };
  virtual std::string table_name() const { return table_name_; };
  virtual std::string column_name() const { return column_name_; };
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
  SqlCommandDeleteFrom(){};
  SqlCommandDeleteFrom(const SqlCommandDeleteFrom& df){
    SqlCommand::set_command_type(df.command_type());
    delete_all_records_=df.delete_all_records();
    table_name_=df.table_name();
    where_clause_=df.where_clause();
  };
  void set_table_name(std::string& table_name) { table_name_ = table_name; };
  void set_where_clause(WhereClause& where_clause) { where_clause_.push_back(where_clause); };
  void set_delete_all_records(bool delete_all_records) { delete_all_records_ = delete_all_records; };
  
  virtual std::string index_name() const{};
  virtual std::string column_name() const{};
  virtual std::string primary_key() const{};
  virtual std::vector<std::string> unique() const{};
  virtual std::map<std::string, std::pair<std::string,int>> attribute() const{};
  virtual std::vector<std::string> attribute_names_ordered() const{};

  bool delete_all_records() const{ return delete_all_records_; };
  virtual std::string table_name() const { return table_name_; };
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
  SqlCommandDropTable(){};
  SqlCommandDropTable(const SqlCommandDropTable& dt){
    SqlCommand::set_command_type(dt.command_type());
    table_name_=dt.table_name();
  };

  void set_table_name(std::string& table_name) { table_name_ = table_name; };

  virtual std::string index_name() const{};
  virtual std::string column_name() const{};
  virtual std::string primary_key() const{};
  virtual std::vector<std::string> unique() const{};
  virtual std::map<std::string, std::pair<std::string,int>> attribute() const{};
  virtual std::vector<std::string> attribute_names_ordered() const{};

  virtual std::string table_name() const { return table_name_; };
private:
  std::string table_name_;
};

//@author: ryecao
//@brief: SQL drop index 语句类
class SqlCommandDropIndex : public SqlCommand
{
public:
  SqlCommandDropIndex(){};
  SqlCommandDropIndex(const SqlCommandDropIndex& di){
    SqlCommand::set_command_type(di.command_type());
    index_name_=di.index_name();
  };

  void set_index_name(std::string& index_name) { index_name_ = index_name; };

  virtual std::string table_name() const{};
  virtual std::string column_name() const{};
  virtual std::string primary_key() const{};
  virtual std::vector<std::string> unique() const{};
  virtual std::map<std::string, std::pair<std::string,int>> attribute() const{};
  virtual std::vector<std::string> attribute_names_ordered() const{};

  virtual std::string index_name() const { return index_name_; };
private:
  std::string index_name_;
  
};

class SqlCommandExecfile : public SqlCommand
{
public:
  SqlCommandExecfile(){SqlCommand::set_command_type(kSqlExecfile);};
  SqlCommandExecfile(const SqlCommandExecfile& e){
    SqlCommand::set_command_type(e.command_type());
  };
  virtual std::string table_name() const{};
  virtual std::string column_name() const{};
  virtual std::string primary_key() const{};
  virtual std::vector<std::string> unique() const{};
  virtual std::map<std::string, std::pair<std::string,int>> attribute() const{};
  virtual std::vector<std::string> attribute_names_ordered() const{};
  virtual std::string index_name() const {};

};
//@author: ryecao
//@brief: SQL insert into 语句类
class SqlCommandInsertInto : public SqlCommand
{
public:
  SqlCommandInsertInto(){};
  SqlCommandInsertInto(const SqlCommandInsertInto& ii){
    SqlCommand::set_command_type(ii.command_type());
    table_name_=ii.table_name();
    values_=ii.values();
  };

  void set_table_name(std::string& table_name){ table_name_ = table_name; };
  void set_values(std::vector<std::string>& values){ values_ = values; };

  virtual std::string index_name() const{};
  virtual std::string column_name() const{};
  virtual std::string primary_key() const{};
  virtual std::vector<std::string> unique() const{};
  virtual std::map<std::string, std::pair<std::string,int>> attribute() const{};
  virtual std::vector<std::string> attribute_names_ordered() const{};

  virtual std::string table_name() const { return table_name_; };
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
  SqlCommandSelectFrom(){};
  SqlCommandSelectFrom(const SqlCommandSelectFrom& sf){
    SqlCommand::set_command_type(sf.command_type());
    select_all_columns_=sf.select_all_columns();
    select_all_records_=sf.select_all_records();
    column_names_=sf.column_names();
    table_name_=sf.table_name();
    where_clause_=sf.where_clause();
  };

  void set_column_names(std::string& column_name){ column_names_.push_back(column_name); };
  void set_table_name(std::string& table_name){ table_name_ = table_name; };
  void set_where_clause(WhereClause& where_clause){ where_clause_.push_back(where_clause); };
  void set_select_all_columns(bool select_all_columns){ select_all_columns_ = select_all_columns; };
  void set_select_all_records(bool select_all_records) { select_all_records_ = select_all_records; };

  virtual std::string index_name() const{};
  virtual std::string column_name() const{};
  virtual std::string primary_key() const{};
  virtual std::vector<std::string> unique() const{};
  virtual std::map<std::string, std::pair<std::string,int>> attribute() const{};
  virtual std::vector<std::string> attribute_names_ordered() const{};

  virtual bool select_all_records() const{ return select_all_records_; };
  virtual bool select_all_columns() const { return select_all_columns_; };  
  virtual std::vector<std::string> column_names() const { return column_names_; };
  virtual std::string table_name() const { return table_name_; };
  virtual std::vector<WhereClause> where_clause() const { return where_clause_; };
private:
  bool select_all_columns_;
  bool select_all_records_;
  std::vector<std::string> column_names_;
  std::string table_name_;
  std::vector<WhereClause> where_clause_;
};

#endif