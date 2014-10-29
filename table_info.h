#ifndef TABLE_INFO_
#define TABLE_INFO_

#include <string>
#include <map>
#include <vector>

enum DataType{
  kInt = 0,
  kChar,
  kFloat,
};

class AttributeInfo {
public:    
  std::string name(); //get the attribute's name
  DataType type(); //return the attribute's type
  int length(); //return the length of the attribute
  std::vector<std::string> index_names(); //return the index names defined for this attribute
  bool is_unique(); //return true if the attribute is unique
  bool is_primary_key(); //return true if the attribute is a primary key
    
  void  set_name(std::string&); 
  void  set_type(std::string&); 
  void  set_length(int); 
  void  set_index_names();
  void  set_is_unique(bool);
  void  set_is_primary_key(bool); 
private:
  std::string name_;
  DataType type_;
  int length_;
  std::vector<std::string> index_names_;
  bool is_unique_;
  bool is_primary_key_;    
};

class TableInfo {
public:
  TableInfo();
  TableInfo(const std::string &table_name, const std::map<std::string,AttributeInfo>&attribute_info);
  int attribute_number(); //get the number of table's attributes
  std::string table_name(); //get the table's name
  AttributeInfo attribute(std::string); //get the attribute's information    
  bool HasAttribute(const std::string &table_name, const std::string &attribute_name);

  void add_attribute(AttributeInfo&);
  void set_table_name(std::string&); //set the table's name
  void set_attribute_number(int); //set the table's name

private:
  std::string table_name_;
  int attribute_number_;
  std::map<std::string,AttributeInfo> attribute_info_;
};

#endif