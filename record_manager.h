#ifndef RECORD_MANAGER_H
#define RECORD_MANAGER_H

#include "table_info.h"
#include "attribute_type.h"
<<<<<<< HEAD
=======
#include "sql_command.h"
>>>>>>> 9066a870eda922a78cec972b5019057908e63623
#include <vector>
#include <string>
#include <set>

class RecordManager{
public:
<<<<<<< HEAD
  //@brief: insert into 语句
  //@params: 插入表的信息; 插入的值.
  int InsertRecord(const TableInfo &table, const std::vector<std::string>& values);  

  //@brief: 根据 Index 送进来的 offset 找到block，在这些 block 中找到符合条件的 tuple.
  //@params: block offsets, table info, 一条 where 从句;
  //@return:vector<block_offset_in_file, tuple_position_in_block>.
  std::vector<std::pair<int,int>> FindRecordsWithIndex(const std::vector<int> offsets, const TableInfo& table,WhereClause where_clause);
 
  //@brief: 无index，遍历所有 block 中的所有tuple, 找到符合条件的 tuple.
  //@params: table info, 一条 where 从句;
  //@return: vector<block_offset_in_file, tuple_position_in_block>.
  std::vector<std::pair<int,int>> FindRecordsWithNoIndex(const TableInfo& table,WhereClause where_clause);
  
  //@brief: 根据 block 偏移量和 tuple 在 block 中的位置删除某条记录.
  //@params: vector<block_offset_in_file, tuple_position_in_block>, table info
  void DeleteRecords(std::vector<std::pair<int,int>> offsets, const TableInfo& table); 
  
  //@brief: 根据 block 偏移量和 tuple 在 block 中的位置删除某条记录.
  //@params: vector<block_offset_in_file, tuple_position_in_block>, table info
  //@return: 包含符合条件的记录的vector.
  std::vector <std::vector<std::string>> SelectRecords(std::vector<std::pair<int,int>> offsets, const TableInfo &table); 
private:
  bool FitterTest(const std::vector <AttrType> &data); // Fitter
  std::vector <AttrType> binaryToEntry(unsigned char *c,const TableInfo &table);
  void loadBlockStatus(const std::string &fileName);
  bool FitinTable(const std::vector<AttrType> &entry, const TableInfo &table);
  static int FetchInt(const unsigned char *loc);
  static float FetchFloat(const unsigned char *loc);
  static std::string FetchString(const unsigned char *loc,int len);
  std::map <std::string,std::map <int,int> >  blockStatus;
=======
	int rmInsertRecord(const std::string &fileName,const std::vector<AttrType> &entry, const TableInfo &datatable);  
	void rmAddIndex(const std::string dbName,const std::string BTreeName, const TableInfo &datatable, int itemIndex);
	void rmDeleteWithIndex(const std::string fileName,int offset, const std::vector<WhereClause> &constraint, const TableInfo &datatable); 
	void rmDeleteWithoutIndex(const std::string fileName, const std::vector<WhereClause> &constraint, const TableInfo &datatable);	
	std::vector <std::vector <AttrType> > rmSelectWithIndex(const std::string fileName, int offset, const std::vector<WhereClause> &constraint, const TableInfo &datatable); 
	std::vector <std::vector <AttrType> > rmSelectWithoutIndex(const std::string fileName, const std::vector<WhereClause> &constraint, const TableInfo &datatable);	
	std::set<int> rmGetAllOffsets(const std::string &fileName);
	void rmClear(const std::string fileName) ;
private:
	void loadBlockStatus(const std::string &fileName);
	bool FitinTable(const std::vector <AttrType> &entry, const TableInfo &datatable);
	void entryToBinary(const std::vector <AttrType> &entry, unsigned char * c,const TableInfo &datatable);
	std::vector <AttrType> binaryToEntry(unsigned char *c,const TableInfo &datatable);
	bool FitterTest(const std::vector<WhereClause> &constraint, const std::vector <AttrType> &data); 
	

>>>>>>> 9066a870eda922a78cec972b5019057908e63623
};

#endif
