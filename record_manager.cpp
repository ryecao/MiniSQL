#include "record_manager.h"
#include "buffer_manager.h"
#include "block.h"
#include "table_info.h"
#include "index_info.h"
#include "attribute_type.h"

#include <cassert>
#include <string>
#include <sstream>
#define BLOCK_SIZE 4096

struct FileStatus {
	FILE *fp;
	int fileSize;
};

std::map <std::string,std::map <int,int> >  blockStatus;

class blockStatusSaver{
public:
	blockStatusSaver() {}
	~blockStatusSaver() {
		for (auto it=blockStatus.begin();it!=blockStatus.end();it++) {
			std::string fileName=it->first;
			FILE *fp=fopen((fileName+".blockinfo").c_str(),"w");
			for (auto i=blockStatus[fileName].begin();i!=blockStatus[fileName].end();i++) {
				fprintf(fp,"%d %d\n",i->first,i->second);
			}
			fclose(fp);
		}
	}
}BSS;

bool is_Int(std::string s){
	int st=0;
	if(s[0]=='-') st++;
	for(int i=st;i<s.length();i++){
		if( !(s[i]>='0' && s[i]<='9') )
			return false;
	}
	return true;
}

bool is_Float(std::string s){
	int st=0;
	if(s[0]=='-')	st++;
	bool dot=false;
	for(int i=st;i<s.length();i++){
		if(s[i]=='.' && !dot){
			dot=true;
			continue;
		}
		if( !(s[i]>='0' && s[i]<='9') )
			return false;
	}
	return true;
}

int change_Int(std::string s){
	bool negative=false;
	int st=0;
	if(s[0]=='-'){	
		st++;
		negative=true;
	}
	int sum=0;
	for(int i=st;i<s.length();i++)
		sum=sum*10+int(s[i]-'0');
	if(negative)	sum=-sum;
	return sum;
}

float change_Float(std::string s){
	bool negative=false;
	int st=0;
	if(s[0]=='-'){	
		st++;
		negative=true;
	}
	int s1;
	double s2=0.0,ori=1.0;
	bool dot=false;
	for(int i=st;i<s.length();i++){
		if(s[i]=='.'){
			dot=true;
			continue;
		}
		if(!dot)	
			s1=s1*10+int(s[i]-'0');
		else{
			double opt=double(s[i]-'0');
			ori=ori/10.0;
			opt=opt*ori;
			s2=s2+opt;
		}
	}
	s2=s2+1.0*s1;
	return (float)s2;
}

int getEntrySize(const TableInfo &datatable){
	std::vector<std::string> ordered = datatable.attribute_names_ordered();
	auto dataInfo = datatable.all_attributes();		
	int res = 0;
    std::cout<<"attribute_number_getEntrySize: "<<datatable.attribute_number()<<std::endl;//DEBUG
	for (int i=0;i<datatable.attribute_number();i++) {
		std::string target=ordered[i];
		AttributeInfo ATT = dataInfo.at(target);
		res += ATT.length();
	}
	return res;
}

void RecordManager::loadBlockStatus(const std::string &filename) {
	if (blockStatus.find(filename)!=blockStatus.end()) 
		return ;

	FILE *fplist=fopen((filename+".blockinfo").c_str(),"r");
	if (fplist) {
		int offset,occ;
		while (fscanf(fplist,"%d%d",&offset,&occ)!=EOF) {
			blockStatus[filename][offset]=occ;
		}
		fclose(fplist);
	}
}

bool RecordManager::FitinTable(const std::vector<AttrType> &entry, const TableInfo &datatable){//  compare the attribute type with input and target table
	if (entry.size() != datatable.attribute_number()){
		std::cout<<"rm 131:entry.size() != datatable.attribute_number()"<<std::endl;//DEBUG		
		return false;
	} 
	std::vector<std::string> ordered = datatable.attribute_names_ordered(); // attributes without sort
	auto dataInfo = datatable.all_attributes();							// the whole map<std::string,AttributeInfo>
	for (int i=0;i<entry.size();i++){
		std::string target=ordered[i];
		AttributeInfo ATT = dataInfo[target];
		if (ATT.type() != 2){
			if (entry[i].t != ATT.type()) {
				std::cout<<"rm 140: entry[i].t: "<<entry[i].t<<"ATT.type()"<<ATT.type()<<std::endl;//DEBUG		
				return false;
			}
		}
	}
	return true;
}

