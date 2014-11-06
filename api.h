// @copyright (c) 2014 ryecao
// @license: MIT
// @author(s): ryecao/ryecao@gmail.com
// created by ryecao on Sept. 28, 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file:api.h
// @brief: Header file of API.
//
// please compile with -std=c++11

#ifndef API_H_
#define API_H_

#include "sql_command.h"
#include "interpreter.h"
#include <string>
#include <iostream>

class Info
{
public:
  Info(){is_succeed_= true;};
  Info(std::string error_info){is_succeed_= false;error_info_ = error_info;};
  bool is_succeed(){return is_succeed_;};
  void PrintInfo(){if (is_succeed_) std::cout<<"Success"<<std::endl; else std::cout<<"Error: "<<error_info_<<std::endl;}
private:
  bool is_succeed_;
  std::string error_info_;
};

class API
{
public:
  void Switcher(SqlCommand*);
private:
  Info CreateTable(SqlCommandCreateTable*);
  Info CreateIndex(SqlCommandCreateIndex*);
  Info DeleteFrom(SqlCommandDeleteFrom*);
  Info DropTable(SqlCommandDropTable*);
  Info DropIndex(SqlCommandDropIndex*); 
  Info InsertInto(SqlCommandInsertInto*);
  Info SelectFrom(SqlCommandSelectFrom*);
};

#endif