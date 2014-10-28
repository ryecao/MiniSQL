class AttributeInfo {
public:    
    string          getName();      //get the attribute's name
    int             getType();      //return the attribute's type
    vector<string>  indexNames();   //return the index names defined for this attribute
    bool            isUnique();     //return true if this attribute is unique
};

class TableInfo {
public:
    TableInfo();
    TableInfo(const string &table_name, const vector<AttributeInfo> &attr_info);

    int             getAttributeNumber();       //get the number of table's attributes
    string          getTableName();             //get the table's name
    AttributeInfo   getAttribute(int k);        //get the k'th attribute's information    
}