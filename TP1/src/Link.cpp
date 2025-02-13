#include "Link.hpp"

void Link::Connect(PMat& m1, PMat &m2) {
    _m1 = m1;
    _m2 = m2;
    _d = GFLsqrdistance(_m1._pos, _m2._pos);
}

void Link::update_Hook() {
    
}