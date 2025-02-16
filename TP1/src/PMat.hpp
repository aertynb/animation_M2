#pragma once

#include <functional>
#include <gfl.h>

class PMat {
public:
    PMat(float m, const GFLpoint& pos, const GFLvector& vit, const GFLvector& frc, int type)
    : _m {m}, _pos {pos}, _vit {vit}, _frc {frc}
    {
        switch (type)// choix de la fonction de mise à jour
        {
            case 0 : _update = &PMat::update_fixe; break;// Point Fixe
            case 1 : _update = &PMat::update_leapfrog; break;// Particule Leapfrog
            case 2 : _update = &PMat::update_euler_exp; break;// Particule Euler Exp.
        }
    }

    void update_leapfrog(float h);
    void update_euler_exp(float h);
    void update_fixe(float h);

    float _m;
    GFLpoint _pos;
    GFLvector _vit;
    GFLvector _frc;
    void (PMat::*_update)(float h) = nullptr; // Function pointer
};
