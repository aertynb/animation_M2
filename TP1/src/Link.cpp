#include "Link.hpp"

void Link::Connect(PMat& m1, PMat &m2) {
    _m1 = m1;
    _m2 = m2;
    _d = GFLsqrdistance(_m1._pos, _m2._pos);
}

void Link::update_Hook() {
    float d = GLFsqrdistance(_m1._pos, _m2._pos);
    auto u = GLFvector(_m1._pos, _m2._pos)/_d;
    auto F = -_k*(d-_l0)*u
    _m1._frc += F;
    _m2._frc -= F;
}