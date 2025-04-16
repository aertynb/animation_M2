#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <iostream>

class PMat
{

public:
  enum Model { LEAP_FROG, EULER_EXPLICIT, FIXE };
  glm::vec3 _pos;
  

private:
  float _m; // masse
  
  glm::vec3 _vit;
  glm::vec3 _frc; // Vecteur d'accumulation des forces
  std::function<void(float h)> _update;
  Model _model;

  void update_leapFrog(float h)
  {
    _vit.x += h * _frc.x / _m;
    _vit.y += h * _frc.y / _m;

    _pos.x += h * _vit.x;
    _pos.y += h * _vit.y;

    _frc = {0., 0., 0.};
  }

  void update_euler(float h)
  {
    _pos.x += h * _vit.x;
    _pos.y += h * _vit.y;

    _vit.x += h * (_frc.x / _m);
    _vit.y += h * (_frc.y / _m);

    _frc = {0., 0., 0.};
  }

  void update_fixe()
  {
    _frc = {0., 0., 0.}; // Vide le buffer de force
  }

public:
  PMat()
  {
    _m = 1;
    _model = PMat::Model::FIXE;
    _pos = glm::vec3{0.f, 0.f, 0.f};
    _vit = glm::vec3{0.f, 0.f, 0.f};
    _frc = glm::vec3{0.f, 0.f, 0.f};
  }

  PMat(const glm::vec3 &position)
  {
    _m = 1;
    _model = PMat::Model::FIXE;
    _pos = std::move(position);
    _vit = glm::vec3{0.f};
    _frc = glm::vec3{0.f};
  }

  PMat(float m, glm::vec3 P0, glm::vec3 V0, Model model);

  PMat(const PMat &pmat) :
      _m(pmat._m), _update(pmat._update), _model(pmat._model)
  {
    _pos = {pmat._pos};
    _vit = {pmat._vit};
    _frc = {pmat._frc};
  }

  glm::vec3 force() const { return _frc; }

  void addForce(const glm::vec3 &vector)
  {
    // std::cout << "BF add _frc.x " << _frc.x << std::endl;


    _frc += vector;

    // std::cout << "AF add _frc.x " << _frc.x << std::endl;
  }

  void subForce(glm::vec3 &vector) { _frc = vector - _frc; }

  glm::vec3& position() { return _pos; }

  void updateX(float step) { _pos.x += step; }

  void updateY(float step) { _pos.y += step; }

  glm::vec3 vitesse() const { return _vit; }

  void update(float h)
  {
    if (_model == LEAP_FROG) {
      update_leapFrog(h);
      return;
    }
    update_fixe();
  }
};