#pragma once
#include "PMat.hpp"

class Link
{
public:
  enum Type { HOOK, EXTERNAL_FORCE };

  Type _type;

private:
  float _k, _z, _l, _l0, _maxLength;
  PMat &_pmat1;
  PMat &_pmat2;
  glm::vec3 *_g;

  Link();

  Link(PMat &pmat1, glm::vec3 *g) :
      _pmat1(pmat1), _pmat2(pmat1), _type(EXTERNAL_FORCE)
  {
    _g = g;
    _update = [this]() { this->_pmat1.addForce(*_g); };
  }

  Link(PMat &pmat1, PMat &pmat2, float k) :
      _pmat1(pmat1), _pmat2(pmat2), _k(k), _z(k), _type(HOOK)
  {
    _l0 = glm::distance(_pmat1.position(), _pmat2.position());
    _maxLength = _l0 * 1.5f;
  }

  Link(PMat &pmat1, PMat &pmat2, float k, float z) :
      _pmat1(pmat1), _pmat2(pmat2), _k(k), _z(z), _type(HOOK)
  {
    _l0 = glm::distance(_pmat1.position(), _pmat2.position());
    _maxLength = _l0 * 1.5f;
  }

public:
  std::function<void(void)> _update;

  static Link Hook_Spring(PMat &pmat1, PMat &pmat2, float k)
  {
    return Link(pmat1, pmat2, k);
  }

  static Link Hook_Spring(PMat &pmat1, PMat &pmat2, float k, float z)
  {
    return Link(pmat1, pmat2, k, z);
  }

  static Link Extern_Force(PMat &pmat1, glm::vec3 *g) { return Link(pmat1, g); }

  void updateHook()
  {
    glm::vec3 p1 = _pmat1.position(), p2 = _pmat2.position();
    auto d = glm::distance(p1, p2);

    // Avoid float precision issue
    if (d <= 0.0001f) {
      return;
    }

    glm::vec3 u = (p2 - p1) / d;

    // glm::vec3 f = (GFLvector)gfl_mapscal2(u, _k * (d - _l0));
    auto f = _k * (d - p2) * u;

    std::cout << "u.x = " << u.x << std::endl;
    std::cout << "Distance d: " << d << " | Longueur au repos l0: " << _l0
              << std::endl;
    std::cout << "Force: " << f.x << ", " << f.y << std::endl;
    std::cout << "P1: " << p1.x << ", " << p1.y << " | P2: " << p2.x << ", "
              << p2.y << std::endl;

    _pmat1.addForce(f);
    // _pmat2.subForce(f);
    // _pmat2.addForce((GFLvector){-f.x, -f.y});
    _pmat2.subForce(f);
  }

  void update_Damper()
  {
    glm::vec3 v1 = _pmat1.vitesse(), v2 = _pmat2.vitesse();
    // auto deltaV = gfl_SubVect(p1, p2);

    // GFLvector f = (glm::vec3)gfl_mapscal2(deltaV, -_z);
    auto f = _z * (v2 - v1);
    _pmat1.addForce(f);
    _pmat2.subForce(f);
  }

  void update_Damper_Hook()
  {
    glm::vec3 p1 = _pmat1.position(), p2 = _pmat2.position();
    glm::vec3 v1 = _pmat1.vitesse(), v2 = _pmat2.vitesse();

    auto d = glm::distance(p1, p2);
    // auto deltaV = gfl_SubVect(v2, v1);

    auto u = (p2 - p1) / d; // direction

    // GFLvector hook = (GFLvector)gfl_mapscal2(u, _k * (d - _l0));
    // GFLvector damper = (GFLvector)gfl_mapscal2(deltaV, -_z);

    // glm::vec3 f = glm::vec3{hook.x - damper.x, hook.y - damper.y};

    glm::vec3 f = _k * (d - _l0) * u - _z * (v2 - v1);

    _pmat1.addForce(f);
    // _pmat2.addForce((GFLvector){-f.x, -f.y});
    _pmat2.subForce(f);
  }

  void update_Cond_Damper_Hook()
  {
    glm::vec3 p1 = _pmat1.position(), p2 = _pmat2.position();
    glm::vec3 v1 = _pmat1.vitesse(), v2 = _pmat2.vitesse();

    auto d = glm::distance(p1, p2);

    if (d <= 1e-7f || d >= _maxLength) {
      return;
    }

    auto u = (p2 - p1) / d; // direction

    // GFLvector hook = (GFLvector)gfl_mapscal2(u, _k * (d - _l0));
    // GFLvector damper = (GFLvector)gfl_mapscal2(deltaV, -_z);

    // glm::vec3 f = glm::vec3{hook.x - damper.x, hook.y - damper.y};

    glm::vec3 f = _k * (d - _l0) * u - _z * (v2 - v1);

    _pmat1.addForce(f);
    // _pmat2.addForce((GFLvector){-f.x, -f.y});
    _pmat2.subForce(f);
  }

  void update()
  {
    switch (_type) {
    case HOOK:
      update_Cond_Damper_Hook();
      break;

    default:
      _pmat1.addForce(glm::vec3{_g->x, _g->y * -1, _g->z});
      break;
    }
  }

  const PMat &getPmat1() const { return _pmat1; }

  const PMat &getPmat2() const { return _pmat2; }
};