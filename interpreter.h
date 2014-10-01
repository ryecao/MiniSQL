// @copyright (c) 2014 ryecao
// @license: MIT
// @author(s): ryecao/ryecao@gmail.com
// created by ryecao on Sept. 28, 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file:interpreter.h
// @brief: Header file of an interpreter used for reading in SQL command users type in, check
// the rightness and convert it into inner format used by other module of the MiniSQL
// database

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <iostream>
#include <set>
#include "sql_command.h"

//@author: ryecao
//@brief: Interpreter 类
class Interpreter
{
public:
  Interpreter();
  ~Interpreter();
  bool ReadInput();
private:
  std::string LowerCase();
  SqlCommand SqlCreateTable(std::string& command);
  SqlCommand SqlCreateIndex(std::string& command);
  SqlCommand SqlDeleteFrom(std::string& command);
  SqlCommand SqlDropTable(std::string& command);
  SqlCommand SqlDropIndex(std::string& command);
  SqlCommand SqlExecfile(std::string& command);
  SqlCommand SqlInsertInto(std::string& command);
  SqlCommand SqlQuit(std::string& command);
  SqlCommand SqlSelectFrom(std::string& command);
private:
  set<std::string> sql_command;
};

#endif
