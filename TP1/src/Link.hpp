#pragma once

#include <functional>
#include "PMat.hpp"

#define GFLsqrdist(A,B)                     /***/(SQR((B).x-(A).x)+SQR((B).y-(A).y))

class Link {
public:
    void Connect(PMat& m1, PMat& m2);
    void Hook_Spring(float k);
    void Kinetic_Damper(float z);
    void Damped_Hook(float k, float z);
    void Cond_Damped_Hook(float k, float z, float s);

    void update_Hook();
    void update_Damper();
    void update_Damped_Hook();
    void update_Cond_Damped_Hook();

private:
    float _l0;
    float _k,_l,_z,_s;
    PMat& _m1, & _m2;
    std::function<void()> _update;
};