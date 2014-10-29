#ifndef ATTRIBUTE_TYPE_
#define ATTRIBUTE_TYPE_ value

struct AttrType {
    int idata;
    double fdata;
    string sdata;
    int t;
    AttrType():             t(-1)           {}
    AttrType(int d):        idata(d),t(0)    {}
    AttrType(double d):     fdata(d),t(1)    {}
    AttrType(string d):     sdata(d),t(2)    {}    
    friend bool operator<(const AttrType &lhs, const AttrType &rhs) {
        int t=lhs.t;
        if(t==0)    return lhs.idata<rhs.idata;
        if(t==1)    return dcmp(lhs.fdata-rhs.fdata)<0;
        if(t==2)    return lhs.sdata<rhs.sdata;
        throw "AttrType Not Found Type";
    }
    friend bool operator>(const AttrType &lhs, const AttrType &rhs) {
        int t=lhs.t;
        if(t==0)    return lhs.idata>rhs.idata;
        if(t==1)    return dcmp(lhs.fdata-rhs.fdata)>0;
        if(t==2)    return lhs.sdata>rhs.sdata;
        throw "AttrType Not Found Type";
    }
    friend bool operator<=(const AttrType &lhs, const AttrType &rhs) {
        return !(lhs>rhs);
    }
    friend bool operator>=(const AttrType &lhs, const AttrType &rhs) {
        return !(lhs<rhs);
    }
    friend bool operator==(const AttrType &lhs, const AttrType &rhs) {
        int t=lhs.t;
        if(t==0)    return lhs.idata==rhs.idata;
        if(t==1)    return dcmp(lhs.fdata-rhs.fdata)==0;
        if(t==2)    return lhs.sdata==rhs.sdata;
        throw "AttrType Not Found Type";
    }
    friend bool operator!=(const AttrType &lhs, const AttrType &rhs) {
        return !(lhs==rhs);
    }
};

#endif