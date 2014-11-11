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