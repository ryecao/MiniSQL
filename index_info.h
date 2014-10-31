#ifndef INDEX_INFO_
#define INDEX_INFO_ 

#include <string>

class IndexInfo {
public:
  IndexInfo();
  IndexInfo(const std::string &index_name, const std::string &table_name, const std::string &attribute_name)
  {
    name_ = index_name;
    table_name_ = table_name;
    attribute_name_ = attribute_name;
  };
    
  std::string name() const{ return name_; };
  std::string table_name() const{ return table_name_; };
  std::string attribute_name() const{ return attribute_name_; };

  void set_name(std::string &name){ name_ = name; };
  void set_table_name(std::string &table_name){ table_name_ = table_name; };
  void set_attribute_name(std::string &attribute_name){ attribute_name_ = attribute_name; };
private:
  std::string name_;
  std::string table_name_;
  std::string attribute_name_;
};

#endif