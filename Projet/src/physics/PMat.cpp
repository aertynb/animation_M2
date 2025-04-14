#include "PMat.hpp"

PMat::PMat(float m, glm::vec3 P0, glm::vec3 V0, PMat::Model model) :
    _m(m), _pos(P0), _vit(V0), _model(model), _frc{0.f}
{
  switch (model) {
  case LEAP_FROG:
    _update = [this](float h) { update_leapFrog(h); };
    break;

  default:
    _update = [this](float h) { update_euler(h); };
    break;
  }
}