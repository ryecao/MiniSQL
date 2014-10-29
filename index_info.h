#ifndef INDEX_INFO_
#define INDEX_INFO_ 

#include <string>

class IndexInfo {
public:
  IndexInfo();
  IndexInfo(const string &index_name, const string &table_name, const string &attribute_name)
  {
    name_ = index_name;
    table_name_ = table_name;
    attribute_name_ = attribute_name;
  };
    
  std::string name();
  std::string table_name();
  std::string attribute_name();

  void set_name(std::string);
  void set_table_name(std::string);
  void set_attribute_name(std:string);
private:
  std::string name_;
  std::string table_name_;
  std::string attribute_name_;
};

#endif