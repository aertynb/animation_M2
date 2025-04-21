#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>



struct PMat
{  
  enum Type {
    LEAP_FROG, FIXE, EULER
  };

  float &_m; // masse
  glm::vec3 _pos; // position
  glm::vec3 _vit; // vitesse
  glm::vec3 _frc; // buffer d'accumulation des forces
  std::function<void(float)> _update;

  PMat(Type type, float& m, const glm::vec3& pos, const glm::vec3& vit);

  // intégrateur Leapfrog
  void update_leapfrog(float h);
  // intégrateur Euler Explicite
  void update_euler_exp(float h);
  // mise à jour point fixe : ne fait rien
  void update_fixe(float h);
};