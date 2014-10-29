#ifndef TABLE_INFO_
#define TABLE_INFO_

#include <string>

class AttributeInfo {
public:    
    std::string name();      //get the attribute's name
    int type();      //return the attribute's type
    std::vector<std::string> index_names();   //return the index names defined for this attribute
    bool is_unique();     //return true if this attribute is unique
    bool is_primary_key(); //return true if this attribute is a primary key
};

class TableInfo {
public:
    TableInfo();
    TableInfo(const string &table_name, const vector<AttributeInfo> &attr_info);

    int attribute_number();       //get the number of table's attributes
    std::string table_name();             //get the table's name
    AttributeInfo attribute(int k);        //get the k'th attribute's information    
};

#endif