void RecordManager::entryToBinary(const std::vector<AttrType> &entry, unsigned char * c,const TableInfo &datatable) {
	int totalSize = getEntrySize(datatable);
	memset(c,0,totalSize);
	std::vector<std::string> ordered = datatable.attribute_names_ordered();
	auto dataInfo = datatable.all_attributes();
	for (int i=0;i<entry.size();i++) {
		switch (entry[i].t) {
		case 0: memcpy(c,&entry[i].idata,sizeof(entry[i].idata));c+=sizeof(entry[i].idata);break;
		case 1: memcpy(c,&entry[i].fdata,sizeof(entry[i].fdata));c+=sizeof(entry[i].fdata);break;
		case 2: {
			memcpy(c,entry[i].sdata.c_str(),entry[i].sdata.length());
			std::string target=ordered[i];
			AttributeInfo ATT = dataInfo[target];
			c+=ATT.length();
			break;
		}
		default:assert(0);
		}
	} 
}

static int fetchInt(const unsigned char *loc) {
	int ret;
	memcpy(&ret,loc,sizeof(int));
	return ret;
}

static float fetchFloat(const unsigned char *loc) {
	float ret;
	memcpy(&ret,loc,sizeof(float));
	return ret;
}

static std::string fetchString(const unsigned char *loc,int len) {
	char ret[BLOCK_SIZE];
	memcpy(ret,loc,len);
	ret[len]=0;
	return ret;
}

std::vector <AttrType> RecordManager::binaryToEntry(unsigned char *c,const TableInfo &datatable) {
	std::vector <AttrType> res;
	std::vector<std::string> ordered = datatable.attribute_names_ordered();
	auto dataInfo = datatable.all_attributes();		
	for (int i=0;i<datatable.attribute_number();i++) {
		std::string target=ordered[i];
		AttributeInfo ATT = dataInfo[target];
		switch(ATT.type()) {
			case 0:res.push_back(fetchInt(c));c+=sizeof(int);break;
			case 1:res.push_back(fetchFloat(c));c+=sizeof(float);break;
			case 2:{
				res.push_back(fetchString(c,ATT.length()));
				c+=ATT.length();
				break;
			}
			default:assert(0);
		}
	}
	return res;
}

std::vector<AttrType> ChangetoAttr(const std::vector<std::string>& values,const TableInfo &datatable){
	std::vector<AttrType> res;
	std::string target;
	std::vector<std::string> ordered = datatable.attribute_names_ordered();
	auto dataInfo = datatable.all_attributes();	
	for(int i=0;i<values.size();i++){	
		target=values[i];
		std::string mask = ordered[i];
		AttributeInfo ATT = dataInfo[mask];
		if(ATT.type()==2){  // deal with '1234' as string instead of int
			AttrType temp(target);
			res.push_back(temp);
			continue;
		}

		if(is_Int(target)){
			AttrType temp(change_Int(target));
			res.push_back(temp);
			continue;
		}
		if(is_Float(target)){
			AttrType temp(change_Float(target));
			res.push_back(temp);
			continue;
		}
		AttrType temp(target);
		res.push_back(temp);
	}
	return res;
}

bool RecordManager::FitterTest(const std::vector <AttrType> &data, WhereClause where_clause, const TableInfo& datatable){
	std::string name = where_clause.kColumnName;
	std::string opt = where_clause.kOperator;
	std::string value = where_clause.kCondition;
	int pos = datatable.attribute_index(name);
	AttrType ATT = data[pos]; // target attribute for where_clause
	std::stringstream stream;
	AttrType clause;
	if(ATT.t==0){ // int
		int v;
		stream << value;
		stream >> v;
		AttrType c(v);
		clause = c;
	}

	if(ATT.t==1){ //float
		double p;
		stream << value;
		stream >> p;
		AttrType c(p);
		clause = c;
	}
	
	if(ATT.t==2){
		AttrType c(value);
		clause = c;
	} // string

	//compare
	if(opt=="="){
		if(ATT == clause)
			return true;
	}
	
	if(opt=="<"){
		if(ATT < clause)
				return true;
	}

	if(opt==">"){
		if(ATT > clause)
				return true;
	}

	if(opt=="<="){
		if( ATT <= clause)
				return true;
	}

	if(opt==">="){
		if( ATT >= clause)
				return true;
	}

	if(opt=="!="){
		if( ATT != clause)
				return true;
	}

	return false;
}

