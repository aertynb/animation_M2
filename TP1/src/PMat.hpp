#pragma once

#include <gfl.h>
#include <gfl_geom2d.h>

class PMat {
public:
    PMat(float m, const GFLpoint& pos, const GFLvector& vit, const GFLvector& frc)
    : _m {m}, _pos {pos}, _vit {vit}, _frc {frc}
    {}

    void update_leapfrog(float h);
    void update_euler_exp(float h);
    void update_fixe(float h);

    float _m;
    GFLpoint _pos;
    GFLvector _vit;
    GFLvector _frc;
    void (*update)(float h);
};