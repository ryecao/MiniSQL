// @copyright (c) 2014 ryecao
// @license: MIT
// @author(s): ryecao/ryecao@gmail.com
// created by ryecao on Sept. 28, 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file:attribute_type.cpp
// @brief: attribute type file.
//
// please compile with -std=c++11

#include "attribute_type.h"

std::ostream& operator<<(std::ostream &os,const AttrType &u) {
    os.precision(6);    int t=u.t;
    if(t==0)    os<<u.idata;
    if(t==1)    os<<u.fdata;
    if(t==2)    os<<u.sdata;
    return os;
}