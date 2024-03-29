// @copyright (c) 2014 ryecao
// @license: MIT
// @author(s): ryecao/ryecao@gmail.com
// created by ryecao on Sept. 28, 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file:interpreter.cpp
// @brief: Header file of an interpreter used for reading in SQL command users type in, check
// the rightness and convert it into inner format used by other module of the MiniSQL
// database
//
// please compile with -std=c++11

#include "interpreter.h"
#include "sql_command.h"
#include "api.h"
#include <string>
#include <set>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

//@author: ryecao
//@brief: 构造函数
Interpreter::Interpreter(){
  sql_command = {"create table","create index","delete from","drop table","drop index","execfile",
                 "insert into","quit","select"};
}

//@author: ryecao
//@brief: 把命令转换为小写
//@params: raw_string: 转换为小写之前的字符串
//@return: raw_string: 转换为小写后的字符串
std::string Interpreter::LowerCase(std::string& raw_string){
  std::transform(raw_string.begin(), raw_string.end(), raw_string.begin(), ::tolower);
  return raw_string;
}

//@author: ryecao
//@brief: 用自定义的字符串，替换某字符串中的某一指定部分
//@params: str: 需要更改的字符串,
//         from: 需要更改的部分,
//         to: 需要替换成的部分.
//@return: 修改后的字符串
//@example: str: "this is an example"
//          from: "is"
//          to: "was"
//          return: "this was an example"
std::string Interpreter::ReplacePartInString(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return str;
}


//@author: ryecao
//@brief: 除去命令类型后，对命令内容预处理
//@params: command_content：处理之前的命令内容
//@reutrn: command_content：处理之后的命令内容
std::string Interpreter::CommandContentPreProcess(std::string& command_content){
  ReplacePartInString(command_content, "*"," * ");
  ReplacePartInString(command_content, "("," ( ");
  ReplacePartInString(command_content, ")"," ) ");
  ReplacePartInString(command_content, ";"," ; ");
  ReplacePartInString(command_content, ","," , ");
  ReplacePartInString(command_content, "'"," ' ");
  ReplacePartInString(command_content, ">="," >= ");
  ReplacePartInString(command_content, "<="," <= ");
  ReplacePartInString(command_content, "!="," != ");
  ReplacePartInString(command_content, ">"," > ");
  ReplacePartInString(command_content, "="," = ");
  ReplacePartInString(command_content, "<"," < ");
  ReplacePartInString(command_content, ">  ="," >= ");
  ReplacePartInString(command_content, "<  ="," <= ");
  ReplacePartInString(command_content, "! ="," != ");
  
  return command_content;
}

//@author: ryecao
//@brief: 根据 command_type 选择相应的语句生成器
SqlCommand* Interpreter::SelectSqlCommand(std::string& command_type, std::string& command_content){
  SqlCommand* sql_command = NULL;
  if (command_type == "insert into"){
    SqlCommandInsertInto sql_command_insert_into;
    sql_command_insert_into = SqlInsertInto(command_content);
    sql_command = new SqlCommandInsertInto(sql_command_insert_into);
  }
  else if (command_type == "delete from"){
    SqlCommandDeleteFrom sql_command_delete_from;
    sql_command_delete_from = SqlDeleteFrom(command_content);
    sql_command = new SqlCommandDeleteFrom(sql_command_delete_from);
  }
  else if (command_type == "select"){
    SqlCommandSelectFrom sql_command_select_from;
    sql_command_select_from = SqlSelectFrom(command_content);
    sql_command = new SqlCommandSelectFrom(sql_command_select_from); 
  }
  else if (command_type == "create table"){
    SqlCommandCreateTable sql_command_create_table;
    sql_command_create_table = SqlCreateTable(command_content);
    sql_command = new SqlCommandCreateTable(sql_command_create_table);
  }
  else if (command_type == "create index"){
    SqlCommandCreateIndex sql_command_create_index;
    sql_command_create_index = SqlCreateIndex(command_content);
    sql_command = new SqlCommandCreateIndex(sql_command_create_index);
  }
  else if (command_type == "drop table"){
    SqlCommandDropTable sql_command_drop_table;
    sql_command_drop_table = SqlDropTable(command_content);
    sql_command = new SqlCommandDropTable(sql_command_drop_table);
  }
  else if (command_type == "drop index"){
    SqlCommandDropIndex sql_command_drop_index;
    sql_command_drop_index = SqlDropIndex(command_content);
    sql_command = new SqlCommandDropIndex(sql_command_drop_index);
  }
  else if (command_type == "execfile"){
    SqlExecfile(command_content);
    sql_command = new SqlCommandExecfile();
  }
  else if (command_type == "quit" || command_type == "quit;"){
    SqlQuit(command_content);
  }

  return sql_command;
}

