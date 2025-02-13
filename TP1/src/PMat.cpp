#include "PMat.hpp"

void PMat::update_leapfrog(float h) {
    _vit += h*_frc/_m;
    _pos += h*_vit;
    _frc = GLFvector(0.f, 0.f);
}

void PMat::update_euler_exp(float h) {
    _pos += h*_vit;
    _vit += h*_frc/_m;
    _frc = GLFvector(0.f, 0.f);
}

void PMat::update_fixe(float h) {
    _frc = GLFvector(0.f, 0.f);
}