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
#include <string>
#include <set>
#include <algorithm>

//@author: ryecao
//@brief: 构造函数
Interpreter::Interpreter(){
  sql_command_first_word = {"create","drop","select","insert","delete","quit","execfile"};
  sql_command_second_word = {"table","index","from","into"};
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
  std::string command("");

  std::cin >> first_word;
  LowerCase(first_word);
  command = first_word;
  const bool is_in_sql_command_first_word = sql_command_first_word.find(first_word) != sql_command_first_word.end()
  if(is_in_sql_command_first_word == 0){
    return 0;
  }
  else{
  	
  }

  std::cin >> second_word;
  LowerCase(second_word);
  comand = first_word + " " +second_word
  const bool is_in_sql_command_second_word = sql_command_second_word.find(second_word) != sql_command_second_word.end()
  if (is_in_sql_command_second_word == 0){
    return 0;
  }
  else{

  }
  return command;
}
