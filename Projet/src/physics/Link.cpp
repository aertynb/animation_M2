#include "Link.hpp"

Link::Link(Type type, std::shared_ptr<PMat> M1, std::shared_ptr<PMat> M2, float k, float z, float s) 
: _k {k}, _z {z}, _s {s}
{
    _M1 = M1; _M2 = M2;
    _l0 = glm::distance(_M1->_pos, _M2->_pos);

    switch(type) {
        case Type::HOOK : {
            _update = [this]() {update_Hook();};
            break;
        }
        case Type::DAMPER : {
            _update = [this]() {update_Damper();};
            break;
        }
        case Type::DAMPED_HOOK : {
            _update = [this]() {update_Damped_Hook();};
            break;
        }
        case Type::COND_DAMPED_HOOK : {
            _update = [this]() {update_Cond_Damped_Hook();};
            break;
        }
    }
}

// Ressort linéaire
void Link::update_Hook() {
    float d = glm::distance(_M1->_pos, _M2->_pos);  // distance courante ∣∣−−−→M1M2∣∣
    std::cout << "distance : " << d << std::endl;
    std::cout << "Rest length _l0 = " << _l0 << std::endl;
    auto u = (_M2->_pos - _M1->_pos)/d;             // direction M1 → M2
    auto F = -_k * (d - _l0) * u;                   // force de rappels
    _M1->_frc -= F; // Fixed point "feels" opposite pull
    _M2->_frc += F; // Moving point "feels" spring pulling it back
}

// Frein cinétique linéaire
void Link::update_Damper() {
    auto F = -_z * (_M2->_vit - _M1->_vit); // force de freinage
    _M1->_frc += F;                         // distribution sur M1
    _M2->_frc -= F;                         // distribution sur M2
}

// Combinaison des 2 (montage en parallèle)
void Link::update_Damped_Hook() {
    float d = glm::distance(_M1->_pos, _M2->_pos);  // distance courante ∣∣−−−→M1M2∣∣
    auto u = (_M2->_pos - _M1->_pos)/d;             // direction M1 → M2
    auto F = -_k * (d - _l0) * u - _z * (_M2->_vit - _M1->_vit);
    _M1->_frc += F;                         // distribution sur M1
    _M2->_frc -= F;                         // distribution sur M2
}

// Liaison Ressort-Frein conditionnelle
// avec L->s=1. : simple "choc" visco-élastique
// avec L->s>1. : lien "inter-particule" - légère adhérence pour d ∈ [l0, s ∗ l0]
void Link::update_Cond_Damped_Hook() {
    float d = glm::distance(_M1->_pos, _M2->_pos);  // distance courante ∣∣−−−→M1M2∣∣
    if (d > _s * _l0) return;

}