#ifndef ATTRIBUTE_TYPE_
#define ATTRIBUTE_TYPE_ value

int dcmp(double x) {
    return fabs(x)<1e-9 ? 0 : x<0 ? -1 : 1;
}
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
std::ostream& operator<<(std::ostream &os,const AttrType &u) {
    os.precision(6);    int t=u.t;
    if(t==0)    os<<u.idata;
    if(t==1)    os<<u.fdata;
    if(t==2)    os<<u.sdata;
    return os;
}

#endif
