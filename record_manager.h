// @copyright (c) 2014 Chenhenghong, ryecao 
// @license: MIT
// @author(s): Chenhenghong/450901669@qq.com, ryecao/ryecao@gmail.com 
// created by ryecao on Sept. 28, 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file:record_manager.h
// @brief: used to handle insertion, search and deletion.
//
// @!:please compile with -std=c++11

#ifndef RECORD_MANAGER_H_
#define RECORD_MANAGER_H_

#include <vector>
#include <string>
#include <set>
#include "table_info.h"
#include "sql_command.h"
#include "attribute_type.h"

class RecordManager{
public:
    ~RecordManager();
  //@brief: insert into 语句
  //@params: 插入表的信息; 插入的值.
  //@return: offset
  int InsertRecord(const TableInfo &table, const std::vector<std::string>& values);  

  //@brief: 根据 Index 送进来的 offset 找到block，在这些 block 中找到符合条件的 tuple.
  //@params: block offsets, table info, 一条 where 从句;
  //@return:vector<block_offset_in_file, tuple_position_in_block>.
  std::vector<std::pair<int,int>> FindRecordsWithIndex(const std::vector<int>& offsets, const TableInfo& table,WhereClause where_clause);
 
  //@brief: 无index，遍历所有 block 中的所有tuple, 找到符合条件的 tuple.
  //@params: table info, 一条 where 从句;
  //@return: vector<block_offset_in_file, tuple_position_in_block>.
  std::vector<std::pair<int,int>> FindRecordsWithNoIndex(const TableInfo& table,WhereClause where_clause);
  
  //@brief: 在结果中筛选出符合某条件的记录
  //@params: <block offset，record position in block>, table info, 一条 where 从句;
  //@return: vector<block_offset_in_file, tuple_position_in_block>.
  std::vector<std::pair<int,int>> RecordsFilter(std::vector<std::pair<int,int>>& record,const TableInfo& table,WhereClause where_clause);

  //@brief: 根据 block 偏移量和 tuple 在 block 中的位置删除某条记录.
  //@params: vector<block_offset_in_file, tuple_position_in_block>, table info
  bool DeleteRecords(std::vector<std::pair<int,int>>& offsets, const TableInfo& table); 
  
  //@return: 包含符合条件的记录的vector.
  //!!! 请务必保持传入的vector的每个 <块号，块内位置>与返回的vector 中的每个记录中的位置一一对应!!!
  //!!! 也就是 vector_of_results.at(i) 对应 vector_of_pairs.at(i) !!!
  //!!! 务必务必 !!!
  std::vector <std::vector<std::string>> SelectRecords(std::vector<std::pair<int,int>>& offsets, const TableInfo &table); 

  std::vector <std::pair<std::vector<std::string>,int>> SelectAllRecords(const TableInfo& table);

  bool DeleteAllRecords(const TableInfo& table);
private:
  bool FitterTest(const std::vector <AttrType> &data, WhereClause where_clause,const TableInfo &table); 
  std::vector <AttrType> binaryToEntry(unsigned char *c,const TableInfo &table);
  void entryToBinary(const std::vector<AttrType> &entry, unsigned char * c,const TableInfo &table);
  void loadBlockStatus(const std::string &filename);
  bool FitinTable(const std::vector<AttrType> &entry, const TableInfo &table);
};

#endif
