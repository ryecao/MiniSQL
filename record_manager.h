#ifndef RECORD_MANAGER_
#define RECORD_MANAGER_

#include "table_info.h"
#include "attribute_type.h"
#include "sql_command.h"
#include <vector>
#include <string>
#include <set>

class RocordManager{
public:
	int rmInsertRecord(const std::string &fileName,const std::vector<AttrType> &entry, const TableInfo &datatable);  
	void rmAddIndex(const std::string dbName,const std::string BTreeName, const TableInfo &datatable, int itemIndex);
	void rmDeleteWithIndex(const std::string fileName,int offset, const std::vector<WhereClause> &constraint, const TableInfo &datatable); 
	void rmDeleteWithoutIndex(const std::string fileName, const std::vector<WhereClause> &constraint, const TableInfo &datatable);	
	std::vector <std::vector <AttrType> > rmSelectWithIndex(const std::string fileName, int offset, const std::vector<WhereClause> &constraint, const TableInfo &datatable); 
	std::vector <std::vector <AttrType> > rmSelectWithoutIndex(const std::string fileName, const std::vector<WhereClause> &constraint, const TableInfo &datatable);	
	std::set<int> rmGetAllOffsets(const std::string &fileName);
	void rmClear(const std::string fileName) ;
private:
	bool FitterTest(const std::vector<WhereClause> &constraint, const std::vector <AttrType> &data); // Fitter
	std::vector <AttrType> binaryToEntry(unsigned char *c,const TableInfo &datatable);
	void loadBlockStatus(const std::string &fileName);
	bool FitinTable(const std::vector<AttrType> &entry, const TableInfo &datatable);
	static int FetchInt(const unsigned char *loc);
	static float FetchFloat(const unsigned char *loc);r
	static std::string FetchString(const unsigned char *loc,int len);
	std::map <std::string,std::map <int,int> >  blockStatus;
};

#endif
