#include "attribute_type.h"

std::ostream& operator<<(std::ostream &os,const AttrType &u) {
    os.precision(6);    int t=u.t;
    if(t==0)    os<<u.idata;
    if(t==1)    os<<u.fdata;
    if(t==2)    os<<u.sdata;
    return os;
}