//@author: ryecao
//@breif: 读入
//@return: 命令名是否合法
SqlCommand* Interpreter::ReadInput(){
  std::string first_word;
  std::string second_word;
  std::string command_type("");
  std::string command_content("");
  bool is_command_type_valid;

  std::cin >> first_word;
  LowerCase(first_word);

  command_type = first_word;
  if (first_word == "quit;"){
    std::exit(1);
  }
  if (first_word.back()==';'){
      is_command_type_valid = false;
      SqlCommand* sc = NULL;
      SqlCommandCreateTable sct;
      sct.set_command_type(kSqlInvalid);
      sc = new SqlCommandCreateTable(sct);
      return sc;
  }
  if(first_word != "select" && first_word != "quit" && first_word != "quit;" && first_word != "execfile" ){
    std::cin >> second_word;
    LowerCase(second_word);

    command_type = first_word + " " +second_word;
    const bool is_in_sql_command = sql_command.find(command_type) != sql_command.end();

    if (is_in_sql_command == 0){
      is_command_type_valid = false;
      SqlCommand* sc = NULL;
      SqlCommandCreateTable sct;
      sct.set_command_type(kSqlInvalid);
      sc = new SqlCommandCreateTable(sct);
      return sc;
    }

    is_command_type_valid = true;
  }
  else if (first_word == "select" || first_word == "quit" || first_word == "quit;" || first_word == "execfile"){
    if (first_word =="quit" || first_word =="quit;"){
      std::exit(1);
    }
    is_command_type_valid = true;
  }

  if (is_command_type_valid)
  {
    bool flag_read_not_finish = true;
    std::string temp_word;

    while(flag_read_not_finish){
      std::cin >> temp_word;
      if (temp_word.back() ==';'){
        flag_read_not_finish = false;
      }
      command_content = command_content + " " +temp_word;
    }
    SqlCommand* sql_command_pointer = NULL;

    sql_command_pointer = SelectSqlCommand(command_type, command_content);
    return sql_command_pointer;
  }
  SqlCommand* sc = NULL;
  SqlCommandCreateTable sct;
  sct.set_command_type(kSqlInvalid);
  sc = new SqlCommandCreateTable(sct);
  return sc;
}

SqlCommand* Interpreter::ReadInput(std::stringstream& read_command_stream){
  std::string first_word;
  std::string second_word;
  std::string command_type("");
  std::string command_content("");
  bool is_command_type_valid;

  read_command_stream >> first_word;
  LowerCase(first_word);

  command_type = first_word;
  if (first_word == "quit;"){
    std::exit(1);
  }
  if (first_word.back()==';'){
      is_command_type_valid = false;
      SqlCommand* sc = NULL;
      SqlCommandCreateTable sct;
      sct.set_command_type(kSqlInvalid);
      sc = new SqlCommandCreateTable(sct);
      return sc;
  }
  if(first_word != "select" && first_word != "quit" && first_word != "quit;" && first_word != "execfile" ){
    read_command_stream >> second_word;
    LowerCase(second_word);

    command_type = first_word + " " +second_word;
    const bool is_in_sql_command = sql_command.find(command_type) != sql_command.end();

    if (is_in_sql_command == 0){
      is_command_type_valid = false;
      SqlCommand* sc = NULL;
      SqlCommandCreateTable sct;
      sct.set_command_type(kSqlInvalid);
      sc = new SqlCommandCreateTable(sct);
      return sc;
    }
    is_command_type_valid = true;
  }
  else if (first_word == "select" || first_word == "quit" || first_word == "quit;" || first_word == "execfile")
  {
    if (first_word =="quit" || first_word =="quit;"){
      std::exit(1);
    }    
    is_command_type_valid = true;
  }

  if (is_command_type_valid)
  {
    bool flag_read_not_finish = true;
    std::string temp_word;

    while(flag_read_not_finish){
      read_command_stream >> temp_word;
      if (temp_word.back() ==';'){
        flag_read_not_finish = false;
      }
      command_content = command_content + " " +temp_word;
    }
    SqlCommand* sql_command_pointer;
    sql_command_pointer = SelectSqlCommand(command_type, command_content);
    return sql_command_pointer;
  }
  is_command_type_valid = false;
  SqlCommand* sc = NULL;
  SqlCommandCreateTable sct;
  sct.set_command_type(kSqlInvalid);
  sc = new SqlCommandCreateTable(sct);
  return sc;
}


