#include "Link.hpp"

void Link::Connect(PMat& m1, PMat &m2) {
    _m1 = m1;
    _m2 = m2;
    _l0 = GFLsqrdist(_m1._pos, _m2._pos);
}

void Link::update_Hook() {
    float d = GFLsqrdist(_m1._pos, _m2._pos);
    auto u = GFLvector(_m1._pos, _m2._pos)/d;
    auto F = -_k*(d-_l0)*u;
    _m1._frc += F;
    _m2._frc -= F;
}

void Link::update_Damper() {
    auto F = -_z*(_m2._vit-_m1._vit);
    _m1._frc += F;
    _m2._frc -= F;
}

void Link::update_Damped_Hook() {
    float d = GFLsqrdist(_m1._pos, _m1._pos);
    GFLvector u = GFLvector(_m1._pos, _m2._pos)/d;
    GFLvector F = -_k*(d-_l0)*u-_z*(_m2->_vit-_m1->_vit);
    _m1._frc += F;
    _m2._frc -= F;
}

void Link::Cond_Damped_Hook() {
    float d = GFLsqrdist(_m1._pos, _m2._pos);
    if (d>_s*_l0) return;
    GFLvector u = GFLvector(L->M1->pos,L->M2->pos)/d; // direction M1 → M2
    GFLvector F = -_k*(d-_l0)*u -_z*(_m2->_vit-_m1->_vit); // force combinées
    L->M1->frc += F; // distribution sur M1
    L->M2->frc -= F; // distribution sur M2
}

// Les constructeurs : choix de la nature de la liaison
// Ressort linéaire
void Link::Hook_Spring(float k)
{
    _k = k;
    _update = [this]() { update_Hook(); };;
}
// Frein cinétique linéaire
void Link::Kinetic_Damper(float z)
{
    _z = z;
    _update = [this]() {update_Damper();};
}
// Combinaison des 2 (montage en parallèle)
void Link::Damped_Hook(float k, float z)
{
    _k = k;
    _z = z;
    _update = [this]() {update_Damped_Hook;};
}
// Liaison Ressort-Frein conditionnelle
void Link::Cond_Damped_Hook(float k, float z, float s)
{
    _k = k;
    _z = z;
    _s = s;
    _update = [this]() {update_Cond_Damped_Hook();};
}