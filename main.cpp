// @copyright (c) 2014 ryecao, sodabeta, Chenhenghong
// @license: MIT
// @author(s): ryecao/ryecao@gmail.com, sodabeta/rxzxx0723@gmail.com, Chenhenghong/450901669@qq.com
// created by ryecao on Sept. 28, 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file:main.cpp
//
// @!:please compile with -std=c++11

#include "interpreter.h"
#include "sql_command.h"
#include "api.h"
#include "buffer_manager.h"
#include "catalog_manager.h"
#include <iostream>
#include <sstream>

int main(int argc, char const *argv[])
{
  Interpreter inter;
  API api;

  std::cout<<"--------------------------------------------------------------------------------"<<std::endl;
  std::cout<<"MiniSQL - a course project of Database System Design."<<std::endl;
  std::cout<<"@Author: Zhendong Cao, Ruixiang Zhang, Henghong Chen."<<std::endl;
  std::cout<<"@Site: http://github.com/ryecao/minisql"<<std::endl;
  std::cout<<"Nov 2014 @ Zhejiang Univ."<<std::endl;
  std::cout<<"--------------------------------------------------------------------------------"<<std::endl;

  while(true){
    std::cout<<">> ";
    SqlCommand* cmd = inter.ReadInput();
    api.Switcher(cmd);
  }
  return 0;
}
