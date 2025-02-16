#pragma once

#include "PMat.hpp"

class Link {
public:
    void Connect(PMat& m1, PMat& m2);
    void Hook_Spring(float k);
    void Kinetic_Damper(float z);
    void Damped_Hook(float k, float z);
    void Cond_Damped_Hook(float k, float z, float s);
    void update_Hook();
private:
    float _d, _l0;
    float _k,_l,_z,_s;
    PMat& _m1, & _m2;
};