int RecordManager::InsertRecord(const TableInfo &datatable, const std::vector<std::string>& values){
	std::string filename=datatable.table_name()+".db";
	std::cout<<"attribute_number_InsertRecord: "<<datatable.attribute_number()<<std::endl;//DEBUG

	std::vector<AttrType> entry = ChangetoAttr(values,datatable);
	if(!FitinTable(entry,datatable)) // attribute dismatch
		return -1;
	loadBlockStatus(filename);

	int totalSize = getEntrySize(datatable);
	int capacity = BLOCK_SIZE/(totalSize+1); // the capacity can be saved in one block
	int offset=-1;
	for(auto it=blockStatus[filename].begin();it!=blockStatus[filename].end();it++){
		if(it->second < capacity){
			offset=it->first;
			break;
		}
	}
	Block block;
	BufferManager T;
	if(offset==-1){
		block=T.AllocateNewBlock(filename);
		offset=block.pos;
		blockStatus[filename][offset]=0;
	}
	else{
		block=T.GetBlock(filename,offset);
	}

	bool Find=false;
	for (int i=0;i<capacity;i++) {
		if (!block.data[i]) {	// add capacity_size first ,then add each tuple. example:[1][2][3][tuple1][tuple2][tuple3]
			entryToBinary(entry, block.data + capacity + i*totalSize, datatable);  
			Find=true;  
			block.data[i]=true;
			break;
		}
	}
	assert(Find);

	std:: string tablename = datatable.table_name();
	while (*(tablename.end()-1)!='.')
		tablename.erase(tablename.end()-1);
	tablename.erase(tablename.end()-1);			
	std::vector<std::string> ordered = datatable.attribute_names_ordered(); 
	auto dataInfo = datatable.all_attributes();							
	T.WriteBlock(block);
	blockStatus[filename][offset]++;
	std::cout<<"rm 342"<<std::endl;//DEBUG
	return offset;
}

std::vector<std::pair<int,int>> RecordManager::FindRecordsWithIndex(const std::vector<int> offsets, const TableInfo& datatable,WhereClause where_clause){
	std::vector<std::pair<int,int>> ret;
	std::string filename = datatable.table_name()+".db";
	loadBlockStatus(filename);
	int totalSize = getEntrySize(datatable);
	BufferManager T;

	for(int i=0;i<offsets.size();i++){
		int offset = offsets[i];
		Block block = T.GetBlock(filename,offset);
		int capacity = BLOCK_SIZE/(totalSize+1);
		
		unsigned char *c=block.data+capacity;
		for(int k=0;k<capacity;k++){
			if(block.data[k]){ //exist data kth
				std::vector<AttrType> entry = binaryToEntry(c,datatable);
				if(FitterTest(entry,where_clause,datatable))
					ret.push_back( std::make_pair(offset,capacity + k*totalSize) );   //example:[1][2][3][tuple1][tuple2][tuple3] start from 0
			}
			c+=totalSize;
		}
	}
	return ret;
}

std::vector<std::pair<int,int>> RecordManager::FindRecordsWithNoIndex(const TableInfo& datatable,WhereClause where_clause){
	std::vector<std::pair<int,int>> ret;
	std::string filename = datatable.table_name()+".db";
	loadBlockStatus(filename);
	int totalSize = getEntrySize(datatable);
	BufferManager T;

	for(auto it=blockStatus[filename].begin();it!=blockStatus[filename].end();it++){
		int offset = it->first;
		if(it->second ==0)
			continue;
		Block block = T.GetBlock(filename,offset);
		int capacity = BLOCK_SIZE/(totalSize+1);

		unsigned char *c=block.data+capacity;
		for(int k=0;k<capacity;k++){
			if(block.data[k]){ //exist data kth
				std::vector<AttrType> entry = binaryToEntry(c,datatable);
				if(FitterTest(entry,where_clause,datatable))
					ret.push_back( std::make_pair(offset,capacity + k*totalSize) );   //example:[1][2][3][tuple1][tuple2][tuple3] start from 0
			}
			c+=totalSize;
		}
	}
	return ret;
}

