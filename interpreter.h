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
#include <string>
#include "sql_command.h"

//@author: ryecao
//@brief: Interpreter 类
class Interpreter
{   
public:
  Interpreter();
  ~Interpreter();
  SqlCommand* ReadInput();
private:
  SqlCommand* SelectSqlCommand(std::string&, std::string&);
  SqlCommand* ReadInput(std::stringstream&);
  std::string LowerCase(std::string&);
  std::string CommandContentPreProcess(std::string&);
  std::string  ReplacePartInString(std::string&, const std::string&, const std::string&);
  SqlCommandCreateTable SqlCreateTable(std::string&);
  SqlCommandCreateIndex SqlCreateIndex(std::string&);
  SqlCommandDeleteFrom SqlDeleteFrom(std::string&);
  SqlCommandDropTable SqlDropTable(std::string&);
  SqlCommandDropIndex SqlDropIndex(std::string&);
  SqlCommandInsertInto SqlInsertInto(std::string&);
  SqlCommandSelectFrom SqlSelectFrom(std::string&);
  void SqlExecfile(std::string&);
  void SqlQuit(std::string&);
private:
  std::set<std::string> sql_command;
};

#endif