//@author: ryecao
//@brief: 检查 create table 语句合法性，创建相应语句对象
SqlCommandCreateTable Interpreter::SqlCreateTable(std::string& command){
  SqlCommandCreateTable create_table_command;
  create_table_command.set_command_type(kSqlCreateTable);
  CommandContentPreProcess(command);
  std::stringstream command_stream(command);
  std::string word;
  command_stream >> word; // 第一个word是table_name

  create_table_command.set_table_name(word);
  
  command_stream >> word; // 第二个应是一个 "("
  if (word!="("){
    create_table_command.set_command_type(kSqlInvalid);
    return create_table_command;
  }

  std::vector<std::string> column_names_in;
  std::string column_name,type,char_length;
  std::string first_word, second_word;
  while(word!=";"){
    command_stream >> first_word;
    command_stream >> second_word;
    if (first_word !="primary" && second_word!="key"){
      column_name = first_word;
      type = second_word;

      
      for (auto it:column_names_in){
        if (it == column_name){
          create_table_command.set_command_type(kSqlInvalid);
          return create_table_command;
        }
      }
      column_names_in.push_back(column_name);

      create_table_command.set_attribute_names_ordered(column_name);

      if (type != "char"){
        if(type == "int"){
          create_table_command.set_attribute(column_name,type,sizeof(int));
        }
        else if(type == "float"){
          create_table_command.set_attribute(column_name,type,sizeof(float));
        }
        else{
          std::cout<<"Error: \""<<type<<"\" is not a valid type"<<std::endl;
          create_table_command.set_command_type(kSqlInvalid);
          return create_table_command;
        }
      }
      else{
        command_stream >> word;
        if (word !="("){

          create_table_command.set_command_type(kSqlInvalid);
          return create_table_command;
        }
        command_stream >> char_length;

        command_stream >> word;
        if (word !=")"){

          create_table_command.set_command_type(kSqlInvalid);
          return create_table_command;
        }
        create_table_command.set_attribute(column_name,type,char_length);
      }

      command_stream >> word;

      if (word ==")"){
        break;
      }
      else if (word != ","){
        if (LowerCase(word)=="unique")
        {
          create_table_command.set_unique(column_name);
          command_stream >> word;
        }
        else{

          create_table_command.set_command_type(kSqlInvalid);
          return create_table_command;
        }
      }
    }
    else{
      command_stream >> word;
      if (word !="("){

        create_table_command.set_command_type(kSqlInvalid);
        return create_table_command;
      }

      command_stream >> word;
      create_table_command.set_primary_key(word);

      command_stream >> word;
      if (word !=")"){

        create_table_command.set_command_type(kSqlInvalid);
        return create_table_command;
      }

      command_stream >> word;
      if (word ==")"){
        break;
      }
      else if (word != ",")
      {

        create_table_command.set_command_type(kSqlInvalid);
        return create_table_command;
      }

    }
  }

  return create_table_command; 
}

//@author: ryecao
//@brief: 检查 create index 语句合法性，创建相应语句对象
SqlCommandCreateIndex Interpreter::SqlCreateIndex(std::string& command){
  SqlCommandCreateIndex create_index_command;
  create_index_command.set_command_type(kSqlCreateIndex);
  CommandContentPreProcess(command);
  std::stringstream command_stream(command);
  std::string word;

  command_stream >> word;
  create_index_command.set_index_name(word);

  command_stream >> word;
  if(LowerCase(word) != "on"){
    create_index_command.set_command_type(kSqlInvalid);
    return create_index_command;
  }

  command_stream >> word;
  create_index_command.set_table_name(word);

  command_stream >> word;
  if (word != "("){
    create_index_command.set_command_type(kSqlInvalid);
    return create_index_command;
  }

  command_stream >> word;
  create_index_command.set_column_name(word);

  command_stream >> word;
  if (word != ")"){
    create_index_command.set_command_type(kSqlInvalid);
    return create_index_command;
  }

  command_stream >> word;
  if (word != ";"){
    create_index_command.set_command_type(kSqlInvalid);
    return create_index_command;
  }

  return create_index_command;
}

