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
#include <string>
#include <set>
#include <algorithm>

//@author: ryecao
//@brief: 构造函数
Interpreter::Interpreter(){
  sql_command = {"create table","create index","delete from","drop table","drop index","execfile",
                 "insert into","quit","select"};
}

//@author: ryecao
//@brief: 把命令转换为小写
//@parms: raw_string: 转换为小写之前的字符串
//@return: raw_string: 转换为小写后的字符串
std::string Interpreter::LowerCase(std::string& raw_string){
  std::transform(raw_string.begin(), raw_string.end(), raw_string.begin(), ::tolower);
  return raw_string
}

//@author: ryecao
//@breif: 读入
//@return: 命令名是否合法
//TODO
bool Interpreter::ReadInput(){
  std::string first_word;
  std::string second_word;
  std::string command_type("");
  std::string command_content("");

  std::cin >> first_word;
  LowerCase(first_word);

  command_type = first_word;
  
  if(first_word != "select"){
    std::cin >> second_word;
    LowerCase(second_word);

    command_type = first_word + " " +second_word;
    const bool is_in_sql_command = sql_command.find(command_type) != sql_command.end();

    if (is_in_sql_command == 0){
      return false;
    }
  }

  bool flag_read_not_finish = true;
  std::string temp_word;

  while(flag_read_not_finish){
    std::cin >> temp_word;
    if (temp_word.back() ==';'){
      flag_read_not_finish = false;
    }
    command_content = command_content + " " +temp_word;
  }

  return true;
}

SqlCommand Interpreter::SqlCreateTable(std::string& command){
  
}

SqlCommand Interpreter::SqlCreateIndex(std::string& command){

}
SqlCommand Interpreter::SqlDeleteFrom(std::string& command){

}
SqlCommand Interpreter::SqlDropTable(std::string& command){

}
SqlCommand Interpreter::SqlDropIndex(std::string& command){

}
SqlCommand Interpreter::SqlExecfile(std::string& command){

}
SqlCommand Interpreter::SqlInsertInto(std::string& command){

}
SqlCommand Interpreter::SqlQuit(std::string& command){

}
SqlCommand Interpreter::SqlSelectFrom(std::string& command){

}
