#include "PMat.hpp"

PMat::PMat(Type type, float& m, const glm::vec3& pos, const glm::vec3& vit)
: _pos {pos}
, _vit {vit}
, _m {m}

{
  switch (type) {
    case Type::LEAP_FROG : {
      // leap frog function
      _update = [this](float h) {update_leapfrog(h);};
      break;
    }
    case Type::FIXE : {
      // fixe function
      _update = [this](float h) {update_fixe(h);};
      break;
    }
    case Type::EULER : {
      // euler function
      _update = [this](float h) {update_euler_exp(h);};
      break;
    }
  }
}

void PMat::update_leapfrog(float h) {
  _vit += h*_frc/_m; // intégration 1 : vitesse m.F(n) = (V(n+1)-V(n))/h -EXplicite
  _pos += h*_vit; // intégration 2 : position V(n+1) = (X(n+1)-X(n))/h -IMplicite
  _frc = glm::vec3(0.f); // on vide le buffer de force
  std::cout << "leap frog : " << glm::to_string(_pos) << std::endl;
  std::cout << "vel: " << glm::to_string(_vit) << std::endl;
}

// juste pour l’exemple : méthode très instable -> à éviter
// simple échange des 2 premières lignes par rapport à Leapfrog -> ça change tout
void PMat::update_euler_exp(float h) {
  _pos += h*_vit; // intégration 1 : position V(n) = (X(n+1)-X(n-1))/h -EXplicite
  _vit += h*_frc/_m; // intégration 2 : vitesse m.F(n) = (V(n+1)-V(n))/h -EXplicite
  _frc = glm::vec3(0.f); // on vide le buffer de force
}

// mise à jour point fixe : ne fait rien
void PMat::update_fixe(float h) {
  // position et vitesse restent inchangées
  std::cout << "Fixe : " << glm::to_string(_pos) << std::endl;
  _frc = glm::vec3(0.f); // on vide le buffer de force (par sécurité)
}