bool is_float(std::string s){
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

inline bool is_number(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p ;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0)||is_float(s) ;
}


//@author: ryecao
//@brief: 检查 delete from 语句合法性，创建相应语句对象
SqlCommandDeleteFrom Interpreter::SqlDeleteFrom(std::string& command){
  SqlCommandDeleteFrom delete_from_command;
  delete_from_command.set_command_type(kSqlDeleteFrom);
  CommandContentPreProcess(command);
  std::stringstream command_stream(command);
  std::string word;

  command_stream >> word;
  delete_from_command.set_table_name(word);


  command_stream >> word;
  if (word ==";"){
    delete_from_command.set_delete_all_records(true);
    return delete_from_command;
  }
  else{
    delete_from_command.set_delete_all_records(false);
    std::set<std::string> WhereClauseOperators = {"<","<=","=",">=",">","!="};
    WhereClause clause;

    command_stream >> word;
    while(word!=";"){
      clause.kColumnName = word;

      command_stream >> word;
      if (WhereClauseOperators.find(word) != WhereClauseOperators.end()){
        clause.kOperator = word;
      }
      else{
        delete_from_command.set_command_type(kSqlInvalid);
        return delete_from_command;
      }

      command_stream >> word;

      bool number_flag;

      number_flag = is_number(word);

      if (word != "'" && !number_flag){
        delete_from_command.set_command_type(kSqlInvalid);
        return delete_from_command;
      }

      if (!number_flag){
        command_stream >> word;
      }
      clause.kCondition = word;

      if (!number_flag)
      {
        command_stream >> word;
      }
      if (word != "'"&&!number_flag){
        delete_from_command.set_command_type(kSqlInvalid);
        return delete_from_command;
      }

      delete_from_command.set_where_clause(clause);

      command_stream >> word;

      if (word ==";"){
        break;
      }
      else if (LowerCase(word) != "and"){
        delete_from_command.set_command_type(kSqlInvalid);
        return delete_from_command;
      }

      command_stream >> word;
    }
  }

  return delete_from_command;  
}

//@author: ryecao
//@brief: 检查 drop table 语句合法性，创建相应语句对象
SqlCommandDropTable Interpreter::SqlDropTable(std::string& command){
  SqlCommandDropTable drop_table_command;
  drop_table_command.set_command_type(kSqlDropTable);
  CommandContentPreProcess(command);
  std::stringstream command_stream(command);
  std::string word;

  command_stream >> word;
  drop_table_command.set_table_name(word);

  command_stream >> word;
  if(word != ";"){
    drop_table_command.set_command_type(kSqlInvalid);
    return drop_table_command;
  }

  return drop_table_command;
}

//@author: ryecao
//@brief: 检查 drop index 语句合法性，创建相应语句对象
SqlCommandDropIndex Interpreter::SqlDropIndex(std::string& command){
  SqlCommandDropIndex drop_index_command;
  drop_index_command.set_command_type(kSqlDropIndex);
  CommandContentPreProcess(command);
  std::stringstream command_stream(command);
  std::string word;

  command_stream >> word;
  drop_index_command.set_index_name(word);

  command_stream >> word;
  if(word != ";"){
    drop_index_command.set_command_type(kSqlInvalid);
    return drop_index_command;
  }

  return drop_index_command;
}

//@author: ryecao
//@brief: 检查 execfile 语句合法性.  
void Interpreter::SqlExecfile(std::string& command){
  CommandContentPreProcess(command);
  std::stringstream command_stream(command);
  std::string filename;
  char read_buffer[256];
  command_stream >> filename;

  std::ifstream file(filename);
  API api;
  if (!file.is_open()){
    std::cout<<"File could not be opened. Please check your input."<<std::endl;
  }
  else{
    while(!file.eof()){
      file.getline(read_buffer,256);
      std::stringstream read_command_stream(read_buffer);

      api.Switcher(ReadInput(read_command_stream));      
    }
  }
  file.close();
  //std::exit(1);
}

