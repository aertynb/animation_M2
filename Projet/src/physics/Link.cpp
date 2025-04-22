#include "Link.hpp"

Link::Link(Type type, std::shared_ptr<PMat> M1, std::shared_ptr<PMat> M2, float& k, float& z, float& s) 
: _k {k}, _z {z}, _s {s}
{
    _M1 = M1; _M2 = M2;
    _l0 = glm::distance(_M1->_pos, _M2->_pos);

    setType(type);
}

// Ressort linéaire
void Link::update_Hook() {
    float d = glm::distance(_M1->_pos, _M2->_pos);  // distance courante ∣∣−−−→M1M2∣∣
    // std::cout << "distance : " << d << std::endl;
    // std::cout << "Rest length _l0 = " << _l0 << std::endl;
    auto u = (_M2->_pos - _M1->_pos)/d;             // direction M1 → M2
    auto F = -_k * (d - _l0) * u;                   // force de rappels
    _M1->_frc -= F; // Fixed point "feels" opposite pull
    _M2->_frc += F; // Moving point "feels" spring pulling it back
}

// Frein cinétique linéaire
void Link::update_Damper() {
    glm::vec3 dir = _M2->_pos - _M1->_pos;
    float dist = glm::length(dir);
    if (dist == 0.f) return;

    glm::vec3 n = dir / dist; // normalise la direction

    glm::vec3 v_rel = _M2->_vit - _M1->_vit;
    float v_rel_n = glm::dot(v_rel, n); // la direction et la vitesse de M2

    glm::vec3 f_damp = -_z * v_rel_n * n;

    _M1->_frc -= f_damp;
    _M2->_frc += f_damp;
}

// Combinaison des 2 (montage en parallèle)
void Link::update_Damped_Hook() {
    float d = glm::distance(_M1->_pos, _M2->_pos);  // distance courante ∣∣−−−→M1M2∣∣
    auto u = (_M2->_pos - _M1->_pos)/d;             // direction M1 → M2
    auto F = -_k * (d - _l0) * u - _z * (_M2->_vit - _M1->_vit);
    _M1->_frc -= F;                         // distribution sur M1
    _M2->_frc += F;                         // distribution sur M2
}

// Liaison Ressort-Frein conditionnelle
// avec L->s=1. : simple "choc" visco-élastique
// avec L->s>1. : lien "inter-particule" - légère adhérence pour d ∈ [l0, s ∗ l0]
void Link::update_Cond_Damped_Hook() {
    float d = glm::distance(_M1->_pos, _M2->_pos);  // distance courante ∣∣−−−→M1M2∣∣
    if (d > _s * _l0) return;
    auto u = (_M2->_pos - _M1->_pos)/d;             // direction M1 → M2
    auto F = -_k * (d - _l0) * u - _z * (_M2->_vit - _M1->_vit);
    _M1->_frc -= F;                         // distribution sur M1
    _M2->_frc += F;   
}