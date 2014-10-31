#ifndef TABLE_INFO_
#define TABLE_INFO_

#include <string>
#include <map>
#include <vector>

class AttributeInfo {
public:
  AttributeInfo();
  AttributeInfo(std::string& name, int type, int length, std::vector<std::string>& index_names, bool is_unique, bool is_primary_key)
                :name_(name), type_(type), length_(length), index_names_(index_names), is_unique_(is_unique), is_primary_key_(is_primary_key_){};    
  std::string name(){ return name_; }; //get the attribute's name
  int type(){ return type_; }; //return the attribute's type
  int length(){ return length_; }; //return the length of the attribute
  std::vector<std::string> index_names(){ return index_names_; }; //return the index names defined for this attribute
  bool is_unique(){ return is_unique_; }; //return true if the attribute is unique
  bool is_primary_key(){ return is_primary_key_; }; //return true if the attribute is a primary key
    
  void  set_name(const std::string& name){ name_ = name;}; 
  void  set_type(const std::string& type){
    if (type == "int"){
      type_ = 0;
    }
    else if (type == "float"){
      type_ = 1;
    }
    else if (type == "char"){
      type_ = 2;
    }    
  }; 
  void  set_length(const int length){ length_ = length; }; 
  void  set_index_names(const std::vector<std::string> index_names){ index_names_ = index_names; };
  void  set_is_unique(const bool is_unique){ is_unique_ = is_unique; };
  void  set_is_primary_key(const bool is_primary_key){ is_primary_key_ = is_primary_key; }; 
private:
  std::string name_;
  int type_;
  int length_;
  std::vector<std::string> index_names_;
  bool is_unique_;
  bool is_primary_key_;    
};

class TableInfo {
public:
  TableInfo();
  TableInfo(const std::string &table_name, const std::map<std::string,AttributeInfo> &attribute_info)
            :table_name_(table_name), attribute_info_(attribute_info){};
  int attribute_number() const{ return attribute_number_; }; //get the number of table's attributes
  std::string table_name() const{ return table_name_; }; //get the table's name
  AttributeInfo attribute(const std::string& attribute_name){ return attribute_info_[attribute_name]; }; //get the attribute's information    
  bool HasAttribute(const std::string& attribute_name) const{ 
    auto search = attribute_info_.find(attribute_name);  
    if (search != attribute_info_.end()){
      return true;
    }
    else{
      return false;
    }
  }
  std::map<std::string,AttributeInfo> all_attributes() const{ return attribute_info_; };

  void add_attribute(AttributeInfo info){
    auto pointer_to_an_attribute_info = std::make_pair(info.name(), info);
    attribute_info_.insert(pointer_to_an_attribute_info);
  };
  void set_table_name(const std::string& table_name){ table_name_=table_name; }; //set the table's name
  void set_attribute_number(const int attribute_number){ attribute_number_ = attribute_number; }; //set the table's name

private:
  std::string table_name_;
  int attribute_number_;
  std::map<std::string,AttributeInfo> attribute_info_;
};

#endif