//@author: ryecao
//@brief: 检查 insert into 语句合法性，创建相应语句对象
SqlCommandInsertInto Interpreter::SqlInsertInto(std::string& command){
  SqlCommandInsertInto insert_into_command;
  insert_into_command.set_command_type(kSqlInsertInto);
  CommandContentPreProcess(command);
  std::stringstream command_stream(command);
  std::string word;

  command_stream >> word;
  insert_into_command.set_table_name(word);
  
  command_stream >> word;
  if (LowerCase(word) != "values"){
    insert_into_command.set_command_type(kSqlInvalid);
    return insert_into_command;
  }

  command_stream >> word;
  if (word != "("){
    insert_into_command.set_command_type(kSqlInvalid);
    return insert_into_command;
  }

  command_stream >> word;

  std::vector<std::string> insert_values;

  while(word != ")"){
    bool number_flag;
    number_flag = is_number(word);

    if (word != "'"&& !number_flag){
      insert_into_command.set_command_type(kSqlInvalid);
      return insert_into_command;
    }
    
    if (!number_flag){
      command_stream >> word;
    }
    insert_values.push_back(word);
    if (!number_flag){
      command_stream >> word;
    }
    if (word !="'" && !number_flag){
      insert_into_command.set_command_type(kSqlInvalid);
      return insert_into_command;
    }
    
    command_stream >> word;
    if (word == ")"){
      break;
    }
    else if (word !=","){
      insert_into_command.set_command_type(kSqlInvalid);
      return insert_into_command;
    }

    command_stream >> word;
  }

  command_stream >> word;
  if(word != ";"){
    insert_into_command.set_command_type(kSqlInvalid);
    return insert_into_command;
  }

  insert_into_command.set_values(insert_values);
  return insert_into_command;
}

//@author: ryecao
//@brief: quiet 语句，终止程序
void Interpreter::SqlQuit(std::string& command){
  std::exit(1);
}

//@author: ryecao
//@brief: 检查 select from 语句合法性，创建相应语句对象
SqlCommandSelectFrom Interpreter::SqlSelectFrom(std::string& command){
  SqlCommandSelectFrom select_from_command;
  select_from_command.set_command_type(kSqlSelectFrom);
  CommandContentPreProcess(command);
  std::stringstream command_stream(command);
  std::string word;

  command_stream >> word;
  if (word == "*"){
    select_from_command.set_select_all_columns(true);
    command_stream >> word;
    if (LowerCase(word)!="from"){
      select_from_command.set_command_type(kSqlInvalid);
      return select_from_command;
    }
    command_stream >> word;
    select_from_command.set_table_name(word);
  }
  else{
    select_from_command.set_select_all_columns(false);
    while(LowerCase(word)!="from"){
      select_from_command.set_column_names(word);
      command_stream >> word;
      if (LowerCase(word) != "from" && word !=","){
        select_from_command.set_command_type(kSqlInvalid);
        return select_from_command;
      }
      if (LowerCase(word) == "from"){
        break;
      }
      command_stream >> word;
    }
    command_stream >> word;
    select_from_command.set_table_name(word);
  }

  command_stream >> word;
  if (word ==";" && (word != "WHERE" || word != "where")){
    select_from_command.set_select_all_records(true);
    return select_from_command;
  }
  else{
    select_from_command.set_select_all_records(false);
    std::set<std::string> WhereClauseOperators = {"<","<=","=",">=",">","!="};
    WhereClause clause;

    command_stream >> word;
    while(word!=";"){
      clause.kColumnName = word;
      command_stream >> word;
      if (WhereClauseOperators.find(word) != WhereClauseOperators.end()){
        clause.kOperator = word;
      }
      else{
        select_from_command.set_command_type(kSqlInvalid);
        return select_from_command;
      }

      command_stream >> word;
      bool number_flag;
      number_flag = is_number(word);

      if (word != "'" && !number_flag){
        select_from_command.set_command_type(kSqlInvalid);
        return select_from_command;
      }

      if (!number_flag){
        command_stream >> word;
      }
      clause.kCondition = word;

      if(!number_flag){
        command_stream >> word;
      }
      if (word != "'" && !number_flag){
        select_from_command.set_command_type(kSqlInvalid);
        return select_from_command;
      }

      select_from_command.set_where_clause(clause);

      command_stream >> word;

      if (word ==";"){
        break;
      }
      else if (LowerCase(word) != "and"){
        select_from_command.set_command_type(kSqlInvalid);
        return select_from_command;
      }
      command_stream >> word;
    }
  }

  return select_from_command;  
}