std::vector<std::pair<int,int>> RecordManager::RecordsFilter(std::vector<std::pair<int,int>> record,const TableInfo& datatable,WhereClause where_clause){
	std::vector<std::pair<int,int>> ret;
	std::string filename = datatable.table_name()+".db";
	loadBlockStatus(filename);
	BufferManager T;

	for(auto it=record.begin();it!=record.end();it++){
		int offset = it->first;
		int pos = it->second;
		Block block = T.GetBlock(filename,offset);

		unsigned char *c=block.data+pos;
		std::vector<AttrType> entry = binaryToEntry(c,datatable);
		if(FitterTest(entry,where_clause,datatable))
			ret.push_back(std::make_pair(offset,pos));
	}
	return ret;
}

bool RecordManager::DeleteRecords(std::vector<std::pair<int,int>> offsets, const TableInfo& datatable){
	bool success =true;
	std::string filename = datatable.table_name()+".db";
	loadBlockStatus(filename);
	int totalSize = getEntrySize(datatable);
	BufferManager T;

	for(auto it=offsets.begin();it!=offsets.end();it++){
		int offset = it->first;
		int pos = it->second;
		Block block = T.GetBlock(filename,offset);
		int capacity = BLOCK_SIZE/(totalSize+1);

		int tuple = (pos-capacity)/totalSize; // the ith tuple start with capacity + i*totalSize
		if(block.data[tuple]==0) // no exist
			success=false;
		block.data[tuple]=0; // lazy erase, only erase the label of tuple
	}
	return success;
}

std::string attChangeToString(AttrType x){
	std::stringstream stream;
    std::string result;
	if(x.t==0){ //int
		stream << x.idata;
		stream >> result;
		return result;
	}

	if(x.t==1){ //float
		stream << x.fdata;
		stream >> result;
		return result;
	}	

	return x.sdata; // x.t==2 
}

std::vector <std::vector<std::string>> RecordManager::SelectRecords(std::vector<std::pair<int,int>> offsets, const TableInfo &datatable){
	std::vector<std::vector<std::string>> ret;
	std::string filename = datatable.table_name()+".db";
	loadBlockStatus(filename);
	int totalSize = getEntrySize(datatable);
	BufferManager T;
	for(auto it=offsets.begin();it!=offsets.end();it++){
		int offset = it->first;
		int pos = it->second;
		Block block = T.GetBlock(filename,offset);
		
		unsigned char *c=block.data+pos;
		std::vector<AttrType> entry = binaryToEntry(c,datatable);
		std::vector<std::string> each_ret;
		for(auto k : entry){
			std::string target = attChangeToString(k);
			each_ret.push_back(target);
		}
		ret.push_back(each_ret);
	}
	return ret;
}

std::vector <std::vector<std::string>> RecordManager::SelectAllRecords(const TableInfo& datatable){
	std::vector<std::vector<std::string>> ret;
	std::string filename = datatable.table_name()+".db";
	loadBlockStatus(filename);
	int totalSize = getEntrySize(datatable);
	BufferManager T;

	for(auto it=blockStatus[filename].begin();it!=blockStatus[filename].end();it++){
		int offset = it->first;
		if(it->second ==0)
			continue;
		Block block = T.GetBlock(filename,offset);
		int capacity = BLOCK_SIZE/(totalSize+1);
		
		unsigned char *c=block.data+capacity;
		for(int i=0;i<capacity;i++){
			if(block.data[i]){ //exist data ith
				std::vector<AttrType> entry = binaryToEntry(c,datatable);
				std::vector<std::string> each_ret;
				for(auto k : entry){
					std::string target = attChangeToString(k);
					each_ret.push_back(target);
				}
				ret.push_back(each_ret);
			}
			c+=totalSize;
		}
	}
	return ret;
}

bool RecordManager::DeleteAllRecords(const TableInfo& datatable){
	bool opt=false;
	std::string filename = datatable.table_name()+".db";
	loadBlockStatus(filename);
	int totalSize = getEntrySize(datatable);
	BufferManager T;
	for(auto it=blockStatus[filename].begin();it!=blockStatus[filename].end();it++){
		int offset = it->first;
		if(it->second ==0)
			continue;
		Block block = T.GetBlock(filename,offset);
		int capacity = BLOCK_SIZE/(totalSize+1);

		for(int i=0;i<capacity;i++){
			if(block.data[i]) //  exist
				opt=true;
			block.data[i]=0; //erase
		}
	}
	return opt;
}
